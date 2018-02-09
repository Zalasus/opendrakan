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
#include "Model.h"

namespace od
{

    class ObjectTemplateFactory;

    /**
     * AKA "Class"
     */
    class ObjectTemplate : public Asset, public osg::Referenced
    {
    public:

        ObjectTemplate(Database &db, RecordId classId);

        inline bool hasModel() const { return mModelRef.assetId != 0; }
        inline AssetRef getModelRef() const { return mModelRef; }
        inline ModelPtr getModel() { return mModel; }
        inline std::string getName() const { return mTemplateName; }

        void loadFromRecord(ObjectTemplateFactory &factory, DataReader dr);

        // implement Asset
        virtual const char *getAssetTypeName() const { return "object template"; }


    private:

        std::string mTemplateName;
        AssetRef mModelRef;
        ModelPtr mModel;
        //RflClass mRflClass;
        uint16_t mIconNumber;
    };

    typedef osg::ref_ptr<ObjectTemplate> ObjectTemplatePtr;

}

#endif /* OBJECTTEMPLATE_H_ */
