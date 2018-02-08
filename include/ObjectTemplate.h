/*
 * ObjectTemplate.h
 *
 *  Created on: 8 Feb 2018
 *      Author: zal
 */

#ifndef OBJECTTEMPLATE_H_
#define OBJECTTEMPLATE_H_

#include <osg/Referenced>
#include <osg/ref_ptr>

#include "Asset.h"

namespace od
{

    class ObjectTemplateFactory;

    /**
     * AKA "Class"
     */
    class ObjectTemplate : public Asset, public osg::Referenced
    {
    public:

        ObjectTemplate(RecordId classId);

        inline AssetRef getModelRef() const { return mModelRef; }

        void loadFromRecord(ObjectTemplateFactory &factory, DataReader dr);

        // implement Asset
        virtual const char *getAssetTypeName() const { return "object template"; }


    private:

        std::string mTemplateName;
        AssetRef mModelRef;
        //RflClass mRflClass;
        uint16_t mIconNumber;
    };

    typedef osg::ref_ptr<ObjectTemplate> ObjectTemplatePtr;

}

#endif /* OBJECTTEMPLATE_H_ */
