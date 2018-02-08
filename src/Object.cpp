/*
 * Object.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "Object.h"

#include "Level.h"

#define OD_OBJECT_FLAG_SCALED 0x100

namespace od
{

    Object::Object(Level &level)
    : mLevel(level)
    , mId(0)
    , mTransform(new osg::PositionAttitudeTransform)
    {
        this->addChild(mTransform);
    }

    void Object::loadFromRecord(DataReader dr)
    {
        AssetRef templateRef;
        float xPos;
        float yPos;
        float zPos;
        uint16_t xRot;
        uint16_t yRot;
        uint16_t zRot;
        float xScale = 1;
        float yScale = 1;
        float zScale = 1;
        uint32_t dataAreaSize;
        uint32_t paramCount;

        dr >> mId
           >> templateRef
           >> DataReader::Ignore(4)
           >> xPos
           >> yPos
           >> zPos
           >> mFlags
           >> mInitialEventCount
           >> xRot
           >> yRot
           >> zRot;

        if(mFlags & OD_OBJECT_FLAG_SCALED)
        {
            dr >> xScale
               >> yScale
               >> zScale;
        }

        dr >> dataAreaSize
           >> paramCount;

        dr.ignore(4*dataAreaSize + 2*paramCount);

        for(size_t i = 0; i < paramCount; ++i)
        {
            uint32_t type;
            std::string name;
            dr >> type
               >> name;
        }

        mTransform->setPosition(osg::Vec3(xPos, yPos, zPos));
        mTransform->setScale(osg::Vec3(xScale, yScale, zScale));

        ObjectTemplatePtr objTemplate = mLevel.getAssetAsObjectTemplate(templateRef);
        ModelPtr model = mLevel.getAssetAsModel(objTemplate->getModelRef());

        mTransform->addChild(model);
    }

}




