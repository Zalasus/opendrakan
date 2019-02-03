/*
 * ModelNode.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#include <odOsg/render/ModelNode.h>

#include <odCore/OdDefines.h>
#include <odCore/Exception.h>

#include <odCore/db/Model.h>

#include <odOsg/render/Renderer.h>
#include <odOsg/render/Geometry.h>
#include <odOsg/render/GeometryBuilder.h>

namespace odOsg
{

    ModelNode::ModelNode(Renderer *renderer, odDb::Model *model)
    : mRenderer(renderer)
    , mModel(model)
    , mGeometryGroup(new osg::Group)
    {
        if(model->getLodInfoVector().empty())
        {
            _buildSingleLodModelNode();

        }else
        {
            _buildMultiLodModelNode();
        }

        if(model->getShadingType() == odDb::Model::ShadingType::None)
        {
            setLightingMode(ModelNode::LightingMode::Off);

        }else if(!model->isShiny())
        {
            setLightingMode(ModelNode::LightingMode::AmbientDiffuse);

        }else
        {
            setLightingMode(ModelNode::LightingMode::AmbientDiffuseSpecular);
        }

        osg::ref_ptr<osg::Program> modelProgram = mRenderer->getShaderFactory().getProgram("model");
        modelProgram->addBindAttribLocation("influencingBones", OD_ATTRIB_INFLUENCE_LOCATION);
        modelProgram->addBindAttribLocation("vertexWeights", OD_ATTRIB_WEIGHT_LOCATION);
        mGeometryGroup->getOrCreateStateSet()->setAttribute(modelProgram);
    }

    ModelNode::~ModelNode()
    {
    }

    const std::vector<std::pair<float, float>> &ModelNode::getLods()
    {
        return mLods;
    }

    odRender::Geometry *ModelNode::getGeometry(size_t lodIndex)
    {
        if(lodIndex >= mGeometries.size())
        {
            throw od::Exception("LOD index out of bounds");
        }

        return mGeometries[lodIndex];
    }

    size_t ModelNode::addLod(float minDistance, float maxDistance)
    {
        mLods.push_back(std::make_pair(minDistance, maxDistance));

        return mLods.size() - 1;
    }

    void ModelNode::addGeometry(Geometry *g)
    {
        if(g == nullptr)
        {
            return;
        }

        mGeometryGroup->addChild(g->getOsgGeode());
        mGeometries.push_back(g);
    }

    void ModelNode::addGeometry(Geometry *g, size_t lodIndex)
    {
        if(g == nullptr)
        {
            return;
        }

        if(lodIndex >= mLods.size())
        {
            throw od::Exception("LOD index out of bounds");
        }

        if(mLodNode == nullptr)
        {
            mLodNode = new osg::LOD;
            mGeometryGroup->addChild(mLodNode);
        }

        std::pair<float, float> &lod = mLods[lodIndex];

        mLodNode->addChild(g->getOsgGeode(), lod.first, lod.second);
        mGeometries.push_back(g);
    }

    void ModelNode::setLightingMode(LightingMode lm)
    {
        osg::StateSet *ss = mGeometryGroup->getOrCreateStateSet();

        switch(lm)
        {
        case LightingMode::Off:
            ss->removeDefine("LIGHTING");
            ss->removeDefine("SPECULAR");
            break;

        case LightingMode::AmbientDiffuse:
            ss->setDefine("LIGHTING");
            ss->removeDefine("SPECULAR");
            break;

        case LightingMode::AmbientDiffuseSpecular:
            ss->setDefine("LIGHTING");
            ss->setDefine("SPECULAR");
            break;
        }
    }

    osg::Node *ModelNode::getOsgNode()
    {
        return mGeometryGroup;
    }

    void ModelNode::_buildSingleLodModelNode()
    {
        auto geometry = od::make_refd<Geometry>();

        GeometryBuilder gb(mRenderer, *geometry, mModel->getName(), mModel->getAssetProvider());
        gb.setBuildSmoothNormals(mModel->getShadingType() != odDb::Model::ShadingType::Flat);
        gb.setVertexVector(mModel->getVertexVector().begin(), mModel->getVertexVector().end());
        gb.setPolygonVector(mModel->getPolygonVector().begin(), mModel->getPolygonVector().end());
        gb.build();

        addGeometry(geometry);
    }

    void ModelNode::_buildMultiLodModelNode()
    {
        const std::vector<odDb::Model::LodMeshInfo> &lodMeshInfos = mModel->getLodInfoVector();
        const std::vector<glm::vec3> &vertices = mModel->getVertexVector();
        const std::vector<odDb::Model::Polygon> &polygons = mModel->getPolygonVector();

        for(auto it = lodMeshInfos.begin(); it != lodMeshInfos.end(); ++it)
        {
            auto geometry = od::make_refd<Geometry>();

            GeometryBuilder gb(mRenderer, *geometry, mModel->getName() + " (LOD '" + it->lodName + "')", mModel->getAssetProvider());
            gb.setBuildSmoothNormals(mModel->getShadingType() != odDb::Model::ShadingType::Flat);

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

            float minDistance = it->distanceThreshold*OD_WORLD_SCALE;
            float maxDistance = ((it+1) == lodMeshInfos.end()) ? std::numeric_limits<float>::max() : (it+1)->distanceThreshold*OD_WORLD_SCALE;
            size_t lodIndex = addLod(minDistance*0, maxDistance*10000);

            addGeometry(geometry, lodIndex);

            break;
        }
    }

}

