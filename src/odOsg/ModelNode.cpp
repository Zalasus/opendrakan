/*
 * ModelNode.cpp
 *
 *  Created on: Nov 22, 2018
 *      Author: zal
 */

#include <odOsg/ModelNode.h>

#include <odCore/Exception.h>

#include <odOsg/Renderer.h>

namespace odOsg
{

    ModelNode::ModelNode(Renderer *renderer)
    : mRenderer(renderer)
    , mGeometryGroup(new osg::Group)
    {
        osg::ref_ptr<osg::Program> modelProgram = mRenderer->getShaderFactory().getProgram("model");
        mGeometryGroup->getOrCreateStateSet()->setAttribute(modelProgram);
    }

    size_t ModelNode::addLod(float minDistance, float maxDistance)
    {
        mLods.push_back(std::make_pair(minDistance, maxDistance));

        return mLods.size() - 1;
    }

    void ModelNode::addGeometry(odRender::Geometry *g)
    {

    }

    void ModelNode::addGeometry(odRender::Geometry *g, size_t lodIndex)
    {
        if(lodIndex >= mLods.size())
        {
            throw od::Exception("LOD index out of bounds");
        }
    }

    void ModelNode::addGeometry(odRender::Geometry *g, size_t lodIndex, size_t partIndex)
    {
        if(lodIndex >= mLods.size())
        {
            throw od::Exception("LOD index out of bounds");
        }
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

