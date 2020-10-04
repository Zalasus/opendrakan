
#ifndef INCLUDE_ODCORE_NET_MESSAGEDISPATCHER_H_
#define INCLUDE_ODCORE_NET_MESSAGEDISPATCHER_H_

#include <memory>
#include <vector>
#include <unordered_map>

#include <odCore/DataStream.h>
#include <odCore/IdTypes.h>

#include <odCore/net/IdTypes.h>

namespace odNet
{
    class DownlinkConnector;
    class UplinkConnector;
    class MessageDispatcher;


    class GlobalMessageListenerBase
    {

    };


    template <typename _ChannelEnum>
    class GlobalMessageListener : public GlobalMessageListenerBase
    {
    public:

    };


    class GlobalMessageBuilder
    {
    public:

        GlobalMessageBuilder(MessageDispatcher &dispatcher, MessageChannelCode code, std::vector<char> &dataBuffer);
        GlobalMessageBuilder(GlobalMessageBuilder &c) = delete;
        ~GlobalMessageBuilder();

        template <typename T>
        GlobalMessageBuilder &operator<<(const T &v)
        {
            mDataWriter << v;
            return *this;
        }

        // void send() commented. do not encourage storing these objects! they mus die to send the message

    private:

        MessageDispatcher &mDispatcher;
        MessageChannelCode mCode;
        std::vector<char> &mData;

        od::VectorOutputBuffer mStreamBuffer;
        std::ostream mStream;
        od::DataWriter mDataWriter;

    };


    class MessageDispatcher
    {
    public:

        MessageDispatcher() = default;
        MessageDispatcher(MessageDispatcher &d) = delete;

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
        GlobalMessageBuilder sendGlobalMessage(_ChannelEnum channel)
        {
            auto channelCode = static_cast<MessageChannelCode>(channel);
            return { *this, channelCode, mMessageBufferPool[channelCode] };
        }


        //template <typename _ChannelEnum>
        //ObjectMessageBuilder sendObjectMessage(_ChannelEnum channel, od::LevelObjectId sender, od::LevelObjectId receiver)
        //{
        //}


    protected:

        friend class GlobalMessageBuilder;

        virtual void globalMessage(MessageChannelCode code, const char *data, size_t size) = 0;
        virtual void objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size) = 0;


    private:

        std::unordered_map<MessageChannelCode, std::vector<char>> mMessageBufferPool;

    };


    class DownlinkMessageDispatcher final : public MessageDispatcher
    {
    public:

        DownlinkMessageDispatcher(std::shared_ptr<DownlinkConnector> connector);


    protected:

        virtual void globalMessage(MessageChannelCode code, const char *data, size_t size) override;
        virtual void objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size) override;


    private:

        std::shared_ptr<DownlinkConnector> mConnector;

    };


    class UplinkMessageDispatcher final : public MessageDispatcher
    {
    public:

        UplinkMessageDispatcher(std::shared_ptr<UplinkConnector> connector);


    protected:

        virtual void globalMessage(MessageChannelCode code, const char *data, size_t size) override;
        virtual void objectMessage(MessageChannelCode code, od::LevelObjectId sender, od::LevelObjectId receiver, const char *data, size_t size) override;


    private:

        std::shared_ptr<UplinkConnector> mConnector;

    };

}

#endif
