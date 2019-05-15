/*
 * Model.cpp
 *
 *  Created on: May 15, 2019
 *      Author: zal
 */

#include <odOsg/render/Model.h>

#include <odCore/Exception.h>

namespace odOsg
{

    Model::Model()
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

    odRender::Geometry *Model::createNewGeometry()
    {
        return nullptr;
    }

}

