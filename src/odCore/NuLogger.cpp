/*
 * NuLogger.cpp
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#include <odCore/NuLogger.h>

#include <ctime>
#include <iostream>
#include <algorithm>

namespace od
{

    static const char *getTimestamp()
    {
        std::time_t currentTime = std::time(0);
        std::tm localTime = *std::localtime(&currentTime);

        static char timeString[20];

        std::strftime(timeString, 20 , "%F %T", &localTime);

        return timeString;
    }


    Logger::StreamProxy::StreamProxy(Logger &logger, LogLevel level)
    : mLogger(logger)
    , mLogLevel(level)
    , mLock(logger.mLogMutex)
    {
        mLogger.mProxyStream.str("");
    }

    Logger::StreamProxy::StreamProxy(StreamProxy &&proxy)
    : mLogger(proxy.mLogger)
    , mLogLevel(proxy.mLogLevel)
    , mLock(std::move(proxy.mLock))
    {
    }

    Logger::StreamProxy::~StreamProxy()
    {
        if(mLock.owns_lock())
        {
            mLogger._logLocked(mLogLevel, mLogger.mProxyStream.str());
        }
    }


    Logger::Logger(LogLevel outputLevel, std::ostream *outputStream)
    : mOutputLevel(outputLevel)
    , mOutputStream(outputStream)
    , mEnableTimestamps(false)
    {
    }

    void Logger::setOutputStream(std::ostream *output)
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        mOutputStream = output;
    }

    void Logger::setEnableTimestamps(bool b)
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        mEnableTimestamps = b;
    }

    void Logger::setOutputLogLevel(LogLevel level)
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        mOutputLevel = level;
    }

    void Logger::increaseOutputLogLevel()
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        switch(mOutputLevel)
        {
        case LogLevel::Error:
            mOutputLevel = LogLevel::Warning;
            break;

        case LogLevel::Warning:
            mOutputLevel = LogLevel::Info;
            break;

        case LogLevel::Info:
            mOutputLevel = LogLevel::Verbose;
            break;

        case LogLevel::Verbose:
            mOutputLevel = LogLevel::Debug;
            break;

        default:
            break;
        }
    }

    void Logger::decreaseOutputLogLevel()
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        switch(mOutputLevel)
        {
        case LogLevel::Warning:
            mOutputLevel = LogLevel::Error;
            break;

        case LogLevel::Info:
            mOutputLevel = LogLevel::Warning;
            break;

        case LogLevel::Verbose:
            mOutputLevel = LogLevel::Info;
            break;

        case LogLevel::Debug:
            mOutputLevel = LogLevel::Verbose;
            break;

        default:
            break;
        }
    }

    void Logger::addListener(LogListener *listener)
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        if(listener == this || listener == nullptr)
        {
            return;
        }

        mLogListeners.push_back(listener);
    }

    void Logger::removeListener(LogListener *listener)
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        auto it = std::find(mLogListeners.begin(), mLogListeners.end(), listener);
        if(it != mLogListeners.end())
        {
            mLogListeners.erase(it);
        }
    }

    void Logger::onLog(LogLevel lvl, const std::string &message)
    {
        std::lock_guard<std::mutex> lock(mLogMutex);

        _logLocked(lvl, message);
    }

    Logger &Logger::getDefaultLogger()
    {
        static Logger defaultLogger(LogLevel::Error, &std::cout);

        return defaultLogger;
    }

    Logger::StreamProxy Logger::_getProxyForLevel(LogLevel level)
    {
        return StreamProxy(*this, level);
    }

    const char *Logger::_getTagForLevel(LogLevel level)
    {
        switch(level)
        {
        case LogLevel::Error:
            return "ERRO";

        case LogLevel::Warning:
            return "WARN";

        case LogLevel::Debug:
            return "DBUG";

        case LogLevel::Info:
            return "INFO";

        case LogLevel::Verbose:
            return "VERB";

        default:
            return "????";
        }
    }

    void Logger::_logLocked(LogLevel level, const std::string &message)
    {
        if((static_cast<uint32_t>(level) <= static_cast<uint32_t>(mOutputLevel)) && mOutputStream != nullptr)
        {
            if(mEnableTimestamps)
            {
                (*mOutputStream) << '[' << getTimestamp() << ']';
            }

            (*mOutputStream) << '[' << _getTagForLevel(level) << ']' << ' ' << message << std::endl;
        }

        for(auto it = mLogListeners.begin(); it != mLogListeners.end(); ++it)
        {
            if(*it == nullptr)
            {
                continue;
            }

            (*it)->onLog(level, message);
        }
    }


}
