
#include <odCore/state/StateManager.h>

namespace odState
{

    static const TickNumber MAX_BACKLOG = 64;

    class ApplyEventsVisitor
    {
    public:

        void operator()(const Event &event)
        {
            (void)event;
            // catch-all, doesn't do anything
        }

        void operator()(const ObjectTransformEvent &event)
        {
        }

    };

    StateManager::StateManager()
    : mEvents(0)
    {
    }

    void StateManager::addActionEvent(odInput::ActionCode actionCode, bool down)
    {
        mEvents.push(ActionEvent(actionCode, down));
    }

    void StateManager::addObjectTransform(od::LevelObject &object, const ObjectTransform &tf)
    {
        mEvents.push(ObjectTransformEvent(object, tf));
    }

    void StateManager::apply(TickNumber tick)
    {
        auto begin = mEvents.getTickframeBegin(tick);
        auto end = mEvents.getTickframeEnd(tick);
        if(begin == end)
        {
            throw od::Exception("Tick is out of range/not stored");
        }

        ApplyEventsVisitor visitor;

        for(auto it = begin; it != end; ++it)
        {
            it->visit(visitor);
        }
    }

}
