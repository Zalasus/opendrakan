
#ifndef INCLUDE_ODCORE_NET_QUEUEDUPLINKCONNECTOR_H_
#define INCLUDE_ODCORE_NET_QUEUEDUPLINKCONNECTOR_H_

#include <mutex>
#include <vector>
#include <variant>

#include <odCore/CTypes.h>

#include <odCore/net/UplinkConnector.h>

namespace odNet
{

    class QueuedUplinkConnector final : public UplinkConnector
    {
    public:

        void flushQueue(UplinkConnector &connector);

        virtual void actionTriggered(odInput::ActionCode code, odInput::ActionState state) override;
        virtual void analogActionTriggered(odInput::ActionCode code, const glm::vec2 &axes) override;
        virtual void acknowledgeSnapshot(odState::TickNumber tick) override;


    private:

        struct ActionTriggered
        {
            odInput::ActionCode code;
            odInput::ActionState state;
        };

        struct AnalogActionTriggered
        {
            odInput::ActionCode code;
            glm::vec2 axes;
        };

        struct AcknowledgeSnapshot
        {
            odState::TickNumber tick;
        };

        using QueuedCall = std::variant<ActionTriggered, AnalogActionTriggered, AcknowledgeSnapshot>;

        std::mutex mMutex;
        std::vector<QueuedCall> mCalls;

    };

}

#endif
