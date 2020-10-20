
#include <odCore/net/QueuedDownlinkConnector.h>

namespace odNet
{

    using lock_guard = std::lock_guard<std::mutex>;

    void QueuedDownlinkConnector::flushQueue(DownlinkConnector &connector)
    {
        lock_guard lock(mMutex);

        struct FlushVisitor
        {
            DownlinkConnector &connector;

            FlushVisitor(DownlinkConnector &c)
            : connector(c)
            {
            }

            void operator()(const LoadLevel &l)
            {
                connector.loadLevel(l.path);
            }

            void operator()(const ObjectStatesChanged &o)
            {
                connector.objectStatesChanged(o.tick, o.id, o.states);
            }

            void operator()(const ConfirmSnapshot &c)
            {
                connector.confirmSnapshot(c.tick, c.realtime, c.discreteChangeCount, c.referenceTick);
            }

            void operator()(const GlobalMessage &m)
            {
                const char *data = &mMessages[m.dataOffset];
                connector.globalMessage(m.channelCode, data, m.size);
            }

        };

        FlushVisitor visitor(connector);

        for(auto &call : mCalls)
        {
            call.visit(visitor);
        }

        mCalls.clear();
        mMessages.clear();
    }

    void QueuedDownlinkConnector::loadLevel(const std::string &path)
    {
        lock_guard lock(mMutex);
        LoadLevel c{path};
        mCalls.emplace_back(c);
    }

    void QueuedDownlinkConnector::objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states)
    {
        lock_guard lock(mMutex);
        ObjectStatesChanged c{tick, id, states};
        mCalls.emplace_back(c);
    }

    void QueuedDownlinkConnector::confirmSnapshot(odState::TickNumber tick, double realtime, size_t discreteChangeCount, odState::TickNumber referenceTick)
    {
        lock_guard lock(mMutex);
        ConfirmSnapshot c{tick, realtime, discreteChangeCount, referenceTick};
        mCalls.emplace_back(c);
    }

    void QueuedDownlinkConnector::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        lock_guard lock(mMutex);

        GlobalMessage c{code, mMessages.size(), size};
        mCalls.emplace_back(c);

        mMessages.insert(mMessages.end(), data, data+size);
    }

}
