
#ifndef INCLUDE_ODCORE_ANIM_ANIMMODES_H_
#define INCLUDE_ODCORE_ANIM_ANIMMODES_H_

#include <array>

namespace odAnim
{

    enum class PlaybackType
    {
        NORMAL,
        LOOPING,
        PINGPONG
    };

    enum class BoneMode
    {
        NORMAL, ///< The transform is applied to the bone, only deforming the mesh
        ACCUMULATE, ///< The transform is not applied to the bone, but reported to the accumulator
        IGNORE ///< The transform is neither applied to the bone nor the accumulator
    };

    using AxesBoneModes = std::array<BoneMode, 3>;


    struct AnimModes
    {
        inline static constexpr int32_t CHANNEL_WHOLE_SKELETON = -1;

        AnimModes()
        : playbackType(PlaybackType::NORMAL)
        , boneModes({BoneMode::NORMAL, BoneMode::NORMAL, BoneMode::NORMAL})
        , channel(CHANNEL_WHOLE_SKELETON)
        , speed(1.0f)
        , startTime(0.0f)
        , transitionTime(0.0f)
        {
        }

        PlaybackType playbackType;
        AxesBoneModes boneModes;
        int32_t channel;
        float speed;
        float startTime;
        float transitionTime;
    };

}

#endif
