/*
 * Object.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef OBJECT_H_
#define OBJECT_H_

#include <ObjectTemplate.h>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>


namespace od
{

    class Level;

    typedef uint32_t ObjectId;

    class Object : public osg::Group
    {
    public:

        Object(Level &level);

        inline ObjectId getObjectId() const { return mId; }
        inline ObjectTemplatePtr getObjectTemplate() { return mObjTemplate; }

        void loadFromRecord(DataReader dr);


    private:

        Level &mLevel;
        ObjectId mId;
        ObjectTemplatePtr mObjTemplate;
        uint32_t mFlags;
        uint16_t mInitialEventCount;
    };

    typedef osg::ref_ptr<od::Object> ObjectPtr;
}



#endif /* OBJECT_H_ */
