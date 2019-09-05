/*
 * @file SelfDestruct.h
 * @date 4 Sep 2019
 * @author zal
 */

#ifndef INCLUDE_DRAGONRFL_CLASSES_SPECIAL_SELFDESTRUCT_H_
#define INCLUDE_DRAGONRFL_CLASSES_SPECIAL_SELFDESTRUCT_H_

#include <odCore/rfl/Class.h>

namespace dragonRfl
{

    /**
     * @brief A dummy class implementation that destroys the object as soon as it is loaded without any further logic.
     *
     * This can be used as an implementation for client-only or server-only class types for the part that is
     * not needed (for a client-only class, make this the server implementation and vice versa).
     */
    class SelfDestruct : public odRfl::ClassBase
    {
    public:

        virtual void probeFields(odRfl::FieldProbe &probe) override;

        virtual void onLoaded(od::LevelObject &obj) override;

    };

}

#endif /* INCLUDE_DRAGONRFL_CLASSES_SPECIAL_SELFDESTRUCT_H_ */
