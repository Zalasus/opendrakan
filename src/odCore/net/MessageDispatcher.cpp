
#include <odCore/net/MessageDispatcher.h>

#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/UplinkConnector.h>

namespace odNet
{

    GlobalMessageBuilder::GlobalMessageBuilder(MessageDispatcher &dispatcher, MessageChannelCode code, std::vector<char> &dataBuffer)
    : mDispatcher(dispatcher)
    , mCode(code)
    , mData(dataBuffer)
    , mStreamBuffer(dataBuffer)
    , mStream(&mStreamBuffer)
    , mDataWriter(mStream)
    {
    }

    GlobalMessageBuilder::~GlobalMessageBuilder()
    {
        mStreamBuffer.sync();
        mDispatcher.globalMessage(mCode, mData.data(), mData.size());
    }


    void MessageDispatcher::receiveGlobalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        for(auto weakListener : mChannelMeta[code].globalListeners)
        {
            auto listener = weakListener.lock();
            if(listener != nullptr)
            {
                od::MemoryInputBuffer streamBuffer(data, size);
                std::istream stream(&streamBuffer);
                od::DataReader dr(stream);

                listener->triggerCallback(dr);
            }
        }
    }


    DownlinkMessageDispatcher::DownlinkMessageDispatcher(std::shared_ptr<DownlinkConnector> connector)
    : mConnector(connector)
    {
    }

    void DownlinkMessageDispatcher::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        mConnector->globalMessage(code, data, size);
    }

    void DownlinkMessageDispatcher::objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size)
    {
    }


    void UplinkMessageDispatcher::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        if(mConnector != nullptr)
        {
            // TODO: implement
        }
    }

    void UplinkMessageDispatcher::objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size)
    {
        throw od::UnsupportedException("Can't send object messages in the uplink!");
    }

}
