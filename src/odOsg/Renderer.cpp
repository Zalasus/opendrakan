/*
 * Renderer.cpp
 *
 *  Created on: 12 Nov 2018
 *      Author: zal
 */


#include <odOsg/Renderer.h>

#include <memory>

#include <osgGA/TrackballManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <odCore/Logger.h>
#include <odCore/LevelObject.h>

#include <odOsg/ObjectNode.h>
#include <odOsg/ModelNode.h>
#include <odOsg/Geometry.h>
#include <odOsg/GeometryBuilder.h>

namespace odOsg
{

    Renderer::Renderer()
    : mShaderFactory("resources/shader_src")
    {
        mViewer = new osgViewer::Viewer;

        osg::ref_ptr<osgViewer::StatsHandler> statsHandler(new osgViewer::StatsHandler);
        statsHandler->setKeyEventPrintsOutStats(0);
        statsHandler->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
        mViewer->addEventHandler(statsHandler);

        mViewer->setCameraManipulator(new osgGA::TrackballManipulator, true);
        mViewer->setKeyEventSetsDone(osgGA::GUIEventAdapter::KEY_Escape);

        mSceneRoot = new osg::Group;
        mSceneRoot->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("default"));
        mViewer->setSceneData(mSceneRoot);

        mObjects = new osg::Group;
        mObjects->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("model"));
        mSceneRoot->addChild(mObjects);

        mLayers = new osg::Group;
        mLayers->getOrCreateStateSet()->setAttribute(mShaderFactory.getProgram("layer"));
        mSceneRoot->addChild(mLayers);
    }

    Renderer::~Renderer()
    {
        if(mRenderThread.joinable() && mViewer != nullptr)
        {
            Logger::warn() << "Render thread was not stopped when renderer was destroyed";
            mViewer->setDone(true);
            mRenderThread.join();
        }
    }

    void Renderer::onStart()
    {
        mRenderThread = std::thread(&Renderer::_threadedRender, this);
    }

    void Renderer::onEnd()
    {
        if(mRenderThread.joinable() && mViewer != nullptr)
        {
            mViewer->setDone(true);
            mRenderThread.join();
        }
    }

    void Renderer::setEnableLighting(bool b)
    {
    }

    bool Renderer::isLightingEnabled() const
    {
        return false;
    }

    odRender::ObjectNode *Renderer::createObjectNode(od::LevelObject &obj)
    {
        return new ObjectNode(this, mObjects);
    }

    odRender::ModelNode *Renderer::createModelNode(odDb::Model &model)
    {
        od::RefPtr<ModelNode> node(new ModelNode(this));

        if(model.getLodInfoVector().empty())
        {
            _buildSingleLodModelNode(model, node);

        }else
        {
            _buildMultiLodModelNode(model, node);
        }

        if(model.getShadingType() == odDb::Model::ShadingType::None)
        {
            node->setLightingMode(ModelNode::LightingMode::Off);

        }else if(!model.isShiny())
        {
            node->setLightingMode(ModelNode::LightingMode::AmbientDiffuse);

        }else
        {
            node->setLightingMode(ModelNode::LightingMode::AmbientDiffuseSpecular);
        }

        return node.release();
    }

    void Renderer::_threadedRender()
    {
        static const double maxFrameRate = 60;

        mViewer->realize();

        double simTime = 0;
        double frameTime = 0;
        while(!mViewer->done())
        {
            double minFrameTime = (maxFrameRate > 0.0) ? (1.0/maxFrameRate) : 0.0;
            osg::Timer_t startFrameTick = osg::Timer::instance()->tick();

            {
                std::lock_guard<std::mutex> lock(mRenderMutex);

                mViewer->advance(simTime);
                mViewer->eventTraversal();
                mViewer->updateTraversal();
                mViewer->renderingTraversals();
            }

            osg::Timer_t endFrameTick = osg::Timer::instance()->tick();
            frameTime = osg::Timer::instance()->delta_s(startFrameTick, endFrameTick);
            simTime += frameTime;
            if(frameTime < minFrameTime)
            {
                simTime += (minFrameTime-frameTime);
                std::this_thread::sleep_for(std::chrono::microseconds(1000000*static_cast<size_t>(minFrameTime-frameTime)));
            }
        }

        // FIXME: we need to notify the engine that it should shut down here
    }

    void Renderer::_buildSingleLodModelNode(odDb::Model &model, ModelNode *node)
    {
        od::RefPtr<Geometry> geometry = new Geometry;

        GeometryBuilder gb(*geometry, model.getName(), model.getAssetProvider());
        gb.setBuildSmoothNormals(model.getShadingType() != odDb::Model::ShadingType::Flat);
        gb.setVertexVector(model.getVertexVector().begin(), model.getVertexVector().end());
        gb.setPolygonVector(model.getPolygonVector().begin(), model.getPolygonVector().end());
        gb.build();

        node->addGeometry(geometry);
    }

    void Renderer::_buildMultiLodModelNode(odDb::Model &model, ModelNode *node)
    {
        const std::vector<odDb::Model::LodMeshInfo> &lodMeshInfos = model.getLodInfoVector();
        const std::vector<glm::vec3> &vertices = model.getVertexVector();
        const std::vector<odDb::Model::Polygon> &polygons = model.getPolygonVector();

        for(auto it = lodMeshInfos.begin(); it != lodMeshInfos.end(); ++it)
        {
            od::RefPtr<Geometry> geometry = new Geometry;

            GeometryBuilder gb(*geometry, model.getName() + " (LOD '" + it->lodName + "')", model.getAssetProvider());
            gb.setBuildSmoothNormals(model.getShadingType() != odDb::Model::ShadingType::Flat);

            // the count fields in the mesh info sometimes do not cover all vertices and polygons. gotta be something with those "LOD caps"
            //  instead of using those values, use all vertices up until the next lod until we figure out how else to handle this
            size_t actualVertexCount = ((it+1 == lodMeshInfos.end()) ? vertices.size() : (it+1)->firstVertexIndex) - it->firstVertexIndex;
            size_t actualPolyCount = ((it+1 == lodMeshInfos.end()) ? polygons.size() : (it+1)->firstPolygonIndex) - it->firstPolygonIndex;

            auto verticesBegin = vertices.begin() + it->firstVertexIndex;
            auto verticesEnd = vertices.begin() + actualVertexCount + it->firstVertexIndex;
            gb.setVertexVector(verticesBegin, verticesEnd);

            auto polygonsBegin = polygons.begin() + it->firstPolygonIndex;
            auto polygonsEnd = polygons.begin() + actualPolyCount + it->firstPolygonIndex;
            gb.setPolygonVector(polygonsBegin, polygonsEnd);

            auto bonesBegin = it->boneAffections.begin();
            auto bonesEnd = it->boneAffections.end();
            gb.setBoneAffectionVector(bonesBegin, bonesEnd);

            gb.build();

            float minDistance = it->distanceThreshold;
            float maxDistance = ((it+1) == lodMeshInfos.end()) ? std::numeric_limits<float>::max() : (it+1)->distanceThreshold;
            size_t lodIndex = node->addLod(minDistance, maxDistance);

            node->addGeometry(geometry, lodIndex);
        }
    }

}
