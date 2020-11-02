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
#include <odCore/db/AssetProvider.h>
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

	class Database : public AssetProvider
	{
	public:

		Database(const od::FilePath &dbFilePath, DbManager &dbManager, size_t globalIndex);
		~Database();

		inline od::FilePath getDbFilePath() const { return mDbFilePath; }
		inline std::string getShortName() const { return mDbFilePath.fileStrNoExt(); }
        inline size_t getGlobalIndex() const { return mGlobalIndex; }
		inline TextureFactory *getTextureFactory() { return mTextureFactory.get(); }
		inline ClassFactory *getClassFactory() { return mClassFactory.get(); }
		inline ModelFactory *getModelFactory() { return mModelFactory.get(); }
		inline SequenceFactory *getSequenceFactory() { return mSequenceFactory.get(); }
		inline AnimationFactory *getAnimationFactory() { return mAnimFactory.get(); }
		inline SoundFactory *getSoundFactory() { return mSoundFactory.get(); }

		void loadDbFileAndDependencies(size_t dependencyDepth);

        /**
         * @brief Does the same thing as getDependency(), but returns an owned pointer to Database.
         *
         * Since the database module is up for an overhaul as well, we might make this the official method
         * and remove the non-owned AssetProvider version.
         */
        std::shared_ptr<Database> getDependencyDatabase(uint16_t index);

	    // override AssetProvider
		virtual AssetProvider &getDependency(uint16_t index) override;
        virtual std::shared_ptr<Texture>   getTexture(od::RecordId recordId) override;
        virtual std::shared_ptr<Class>     getClass(od::RecordId recordId) override;
        virtual std::shared_ptr<Model>     getModel(od::RecordId recordId) override;
        virtual std::shared_ptr<Sequence>  getSequence(od::RecordId recordId) override;
        virtual std::shared_ptr<Animation> getAnimation(od::RecordId recordId) override;
        virtual std::shared_ptr<Sound>     getSound(od::RecordId recordId) override;


	private:

        template <typename T>
        void _tryOpeningAssetContainer(std::unique_ptr<T> &factoryPtr, std::unique_ptr<od::SrscFile> &containerPtr, const char *extension);


		od::FilePath mDbFilePath;
		DbManager &mDbManager;
        size_t mGlobalIndex;

		uint32_t mVersion;
		std::unordered_map<uint16_t, std::shared_ptr<Database>> mDependencyMap; // TODO: we have to check for dependency loops to prevent leaks

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
