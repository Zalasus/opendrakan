/*
 * Class.h
 *
 *  Created on: 11 Feb 2018
 *      Author: zal
 */

#ifndef INCLUDE_CLASS_H_
#define INCLUDE_CLASS_H_

#include <memory>

#include <odCore/db/Asset.h>
#include <odCore/rfl/Class.h>
#include <odCore/rfl/FieldLoaderProbe.h>

namespace od
{
    class Engine;
    class LevelObject;
}

namespace odRfl
{
    class ClassFactory;
	class ClassBase;
	class RflManager;
}

namespace odDb
{
    class ClassFactory;
    class Model;

	class Class : public Asset
	{
	public:

		Class(ClassFactory &classFactory);

		inline bool hasModel() const { return mModelRef != AssetRef::NULL_REF; }
        inline AssetRef getModelRef() const { return mModelRef; }
        inline const std::string &getName() const { return mClassName; }
        inline odRfl::ClassId getRflClassId() const { return mRflClassId; }
        inline const odRfl::FieldLoaderProbe &getFieldLoader() const { return mFieldLoader; }

        virtual void load(od::SrscFile::RecordInputCursor cursor) override;

        /**
         * @brief Fills a provided field bundle with the values defined by this class.
         */
        void fillFields(odRfl::FieldBundle &fields);

        /**
         * @brief Queries a factory from the RFL that can be used to instantiate the class.
         *
         * If no RFL provides an implementation of the referenced class ID, this will return nullptr;
         *
         * The factory is cached internally. Once a valid factory has been obtained, this method
         * will not query the RFL manager anymore and instead return the cached entry.
         */
        odRfl::ClassFactory *getRflClassFactory(odRfl::RflManager &rflManager);

        /**
         * @brief Obtains a factory, instanciates the class and fills it's fields.
         *
         * This is a convenience method.
         *
         * For now, this will return nullptr for an unimplemented class. Later, this
         * might become an exceptin.
         */
        std::unique_ptr<odRfl::ClassBase> makeInstance(od::Engine &engine);

        /**
         * @brief Returns the cached model for this class or attempts to load it if it has not yet been cached.
         */
        std::shared_ptr<odDb::Model> getOrLoadModel();


	private:

        ClassFactory &mClassFactory; // TODO: don't store references?

        std::string mClassName;
        AssetRef mModelRef;
        uint16_t mRflClassId;
        odRfl::FieldLoaderProbe mFieldLoader;
        uint16_t mIconNumber;

        odRfl::ClassFactory *mCachedRflClassFactory;
        std::shared_ptr<odDb::Model> mCachedModel;
	};


	template <>
    struct AssetTraits<Class>
    {
        static const char *name()
        {
            return "Class";
        }

        static constexpr od::RecordType baseType()
        {
            return static_cast<od::RecordType>(od::SrscRecordType::CLASS);
        }
    };

}

#endif /* INCLUDE_CLASS_H_ */
