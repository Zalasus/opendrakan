/*
 * ModelNode.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#include <odOsg/ModelNode.h>

#include <odCore/Exception.h>

#include <odOsg/Renderer.h>
#include <odOsg/Geometry.h>

namespace odOsg
{

    ModelNode::ModelNode(Renderer *renderer)
    : mRenderer(renderer)
    , mGeometryGroup(new osg::Group)
    {
        osg::ref_ptr<osg::Program> modelProgram = mRenderer->getShaderFactory().getProgram("model");
        mGeometryGroup->getOrCreateStateSet()->setAttribute(modelProgram);
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
            break;

        case LightingMode::AmbientDiffuse:
            ss->setDefine("LIGHTING");
            ss->removeDefine("SPECULAR");
            break;

        case LightingMode::AmbientDiffuseSpecular:
            ss->setDefine("LIGHTNG");
            ss->setDefine("SPECULAR");
            break;
        }
    }

    osg::Node *ModelNode::getOsgNode()
    {
        return mGeometryGroup;
    }

}

