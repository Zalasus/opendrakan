
#ifndef INCLUDE_ODCORE_NET_MESSAGEDISPATCHER_H_
#define INCLUDE_ODCORE_NET_MESSAGEDISPATCHER_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include <odCore/DataStream.h>

#include <odCore/net/IdTypes.h>

namespace odNet
{
    class DownlinkConnector;
    class UplinkConnector;


    class GlobalMessageListenerBase
    {

    };

    template <typename _ChannelEnum>
    class GlobalMessageListener : public GlobalMessageListenerBase
    {
    public:

    };


    class GlobalDownlinkMessageBuilder
    {
    public:

        GlobalDownlinkMessageBuilder(std::shared_ptr<DownlinkConnector> connector, MessageChannelCode code, std::vector<char> &dataBuffer);
        GlobalDownlinkMessageBuilder(GlobalDownlinkMessageBuilder &c) = delete;
        ~GlobalDownlinkMessageBuilder();

        template <typename T>
        GlobalDownlinkMessageBuilder &operator<<(const T &v)
        {
            mDataWriter << v;
            return *this;
        }

        // void send() commented. do not encourage storing these objects! they mus die to send the message

    private:

        std::shared_ptr<DownlinkConnector> mConnector;
        MessageChannelCode mCode;
        std::vector<char> &mData;

        od::VectorOutputBuffer mStreamBuffer;
        std::ostream mStream;
        od::DataWriter mDataWriter;

    };


    /*class UplinkMessageDispatcher
    {
    public:

        UplinkMessageDispatcher(std::shared_ptr<UplinkConnector> connector);

        template <typename _ChannelEnum>
        MessageBuilder sendMessage(_ChannelEnum channel)
        {
            auto channelId = static_cast<MessageChannelCode>(channel);
        }


    private:

        std::shared_ptr<UplinkConnector> mConnector;

    };
  */

    class DownlinkMessageDispatcher
    {
    public:

        DownlinkMessageDispatcher(std::shared_ptr<DownlinkConnector> connector);

        template <typename _ChannelEnum>
        std::shared_ptr<GlobalMessageListener<_ChannelEnum>> createGlobalListener(_ChannelEnum channel)
        {
            auto newListener = std::make_shared<GlobalMessageListener<_ChannelEnum>>(channel);
            // TODO: store weak ref so we can signal the listener later!
            return newListener;
        }

        //template <typename _ChannelEnum>
        //std::shared_ptr<ObjectMessageListener<_ChannelEnum>> createObjectListener(_ChannelEnum channel, od::LevelObjectId id)
        //{
        //}

        template <typename _ChannelEnum>
        GlobalDownlinkMessageBuilder sendGlobalMessage(_ChannelEnum channel)
        {
            auto channelCode = static_cast<MessageChannelCode>(channel);
            return { mConnector, channelCode, mMessageBufferPool[channelCode] };
        }


        //template <typename _ChannelEnum>
        //MessageBuilder sendObjectMessage(_ChannelEnum channel, od::LevelObjectId sender, od::LevelObjectId receiver)
        //{
        //}


    private:

        std::shared_ptr<DownlinkConnector> mConnector;

        std::unordered_map<MessageChannelCode, std::vector<char>> mMessageBufferPool;

    };

}

#endif
