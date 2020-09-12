
#include <odCore/ObjectRecord.h>

#include <odCore/Units.h>

namespace od
{

    ObjectRecordData::ObjectRecordData(DataReader &dr)
    {
        uint16_t xRot;
        uint16_t yRot;
        uint16_t zRot;
        uint16_t linkCount;

        dr >> mId
           >> mClassRef
           >> mLightingLayerId
           >> mPosition
           >> mFlags
           >> mInitialEventCount
           >> linkCount;

        mLinkedObjectIndices.resize(linkCount);
        for(size_t i = 0; i < linkCount; ++i)
        {
            uint16_t linkedIndex = 0;
            dr >> linkedIndex;
            mLinkedObjectIndices[i] = linkedIndex;
        }

        dr >> xRot
           >> yRot
           >> zRot;

        if(mFlags & FLAG_OBJECT_FLAG_SCALED)
        {
            dr >> mScale;

        }else
        {
            mScale = glm::vec3(1, 1, 1);
        }

        mFieldLoader = odRfl::FieldLoaderProbe(dr, odRfl::FieldLoaderProbe::RecordFormat::OBJECT);

        mPosition = Units::worldUnitsToLengthUnits(mPosition);

        glm::vec3 eulerAngles(glm::radians((float)xRot), glm::radians((float)yRot-90), glm::radians((float)zRot)); // -90 deg. determined to be correct through experiment
        mRotation = glm::quat(eulerAngles);
    }

}
