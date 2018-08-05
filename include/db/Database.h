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
#include "SoundFactory.h"
#include "SequenceFactory.h"

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
		inline DbManager &getDbManager() { return mDbManager; }
		inline TextureFactory *getTextureFactory() { return mTextureFactory.get(); }
		inline ClassFactory *getClassFactory() { return mClassFactory.get(); }
		inline ModelFactory *getModelFactory() { return mModelFactory.get(); }
		inline SequenceFactory *getSequenceFactory() { return mSequenceFactory.get(); }
		inline AnimationFactory *getAnimationFactory() { return mAnimFactory.get(); }
		inline SoundFactory *getSoundFactory() { return mSoundFactory.get(); }

		void loadDbFileAndDependencies(size_t dependencyDepth);

		// implement AssetProvider
		virtual Texture   *getTextureByRef(const AssetRef &ref) override;
        virtual Class     *getClassByRef(const AssetRef &ref) override;
        virtual Model     *getModelByRef(const AssetRef &ref) override;
        virtual Sequence  *getSequenceByRef(const AssetRef &ref) override;
        virtual Animation *getAnimationByRef(const AssetRef &ref) override;
        virtual Sound     *getSoundByRef(const AssetRef &ref) override;

	    // override AssetProvider
        virtual Texture   *getTexture(RecordId recordId) override;
        virtual Class     *getClass(RecordId recordId) override;
        virtual Model     *getModel(RecordId recordId) override;
        virtual Sequence  *getSequence(RecordId recordId) override;
        virtual Animation *getAnimation(RecordId recordId) override;
        virtual Sound     *getSound(RecordId recordId) override;


	private:

        template <typename T>
        void _tryOpeningAssetContainer(std::unique_ptr<T> &factoryPtr, std::unique_ptr<SrscFile> &containerPtr, const char *extension);


		FilePath mDbFilePath;
		DbManager &mDbManager;

		uint32_t mVersion;
		std::map<uint16_t, DbRefWrapper> mDependencyMap;

		std::unique_ptr<TextureFactory> mTextureFactory;
		std::unique_ptr<SrscFile> mTextureContainer;

		std::unique_ptr<ModelFactory> mModelFactory;
		std::unique_ptr<SrscFile> mModelContainer;

		std::unique_ptr<ClassFactory> mClassFactory;
		std::unique_ptr<SrscFile> mClassContainer;

		std::unique_ptr<AnimationFactory> mAnimFactory;
		std::unique_ptr<SrscFile> mAnimContainer;

		std::unique_ptr<SoundFactory> mSoundFactory;
		std::unique_ptr<SrscFile> mSoundContainer;

		std::unique_ptr<SequenceFactory> mSequenceFactory;
        std::unique_ptr<SrscFile> mSequenceContainer;
	};

	template <typename T>
    void Database::_tryOpeningAssetContainer(std::unique_ptr<T> &factoryPtr, std::unique_ptr<SrscFile> &containerPtr, const char *extension)
    {
        FilePath path = mDbFilePath.ext(extension);
        if(path.exists())
        {
            containerPtr.reset(new SrscFile(path));
            factoryPtr.reset(new T(*this, *containerPtr));

            Logger::verbose() << AssetTraits<typename T::AssetType>::name() << " container of database opened";

        }else
        {
            Logger::verbose() << "Database has no " << AssetTraits<typename T::AssetType>::name() << " container";
        }
    }
}

#endif /* INCLUDE_DATABASE_H_ */
