
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


    UplinkMessageDispatcher::UplinkMessageDispatcher(std::shared_ptr<UplinkConnector> connector)
    : mConnector(connector)
    {
    }

    void UplinkMessageDispatcher::globalMessage(MessageChannelCode code, const char *data, size_t size)
    {
        // TODO: implement
    }

    void UplinkMessageDispatcher::objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size)
    {
        throw od::UnsupportedException("Can't send object messages in the uplink!");
    }

}
