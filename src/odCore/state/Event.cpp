
#include <odCore/state/Event.h>

namespace odState
{

    ActionEvent::ActionEvent(odInput::ActionCode code, bool down)
    : actionCode(code)
    , keyDown(down)
    {
    }


    ObjectAnimEvent::ObjectAnimEvent(od::LevelObjectId objId, const odDb::GlobalAssetRef &anim, int32_t channel, float speed, const glm::bvec3 &irnt)
    : objectId(objId)
    , animRef(anim)
    , channelIndex(channel)
    , speedModifier(speed)
    , ignoreRootNodeTranslation(irnt)
    {
    }


    ObjectMessageEvent::ObjectMessageEvent(od::LevelObjectId sender, od::LevelObjectId receiver, const od::Message &msg)
    : senderObjectId(sender)
    , receiverObjectId(receiver)
    , message(msg)
    {
    }

}
