
#include <odCore/net/QueuedUplinkConnector.h>

namespace odNet
{

    using lock_guard = std::lock_guard<std::mutex>;

    void QueuedUplinkConnector::flushQueue(UplinkConnector &connector)
    {
        lock_guard lock(mMutex);

        struct FlushVisitor
        {
            UplinkConnector &connector;

            FlushVisitor(UplinkConnector &c)
            : connector(c)
            {
            }

            void operator()(const ActionTriggered &c)
            {
                connector.actionTriggered(c.code, c.state);
            }

            void operator()(const AnalogActionTriggered &c)
            {
                connector.analogActionTriggered(c.code, c.axes);
            }

            void operator()(const AcknowledgeSnapshot &c)
            {
                connector.acknowledgeSnapshot(c.tick);
            }
        };

        FlushVisitor visitor(connector);

        for(auto &call : mCalls)
        {
            std::visit(visitor, call);
        }

        mCalls.clear();
    }

    void QueuedUplinkConnector::actionTriggered(odInput::ActionCode code, odInput::ActionState state)
    {
        lock_guard lock(mMutex);
        mCalls.emplace_back(ActionTriggered{code, state});
    }

    void QueuedUplinkConnector::analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes)
    {
        lock_guard lock(mMutex);
        mCalls.emplace_back(AnalogActionTriggered{code, axes});
    }

    void QueuedUplinkConnector::acknowledgeSnapshot(odState::TickNumber tick)
    {
        lock_guard lock(mMutex);
        mCalls.emplace_back(AcknowledgeSnapshot{tick});
    }

}
