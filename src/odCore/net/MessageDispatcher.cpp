
#include <odCore/net/MessageDispatcher.h>

#include <odCore/net/DownlinkConnector.h>
#include <odCore/net/UplinkConnector.h>

namespace odNet
{

    GlobalDownlinkMessageBuilder::GlobalDownlinkMessageBuilder(std::shared_ptr<DownlinkConnector> connector, MessageChannelCode code, std::vector<uint8_t> &dataBuffer)
    : mConnector(connector)
    , mCode(code)
    , mData(dataBuffer)
    , mStreamBuffer(dataBuffer)
    , mStream(&mStreamBuffer)
    , mDataWriter(mStream)
    {
    }

    GlobalDownlinkMessageBuilder::~GlobalDownlinkMessageBuilder()
    {
        mConnector->globalMessage(mCode, mData.data(), mData.size());
    }



    /*UplinkMessageDispatcher::UplinkMessageDispatcher(std::shared_ptr<UplinkConnector> connector)
    : mConnector(connector)
    {
    }*/


    DownlinkMessageDispatcher::DownlinkMessageDispatcher(std::shared_ptr<DownlinkConnector> connector)
    : mConnector(connector)
    {
    }


}
