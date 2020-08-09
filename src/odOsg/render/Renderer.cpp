/*
 * Renderer.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */


#include <odOsg/render/Renderer.h>

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <odCore/Logger.h>
#include <odCore/LevelObject.h>
#include <odCore/Light.h>
#include <odCore/Layer.h>
#include <odCore/Level.h>
#include <odCore/OdDefines.h>
#include <odCore/Downcast.h>

#include <odCore/render/RendererEventListener.h>
#include <odCore/render/GuiCallback.h>

#include <odCore/db/Model.h>

#include <odOsg/GlmAdapter.h>
#include <odOsg/Constants.h>

#include <odOsg/render/Geometry.h>
#include <odOsg/render/Image.h>
#include <odOsg/render/Texture.h>
#include <odOsg/render/Camera.h>
#include <odOsg/render/Group.h>
#include <odOsg/render/Handle.h>
#include <odOsg/render/Model.h>
#include <odOsg/render/ModelBuilder.h>

namespace odOsg
{


    Renderer::Renderer()
    : mShaderFactory("resources/shader_src")
    , mEventListener(nullptr)
    , mFreeLook(false)
    , mLightingEnabled(true)
    , mSimTime(0.0)
    {
        mViewer = new osgViewer::Viewer;

        mCamera = std::make_shared<Camera>(mViewer->getCamera());

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
        statsHandler->setKeyEventPrintsOutStats(0);
        statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
        mViewer->addEventHandler(statsHandler);

        mViewer->setKeyEventSetsDone(osgGA::GUIEventAdapter::KEY_Escape);

        mSceneRoot = new osg::Group;
        mViewer->setSceneData(mSceneRoot);

        // set up root state
        osg::StateSet *ss = mSceneRoot->getOrCreateStateSet();
        ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

        ss->setDefine("MAX_LIGHTS", std::to_string(Constants::MAX_LIGHTS));
        ss->setDefine("MAX_BONES", std::to_string(Constants::MAX_BONES));

        mGlobalLightDiffuse   = new osg::Uniform("layerLightDiffuse",   osg::Vec3(0.0, 0.0, 0.0));
        mGlobalLightAmbient   = new osg::Uniform("layerLightAmbient",   osg::Vec3(0.0, 0.0, 0.0));
        mGlobalLightDirection = new osg::Uniform("layerLightDirection", osg::Vec3(0.0, 1.0, 0.0));
        ss->addUniform(mGlobalLightDiffuse);
        ss->addUniform(mGlobalLightAmbient);
        ss->addUniform(mGlobalLightDirection);

        mLocalLightsColor     = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightDiffuse", Constants::MAX_LIGHTS);
        mLocalLightsIntensity = new osg::Uniform(osg::Uniform::FLOAT, "objectLightIntensity", Constants::MAX_LIGHTS);
        mLocalLightsRadius    = new osg::Uniform(osg::Uniform::FLOAT, "objectLightRadius", Constants::MAX_LIGHTS);
        mLocalLightsPosition  = new osg::Uniform(osg::Uniform::FLOAT_VEC3, "objectLightPosition", Constants::MAX_LIGHTS);
        ss->addUniform(mLocalLightsColor);
        ss->addUniform(mLocalLightsIntensity);
        ss->addUniform(mLocalLightsRadius);
        ss->addUniform(mLocalLightsPosition);

        ss->setAttribute(mShaderFactory.getProgram("default"), osg::StateAttribute::ON);

        mLevelRoot = new osg::Group;
        mSceneRoot->addChild(mLevelRoot);

        _setupGuiStuff();
    }

    Renderer::~Renderer()
    {
        if(mViewer != nullptr)
        {
            mViewer->setDone(true);
        }
    }

    void Renderer::setRendererEventListener(odRender::RendererEventListener *listener)
    {
        mEventListener = listener;
    }

    void Renderer::setEnableLighting(bool b)
    {
        mLightingEnabled = b;

        if(!b)
        {
            mGlobalLightAmbient->set(osg::Vec3(1.0, 1.0, 1.0));
        }
    }

    bool Renderer::isLightingEnabled() const
    {
        return mLightingEnabled;
    }

    std::shared_ptr<odRender::Handle> Renderer::createHandle(odRender::RenderSpace space)
    {
        auto newHandle = std::make_shared<Handle>(*this);

        moveToRenderSpace(newHandle, space);

        return newHandle;
    }

    std::shared_ptr<odRender::Model> Renderer::createModel()
    {
        return std::make_shared<Model>();
    }

    std::shared_ptr<odRender::Geometry> Renderer::createGeometry(odRender::PrimitiveType primitiveType, bool indexed)
    {
        return std::make_shared<Geometry>(primitiveType, indexed);
    }

    std::shared_ptr<odRender::Group> Renderer::createGroup(odRender::RenderSpace space)
    {
        auto newGroup = std::make_shared<Group>();

        moveToRenderSpace(newGroup, space);

        return newGroup;
    }

    std::shared_ptr<odRender::Model> Renderer::createModelFromDb(std::shared_ptr<odDb::Model> model)
    {
        OD_CHECK_ARG_NONNULL(model);

        std::shared_ptr<Model> renderModel;
        if(model->getLodInfoVector().empty())
        {
            renderModel = _buildSingleLodModelNode(*model);

        }else
        {
            renderModel = _buildMultiLodModelNode(*model);
        }

        // assign correct lighting modes. note that some of the shading types are baked into the geometry (flat shading)
        if(model->getShadingType() == odDb::Model::ShadingType::None)
        {
            renderModel->setLightingMode(odRender::LightingMode::OFF);

        }else if(!model->isShiny())
        {
            renderModel->setLightingMode(odRender::LightingMode::AMBIENT_DIFFUSE);

        }else
        {
            renderModel->setLightingMode(odRender::LightingMode::AMBIENT_DIFFUSE_SPECULAR);
        }

        osg::ref_ptr<osg::Program> modelProgram = getShaderFactory().getProgram("model");
        modelProgram->addBindAttribLocation("influencingBones", Constants::ATTRIB_INFLUENCE_LOCATION);
        modelProgram->addBindAttribLocation("vertexWeights", Constants::ATTRIB_WEIGHT_LOCATION);
        renderModel->getGeode()->getOrCreateStateSet()->setAttribute(modelProgram, osg::StateAttribute::ON);

        return renderModel;
    }

    std::shared_ptr<odRender::Model> Renderer::createModelFromLayer(od::Layer *layer)
    {
        ModelBuilder mb(*this, "layer " + layer->getName(), layer->getLevel());
        mb.setCWPolygonFlag(false);
        mb.setUseClampedTextures(true);

        uint32_t width = layer->getWidth();
        uint32_t height = layer->getHeight();
        const std::vector<od::Layer::Vertex> &layerVertices = layer->getVertexVector();
        const std::vector<od::Layer::Cell> &layerCells = layer->getCellVector();

        std::vector<glm::vec3> vertices;
        vertices.reserve(layerVertices.size());
        for(size_t i = 0; i < layerVertices.size(); ++i)
        {
            size_t aXRel = i%(width+1);
            size_t aZRel = i/(width+1); // has to be an integer operation to floor it

            vertices.push_back(glm::vec3(aXRel, layerVertices[i].heightOffsetLu, aZRel));
        }
        mb.setVertexVector(std::move(vertices));

        std::vector<odDb::Model::Polygon> polygons; // TODO: move the Polygon struct somewhere where it belongs
        polygons.reserve(layer->getVisibleTriangleCount());
        for(size_t triIndex = 0; triIndex < width*height*2; ++triIndex)
        {
            size_t cellIndex = triIndex/2;
            bool isLeft = (triIndex%2 == 0);
            od::Layer::Cell cell = layerCells[cellIndex];
            odDb::Model::Polygon poly;
            poly.vertexCount = 3;
            poly.texture = isLeft ? cell.leftTextureRef : cell.rightTextureRef;
            poly.doubleSided = (layer->getLayerType() == od::Layer::TYPE_BETWEEN);

            if(poly.texture == od::Layer::HoleTextureRef || poly.texture == od::Layer::InvisibleTextureRef)
            {
                continue;
            }

            int aZRel = cellIndex/width; // has to be an integer operation to floor it

            // calculate indices of corner vertices
            // z x> --a------b---
            // V      | cell | cell
            //        |  #n  | #n+1
            //      --c------d---
            size_t a = cellIndex + aZRel; // add row index since we want to skip top right vertex in every row passed so far
            size_t b = a + 1;
            size_t c = a + (width+1); // one row below a, one row contains width+1 vertices
            size_t d = c + 1;

            glm::vec2 uvA(cell.texCoords[3]/0xffff, cell.texCoords[7]/0xffff);
            glm::vec2 uvB(cell.texCoords[2]/0xffff, cell.texCoords[6]/0xffff);
            glm::vec2 uvC(cell.texCoords[0]/0xffff, cell.texCoords[4]/0xffff);
            glm::vec2 uvD(cell.texCoords[1]/0xffff, cell.texCoords[5]/0xffff);

            if(!(cell.isBackslashCell()))
            {
                if(isLeft)
                {
                    poly.vertexIndices[0] = c;
                    poly.vertexIndices[1] = b;
                    poly.vertexIndices[2] = a;
                    poly.uvCoords[0] = uvC;
                    poly.uvCoords[1] = uvB;
                    poly.uvCoords[2] = uvA;

                }else
                {
                    poly.vertexIndices[0] = c;
                    poly.vertexIndices[1] = d;
                    poly.vertexIndices[2] = b;
                    poly.uvCoords[0] = uvC;
                    poly.uvCoords[1] = uvD;
                    poly.uvCoords[2] = uvB;
                }

            }else // division = BACKSLASH
            {
                if(isLeft)
                {
                    poly.vertexIndices[0] = a;
                    poly.vertexIndices[1] = c;
                    poly.vertexIndices[2] = d;
                    poly.uvCoords[0] = uvA;
                    poly.uvCoords[1] = uvC;
                    poly.uvCoords[2] = uvD;

                }else
                {
                    poly.vertexIndices[0] = a;
                    poly.vertexIndices[1] = d;
                    poly.vertexIndices[2] = b;
                    poly.uvCoords[0] = uvA;
                    poly.uvCoords[1] = uvD;
                    poly.uvCoords[2] = uvB;
                }
            }

            if(layer->getLayerType() == od::Layer::TYPE_CEILING)
            {
                // swap two vertices, thus reversing the winding order
                std::swap(poly.vertexIndices[0], poly.vertexIndices[1]);
                std::swap(poly.uvCoords[0], poly.uvCoords[1]);
            }

            polygons.push_back(poly);
        }
        mb.setPolygonVector(polygons.begin(), polygons.end());

        std::shared_ptr<Model> builtModel = mb.build();

        osg::ref_ptr<osg::Program> layerProg = getShaderFactory().getProgram("layer");
        builtModel->getGeode()->getOrCreateStateSet()->setAttribute(layerProg, osg::StateAttribute::ON);

        return builtModel;
    }

    std::shared_ptr<odRender::Image> Renderer::createImageFromDb(std::shared_ptr<odDb::Texture> dbTexture)
    {
        OD_CHECK_ARG_NONNULL(dbTexture);

        return std::make_shared<Image>(dbTexture);;
    }

    std::shared_ptr<odRender::Texture> Renderer::createTexture(std::shared_ptr<odRender::Image> image, odRender::TextureReuseSlot reuseSlot)
    {
        OD_CHECK_ARG_NONNULL(image);

        auto odOsgImage = od::confident_downcast<Image>(image);

        switch(reuseSlot)
        {
        case odRender::TextureReuseSlot::NONE:
            return std::make_shared<Texture>(odOsgImage);

        case odRender::TextureReuseSlot::OBJECT:
            if(odOsgImage->getSharedObjectTexture().expired())
            {
                auto newTexture = std::make_shared<Texture>(odOsgImage);
                newTexture->setEnableWrapping(true);
                odOsgImage->getSharedObjectTexture() = newTexture;
                return newTexture;
            }
            return std::shared_ptr<odRender::Texture>(odOsgImage->getSharedObjectTexture());

        case odRender::TextureReuseSlot::LAYER:
            if(odOsgImage->getSharedLayerTexture().expired())
            {
                auto newTexture = std::make_shared<Texture>(odOsgImage);
                newTexture->setEnableWrapping(false);
                odOsgImage->getSharedLayerTexture() = newTexture;
                return newTexture;
            }
            return std::shared_ptr<odRender::Texture>(odOsgImage->getSharedLayerTexture());
        }

        OD_UNREACHABLE();
    }

    void Renderer::moveToRenderSpace(std::shared_ptr<odRender::Handle> handle, odRender::RenderSpace space)
    {
        OD_CHECK_ARG_NONNULL(handle);

        auto myHandle = od::confident_downcast<Handle>(handle);

        if(myHandle->getParentOsgGroup() != nullptr)
        {
            myHandle->getParentOsgGroup()->removeChild(myHandle->getOsgNode());
        }

        auto newParentGroup = _getOsgGroupForRenderSpace(space);
        myHandle->setParentOsgGroup(newParentGroup);
        if(newParentGroup != nullptr)
        {
            newParentGroup->addChild(myHandle->getOsgNode());
        }
    }

    void Renderer::moveToRenderSpace(std::shared_ptr<odRender::Group> group, odRender::RenderSpace space)
    {
        OD_CHECK_ARG_NONNULL(group);

        auto myGroup = od::confident_downcast<Group>(group);

        if(myGroup->getParentOsgGroup() != nullptr)
        {
            myGroup->getParentOsgGroup()->removeChild(myGroup->getOsgNode());
        }

        auto newParentGroup = _getOsgGroupForRenderSpace(space);
        myGroup->setParentOsgGroup(newParentGroup);
        if(newParentGroup != nullptr)
        {
            newParentGroup->addChild(myGroup->getOsgNode());
        }
    }

    void Renderer::addGuiCallback(odRender::GuiCallback *callback)
    {
        OD_CHECK_ARG_NONNULL(callback);

        mGuiCallbacks.push_back(callback);
    }

    void Renderer::removeGuiCallback(odRender::GuiCallback *callback)
    {
        OD_CHECK_ARG_NONNULL(callback);

        auto it = std::find(mGuiCallbacks.begin(), mGuiCallbacks.end(), callback);
        if(it != mGuiCallbacks.end())
        {
            mGuiCallbacks.erase(it);
        }
    }

    glm::vec2 Renderer::getFramebufferDimensions()
    {
        if(mWindow == nullptr)
        {
             // no window created yet. return a default size. a resize event will fix this later
            return { 640, 480 };

        }else
        {
            int x, y, width, height;
            mWindow->getWindowRectangle(x, y, width, height);
            return { width, height };
        }
    }

    odRender::Camera *Renderer::getCamera()
    {
        return mCamera.get();
    }

    void Renderer::setup()
    {
        mViewer->realize();

        osgViewer::Viewer::Windows windows;
        mViewer->getWindows(windows, true);
        if(windows.empty()) throw od::Exception("Viewer created no windows");
        mWindow = windows[0];

        mWindow->setWindowName("OpenDrakan (OSG)");

        if(!mFreeLook)
        {
            mWindow->setCursor(osgViewer::GraphicsWindow::NoCursor);
        }

        int x, y, width, height;
        mWindow->getWindowRectangle(x, y, width, height);

        for(auto guiCallback : mGuiCallbacks)
        {
            guiCallback->onFramebufferResize({width, height});
        }

        double aspect = static_cast<double>(width)/height;
        mViewer->getCamera()->setProjectionMatrixAsPerspective(45, aspect, 1, 10000);
    }

    void Renderer::shutdown()
    {
        if(mViewer != nullptr)
        {
            mViewer->setDone(true);
            mViewer = nullptr;
        }
    }

    void Renderer::frame(float relTime)
    {
        mSimTime += relTime;

        // TODO: frame rate limiter ("timeUntilNextFrame" or smth)

        mViewer->advance(mSimTime);
        mViewer->eventTraversal();
        mViewer->updateTraversal();
        for(auto guiCallback : mGuiCallbacks)
        {
            guiCallback->onUpdate(relTime);
        }
        mViewer->renderingTraversals();
    }

    void Renderer::applyLayerLight(const osg::Matrix &viewMatrix, const osg::Vec3 &diffuse, const osg::Vec3 &ambient, const osg::Vec3 &direction)
    {
        if(!mLightingEnabled)
        {
            return;
        }

        mGlobalLightDiffuse->set(diffuse);
        mGlobalLightAmbient->set(ambient);

        osg::Vec4 dirCs = osg::Vec4(direction, 0.0) * viewMatrix;
        mGlobalLightDirection->set(osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void Renderer::applyToLightUniform(const osg::Matrix &viewMatrix, od::Light &light, size_t index)
    {
        if(index >= mLocalLightsColor->getNumElements())
        {
            throw od::InvalidArgumentException("Tried to apply light at out-of-bounds index");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLocalLightsColor->setElement(index, GlmAdapter::toOsg(light.getColor()));
        mLocalLightsIntensity->setElement(index, light.getIntensityScaling());
        mLocalLightsRadius->setElement(index, light.getRadius());

        osg::Vec3 posWs = GlmAdapter::toOsg(light.getPosition());
        osg::Vec4 dirCs = osg::Vec4(posWs, 1.0) * viewMatrix;
        mLocalLightsPosition->setElement(index, osg::Vec3(dirCs.x(), dirCs.y(), dirCs.z()));
    }

    void Renderer::applyNullLight(size_t index)
    {
        if(index >= mLocalLightsColor->getNumElements())
        {
            throw od::InvalidArgumentException("Tried to apply null light at out-of-bounds index");
        }

        if(!mLightingEnabled)
        {
            return;
        }

        mLocalLightsColor->setElement(index, osg::Vec3(0.0, 0.0, 0.0));
        mLocalLightsIntensity->setElement(index, 0.0f);
    }

    void Renderer::setFreeLook(bool f)
    {
        mFreeLook = f;

        mCamera->setIgnoreViewChanges(f);

        if(f)
        {
            mViewer->setCameraManipulator(new osgGA::TrackballManipulator, true);

        }else
        {
            mViewer->setCameraManipulator(nullptr, false);
        }
    }

    osg::Group *Renderer::_getOsgGroupForRenderSpace(odRender::RenderSpace space)
    {
        switch(space)
        {
        case odRender::RenderSpace::NONE:
            return nullptr;

        case odRender::RenderSpace::LEVEL:
            return mLevelRoot;

        case odRender::RenderSpace::GUI:
            return mGuiRoot;
        }

        OD_UNREACHABLE();
    }

    void Renderer::_setupGuiStuff()
    {
        osg::Matrix guiSpaceToNdc = osg::Matrix::scale(2.0, -2.0, -1.0);
        guiSpaceToNdc.setTrans(-1.0, 1.0, 0.0);
        mNdcToGuiSpaceTransform.invert(guiSpaceToNdc);

        mGuiCamera = new osg::Camera;
        mGuiCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
        mGuiCamera->setProjectionMatrix(osg::Matrix::ortho2D(-1, 1, -1, 1));
        mGuiCamera->setViewMatrix(guiSpaceToNdc);
        mGuiCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
        mGuiCamera->setRenderOrder(osg::Camera::POST_RENDER);
        mGuiCamera->setAllowEventFocus(false);
        mSceneRoot->addChild(mGuiCamera);

        mGuiRoot = new osg::Group;
        mGuiRoot->setCullingActive(false);
        osg::StateSet *ss = mGuiRoot->getOrCreateStateSet();
        ss->setMode(GL_BLEND, osg::StateAttribute::ON);
        ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
        //ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
        mGuiCamera->addChild(mGuiRoot);
    }

    std::shared_ptr<Model> Renderer::_buildSingleLodModelNode(odDb::Model &model)
    {
        ModelBuilder mb(*this, model.getName(), model.getAssetProvider());

        mb.setCWPolygonFlag(true);
        mb.setBuildSmoothNormals(model.getShadingType() != odDb::Model::ShadingType::Flat);
        mb.setVertexVector(model.getVertexVector().begin(), model.getVertexVector().end());
        mb.setPolygonVector(model.getPolygonVector().begin(), model.getPolygonVector().end());

        return mb.build();
    }

    std::shared_ptr<Model> Renderer::_buildMultiLodModelNode(odDb::Model &model)
    {
        const std::vector<odDb::Model::LodMeshInfo> &lodMeshInfos = model.getLodInfoVector();
        const std::vector<glm::vec3> &vertices = model.getVertexVector();
        const std::vector<odDb::Model::Polygon> &polygons = model.getPolygonVector();

        for(auto it = lodMeshInfos.begin(); it != lodMeshInfos.end(); ++it)
        {
            ModelBuilder mb(*this, model.getName() + " (LOD '" + it->lodName + "')", model.getAssetProvider());

            mb.setCWPolygonFlag(true);
            mb.setBuildSmoothNormals(model.getShadingType() != odDb::Model::ShadingType::Flat);

            // the count fields in the mesh info sometimes do not cover all vertices and polygons. gotta be something with those "LOD caps"
            //  instead of using those values, use all vertices up until the next lod until we figure out how else to handle this
            size_t actualVertexCount = ((it+1 == lodMeshInfos.end()) ? vertices.size() : (it+1)->firstVertexIndex) - it->firstVertexIndex;
            size_t actualPolyCount = ((it+1 == lodMeshInfos.end()) ? polygons.size() : (it+1)->firstPolygonIndex) - it->firstPolygonIndex;

            auto verticesBegin = vertices.begin() + it->firstVertexIndex;
            auto verticesEnd = vertices.begin() + actualVertexCount + it->firstVertexIndex;
            mb.setVertexVector(verticesBegin, verticesEnd);

            auto polygonsBegin = polygons.begin() + it->firstPolygonIndex;
            auto polygonsEnd = polygons.begin() + actualPolyCount + it->firstPolygonIndex;
            mb.setPolygonVector(polygonsBegin, polygonsEnd);

            auto bonesBegin = it->boneAffections.begin();
            auto bonesEnd = it->boneAffections.end();
            mb.setBoneAffectionVector(bonesBegin, bonesEnd);

            return mb.build(); // FIXME: ignoring LODs past the first for now, as our unified concept does not support this ATM

            /*float minDistance = it->distanceThreshold*OD_WORLD_SCALE;
            float maxDistance = ((it+1) == lodMeshInfos.end()) ? std::numeric_limits<float>::max() : (it+1)->distanceThreshold*OD_WORLD_SCALE;
            size_t lodIndex = addLod(minDistance*0, maxDistance*10000);

            addGeometry(geometry, lodIndex);*/
        }

        OD_UNREACHABLE();
    }

}
