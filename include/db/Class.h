/*
 * Class.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CLASS_H_
#define INCLUDE_CLASS_H_

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <memory>

#include "Asset.h"
#include "Model.h"
#include "rfl/RflClass.h"
#include "rfl/RflFieldProbe.h"

namespace od
{
	class ClassFactory;

	class Class : public Asset, public osg::Referenced
	{
	public:

		Class(Database &db, RecordId classId);

		inline bool hasModel() const { return mModel != nullptr; }
        inline ModelPtr getModel() { return mModel; }
        inline std::string getName() const { return mClassName; }

        void loadFromRecord(ClassFactory &factory, DataReader dr);
        std::unique_ptr<odRfl::RflClass> makeInstance();

        // implement Asset
        virtual const char *getAssetTypeName() const override { return "class"; }


	private:

        std::string mClassName;
        AssetRef mModelRef;
        ModelPtr mModel;
        uint16_t mRflClassId;
        odRfl::RflClassBuilder mClassBuilder;
        uint16_t mIconNumber;

	};

	typedef osg::ref_ptr<Class> ClassPtr;

}

#endif /* INCLUDE_CLASS_H_ */
