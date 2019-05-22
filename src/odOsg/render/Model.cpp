/*
 * Model.cpp
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#include <odOsg/render/Model.h>

#include <algorithm>

#include <odCore/Exception.h>
#include <odCore/Downcast.h>

namespace odOsg
{

    Model::Model()
    : mGeode(new osg::Geode)
    {
    }

    size_t Model::getGeometryCount()
    {
        return mGeometries.size();
    }

    odRender::Geometry *Model::getGeometry(size_t index)
    {
        if(index >= mGeometries.size())
        {
            throw od::Exception("Geometry index out of bounds");
        }

        return mGeometries[index];
    }

    void Model::addGeometry(odRender::Geometry *g)
    {
        if(g == nullptr)
        {
            return;
        }

        auto geo = od::confident_downcast<Geometry>(g);

        mGeometries.emplace_back(g);

        mGeode->addDrawable(geo->getOsgGeometry());
    }

    void Model::removeGeometry(odRender::Geometry *g)
    {
        auto it = std::find(mGeometries.begin(), mGeometries.end(), g);
        if(it != mGeometries.end())
        {
            mGeode->removeDrawable((*it)->getOsgGeometry());

            mGeometries.erase(it);
        }
    }

    odRender::Geometry *Model::createNewGeometry()
    {
        return nullptr;
    }

}

