
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

}

#endif
