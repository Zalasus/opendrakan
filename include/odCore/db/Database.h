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
#include <unordered_map>
#include <memory>
#include <vector>

#include <odCore/FilePath.h>
#include <odCore/SrscFile.h>

#include <odCore/db/Asset.h>
#include <odCore/db/TextureFactory.h>
#include <odCore/db/ModelFactory.h>
#include <odCore/db/ClassFactory.h>
#include <odCore/db/AnimationFactory.h>
#include <odCore/db/SoundFactory.h>
#include <odCore/db/SequenceFactory.h>

namespace odDb
{

	class DbManager;
	class Database;
    class DependencyTable;

	class Database
	{
	public:

		Database(const od::FilePath &dbFilePath, DbManager &dbManager, GlobalDatabaseIndex globalIndex);
		~Database();

		inline od::FilePath getDbFilePath() const { return mDbFilePath; }
		inline std::string getShortName() const { return mDbFilePath.fileStrNoExt(); }
        inline GlobalDatabaseIndex getGlobalIndex() const { return mGlobalIndex; }
        inline DependencyTable &getDependencyTable() const { return *mDependencyTable; }
		inline TextureFactory *getTextureFactory() { return mTextureFactory.get(); }
		inline ClassFactory *getClassFactory() { return mClassFactory.get(); }
		inline ModelFactory *getModelFactory() { return mModelFactory.get(); }
		inline SequenceFactory *getSequenceFactory() { return mSequenceFactory.get(); }
		inline AnimationFactory *getAnimationFactory() { return mAnimFactory.get(); }
		inline SoundFactory *getSoundFactory() { return mSoundFactory.get(); }

		void loadDbFileAndDependencies(size_t dependencyDepth);

        template <typename T>
        std::shared_ptr<T> loadAsset(od::RecordId recordId);

        std::shared_ptr<Texture>   loadTexture(od::RecordId recordId);
        std::shared_ptr<Class>     loadClass(od::RecordId recordId);
        std::shared_ptr<Model>     loadModel(od::RecordId recordId);
        std::shared_ptr<Sequence>  loadSequence(od::RecordId recordId);
        std::shared_ptr<Animation> loadAnimation(od::RecordId recordId);
        std::shared_ptr<Sound>     loadSound(od::RecordId recordId);


	private:

        template <typename T>
        void _tryOpeningAssetContainer(std::unique_ptr<T> &factoryPtr, std::unique_ptr<od::SrscFile> &containerPtr, const char *extension);


		od::FilePath mDbFilePath;
		DbManager &mDbManager;
        GlobalDatabaseIndex mGlobalIndex;

		uint32_t mVersion;
        std::shared_ptr<DependencyTable> mDependencyTable; // TODO: we have to check for dependency loops to prevent leaks

		std::unique_ptr<TextureFactory> mTextureFactory;
		std::unique_ptr<od::SrscFile> mTextureContainer;

		std::unique_ptr<ModelFactory> mModelFactory;
		std::unique_ptr<od::SrscFile> mModelContainer;

		std::unique_ptr<ClassFactory> mClassFactory;
		std::unique_ptr<od::SrscFile> mClassContainer;

		std::unique_ptr<AnimationFactory> mAnimFactory;
		std::unique_ptr<od::SrscFile> mAnimContainer;

		std::unique_ptr<SoundFactory> mSoundFactory;
		std::unique_ptr<od::SrscFile> mSoundContainer;

		std::unique_ptr<SequenceFactory> mSequenceFactory;
        std::unique_ptr<od::SrscFile> mSequenceContainer;
	};

}

#endif /* INCLUDE_DATABASE_H_ */
