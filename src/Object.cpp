/*
 * Object.cpp
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#include "Object.h"

#include "Level.h"
#include "Exception.h"
#include "OdDefines.h"

#define OD_OBJECT_FLAG_VISIBLE 0x001
#define OD_OBJECT_FLAG_SCALED  0x100

namespace od
{

    Object::Object(Level &level)
    : mLevel(level)
    , mId(0)
    , mFlags(0)
    , mInitialEventCount(0)
    {
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

        uint16_t dummyLength;

        dr >> mId
           >> templateRef
           >> DataReader::Ignore(4)
           >> xPos
           >> yPos
           >> zPos
           >> mFlags
           >> mInitialEventCount
		   >> dummyLength;

        dr.ignore(2*dummyLength);

        dr >> xRot
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

        mObjTemplate = mLevel.getAssetAsObjectTemplate(templateRef);

        if(mObjTemplate->hasModel() && (mFlags & OD_OBJECT_FLAG_VISIBLE))
        {
        	try
        	{
        		osg::ref_ptr<osg::PositionAttitudeTransform> transform(new osg::PositionAttitudeTransform);
        		transform->setAttitude(osg::Quat(
        			osg::DegreesToRadians((float)xRot), osg::Vec3(1,0,0),
        			osg::DegreesToRadians((float)yRot-90), osg::Vec3(0,1,0),
					osg::DegreesToRadians((float)zRot), osg::Vec3(0,0,1)));
        		transform->setPosition(osg::Vec3(xPos, yPos, zPos) * OD_WORLD_SCALE);
        		transform->setScale(osg::Vec3(xScale, yScale, zScale));

				transform->addChild(mObjTemplate->getModel());
				this->addChild(transform);

        	}catch(NotFoundException &e)
        	{
        		Logger::error() << "Object " << std::hex << mId << std::dec
        				        << " of class " << mObjTemplate->getName()
								<< " from DB " << mObjTemplate->getDatabase().getDbFilePath().fileStr()
								<< ": Model " << mObjTemplate->getModelRef() << " not found. Leaving invisible";
        	}
        }
    }

}




