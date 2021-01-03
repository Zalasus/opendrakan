
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
            std::vector<char> &messages;

            FlushVisitor(DownlinkConnector &c, std::vector<char> &m)
            : connector(c)
            , messages(m)
            {
            }

            void operator()(const GlobalDatabaseTableEntry &l)
            {
                connector.globalDatabaseTableEntry(l.dbIndex, l.path);
            }

            void operator()(const LoadLevel &l)
            {
                connector.loadLevel(l.path, l.loadedDatabaseCount);
            }

            void operator()(const ObjectStatesChanged &o)
            {
                connector.objectStatesChanged(o.tick, o.id, o.states);
            }

            void operator()(const ObjectExtraStatesChanged &o)
            {
                const char *data = &messages[o.dataOffset];
                connector.objectExtraStatesChanged(o.tick, o.id, data, o.size);
            }

            void operator()(const ConfirmSnapshot &c)
            {
                connector.confirmSnapshot(c.tick, c.realtime, c.discreteChangeCount, c.referenceTick);
            }

            void operator()(const GlobalMessage &m)
            {
                const char *data = &messages[m.dataOffset];
                connector.globalMessage(m.code, data, m.size);
            }

            void operator()(const ObjectAnimation &a)
            {
                connector.objectAnimation(a.id, a.animRef, a.modes, a.realtime);
            }

        };

        FlushVisitor visitor(connector, mMessages);

        for(auto &call : mCalls)
        {
            std::visit(visitor, call);
        }

        mCalls.clear();
        mMessages.clear();
    }

    void QueuedDownlinkConnector::globalDatabaseTableEntry(odDb::GlobalDatabaseIndex dbIndex, const std::string &path)
    {
        lock_guard lock(mMutex);
        mCalls.emplace_back(GlobalDatabaseTableEntry{dbIndex, path});
    }

    void QueuedDownlinkConnector::loadLevel(const std::string &path, size_t loadedDatabaseCount)
    {
        lock_guard lock(mMutex);
        mCalls.emplace_back(LoadLevel{loadedDatabaseCount, path});
    }

    void QueuedDownlinkConnector::objectStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const od::ObjectStates &states)
    {
        lock_guard lock(mMutex);
        ObjectStatesChanged c{tick, id, states};
        mCalls.emplace_back(c);
    }

    void QueuedDownlinkConnector::objectExtraStatesChanged(odState::TickNumber tick, od::LevelObjectId id, const char *data, size_t size)
    {
        lock_guard lock(mMutex);

        mCalls.emplace_back(ObjectExtraStatesChanged{ tick, id, mMessages.size(), size });
        mMessages.insert(mMessages.end(), data, data+size);
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

    void QueuedDownlinkConnector::objectAnimation(od::LevelObjectId id, odDb::GlobalAssetRef animRef, const odAnim::AnimModes &modes, double realtime)
    {
        lock_guard lock(mMutex);
        mCalls.emplace_back(ObjectAnimation{id, animRef, modes, realtime});
    }

}
