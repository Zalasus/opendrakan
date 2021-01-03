
#ifndef INCLUDE_ODCORE_RFL_OBJECTRECORD_H_
#define INCLUDE_ODCORE_RFL_OBJECTRECORD_H_

#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <odCore/DataStream.h>
#include <odCore/IdTypes.h>

#include <odCore/db/Asset.h>

#include <odCore/rfl/FieldLoaderProbe.h>

namespace od
{

    class ObjectRecordData
    {
    public:

        static constexpr uint32_t FLAG_OBJECT_FLAG_VISIBLE = 0x001;
        static constexpr uint32_t FLAG_OBJECT_FLAG_SCALED  = 0x100;

        explicit ObjectRecordData(DataReader &dr);
        ObjectRecordData(ObjectRecordData &&o) = default;

        inline LevelObjectId getObjectId() const { return mId; }
        inline odDb::AssetRef getClassRef() const { return mClassRef; }
        inline LayerId getLightSourceLayerId() const { return mLightingLayerId; }
        inline const std::vector<uint16_t> &getLinkedObjectIndices() const { return mLinkedObjectIndices; }
        inline glm::vec3 getPosition() const { return mPosition; }
        inline glm::quat getRotation() const { return mRotation; }
        inline glm::vec3 getScale() const { return mScale; }
        inline odRfl::FieldLoaderProbe &getFieldLoader() { return mFieldLoader; } // TODO: would be nice if this could also be const
        inline bool isVisible() const { return mFlags & FLAG_OBJECT_FLAG_VISIBLE; }


    private:

        LevelObjectId mId;
        odDb::AssetRef mClassRef;
        LayerId mLightingLayerId;
        uint32_t mFlags;
        uint16_t mInitialEventCount;
        std::vector<uint16_t> mLinkedObjectIndices;
        glm::vec3 mPosition;
        glm::quat mRotation;
        glm::vec3 mScale;
        odRfl::FieldLoaderProbe mFieldLoader;
    };

}

#endif
