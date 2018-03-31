/*
 * RiotDb.h
 *
 *  Created on: 6 Jan 2018
 *      Author: zal
 */

#ifndef INCLUDE_DATABASE_H_
#define INCLUDE_DATABASE_H_

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <vector>
#include <functional>

#include "FilePath.h"
#include "SrscFile.h"
#include "Asset.h"
#include "AssetProvider.h"
#include "TextureFactory.h"
#include "ModelFactory.h"
#include "ClassFactory.h"
#include "AnimationFactory.h"

namespace od
{

	class DbManager;
	class Database;


    typedef std::reference_wrapper<Database> DbRefWrapper;

	class Database : public AssetProvider
	{
	public:

		Database(FilePath dbFilePath, DbManager &dbManager);
		~Database();

		inline FilePath getDbFilePath() const { return mDbFilePath; }
		inline std::string getShortName() const { return mDbFilePath.fileStrNoExt(); }

		void loadDbFileAndDependencies(size_t dependencyDepth);


		// implement AssetProvider
		virtual TexturePtr   getTextureByRef(const AssetRef &ref) override;
	    virtual ClassPtr     getClassByRef(const AssetRef &ref) override;
	    virtual ModelPtr     getModelByRef(const AssetRef &ref) override;
	    virtual SequencePtr  getSequenceByRef(const AssetRef &ref) override;
	    virtual osg::ref_ptr<Animation> getAnimationByRef(const AssetRef &ref) override;

	    // override AssetProvider
	    virtual TexturePtr   getTexture(RecordId recordId) override;
	    virtual ClassPtr     getClass(RecordId recordId) override;
	    virtual ModelPtr     getModel(RecordId recordId) override;
	    virtual SequencePtr  getSequence(RecordId recordId) override;
	    virtual osg::ref_ptr<Animation> getAnimation(RecordId recordId) override;


	private:


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		std::map<uint16_t, DbRefWrapper> mDependencyMap;

		std::unique_ptr<TextureFactory> mTextureFactory;
		std::unique_ptr<ModelFactory> mModelFactory;
		std::unique_ptr<ClassFactory> mClassFactory;
		std::unique_ptr<AnimationFactory> mAnimFactory;
	};
}

#endif /* INCLUDE_DATABASE_H_ */
