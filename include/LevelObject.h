/*
 * Object.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <memory>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include "db/Class.h"

namespace od
{

    class Level;

    typedef uint32_t LevelObjectId;

    class LevelObject : public osg::Group
    {
    public:

        LevelObject(Level &level);

        inline LevelObjectId getObjectId() const { return mId; }
        inline ClassPtr getClass() { return mClass; }
        inline Level &getLevel() { return mLevel; }
        inline osg::Vec3f getPosition() const { return mPosition; }

        void loadFromRecord(DataReader dr);

        // override osg::Group
		virtual const char *libraryName() const override { return "od";    }
        virtual const char *className()   const override { return "LevelObject"; }

    private:

        Level &mLevel;
        LevelObjectId mId;
        ClassPtr mClass;
        std::unique_ptr<RflClass> mRflClassInstance;
        osg::Vec3f mPosition;
        uint32_t mFlags;
        uint16_t mInitialEventCount;
    };

    typedef osg::ref_ptr<od::LevelObject> LevelObjectPtr;
}



#endif /* OBJECT_H_ */
