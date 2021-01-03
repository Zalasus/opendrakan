
#include <odCore/state/Event.h>

namespace odState
{

    ActionEvent::ActionEvent(odInput::ActionCode code, bool down)
    : actionCode(code)
    , keyDown(down)
    {
    }


    ObjectAnimEvent::ObjectAnimEvent(od::LevelObjectId objId, const odDb::GlobalAssetRef &anim, const odAnim::AnimModes &m)
    : objectId(objId)
    , animRef(anim)
    , modes(m)
    {
    }


    ObjectMessageEvent::ObjectMessageEvent(od::LevelObjectId sender, od::LevelObjectId receiver, const od::Message &msg)
    : senderObjectId(sender)
    , receiverObjectId(receiver)
    , message(msg)
    {
    }

}
