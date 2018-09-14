/*
 * Logger.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: zalasus
 */

#include <odCore/Logger.h>

#include <ctime>
#include <algorithm>
#include <cassert>

#include <odCore/Exception.h>

//namespace od
//{

	static const char *getTimestamp()
	{
		std::time_t currentTime = std::time(0);
		std::tm localTime = *std::localtime(&currentTime);

		static char timeString[20];

		std::strftime(timeString, 20 , "%F %T", &localTime);

		return timeString;
	}

	Logger::Logger(LogLevel outputLogLevel, std::ostream *stream)
	: mEnableTimestamp(false)
	, mStream(stream)
	, mChildLogger(nullptr)
	, mStreamLogLevel(LOGLEVEL_INFO)
	, mOutputLogLevel(outputLogLevel)
	{
	}

	void Logger::log(const std::string &msg, LogLevel level)
	{
		if(level > mOutputLogLevel)
		{
			return;
		}

	    if(mChildLogger != nullptr)
	    {
	        mChildLogger->log(msg, level);
	    }

	    if(mStream == nullptr || !mStream->good())
	    {
	        return;
	    }

	    if(mEnableTimestamp)
	    {
	        *mStream << "[" << getTimestamp() << "]";
	    }

        *mStream << "[" << getLabelForLevel(level) << "] " << msg << std::endl;
        
        for(uint32_t i = 0; i < mListeners.size(); ++i)
        {
            mListeners[i]->onLog(msg, level);
        }
	}

    void Logger::setEnableTimestamp(bool ts)
    {
        mEnableTimestamp = ts;
    }

    void Logger::setOutputLogLevel(LogLevel level)
    {
    	mOutputLogLevel = level;
    }

    void Logger::setOutputStream(std::ostream *s)
    {
        mStream = s;
    }

    void Logger::setChildLogger(Logger *l)
    {
        mChildLogger = l;
    }
    
    std::string Logger::getLabelForLevel(LogLevel level)
    {
        switch(level)
		{
		case LOGLEVEL_ERROR:
		    return "ERRO";

		case LOGLEVEL_WARNING:
		    return "WARN";

		case LOGLEVEL_DEBUG:
		    return "DBUG";

		case LOGLEVEL_INFO:
		    return "INFO";
		    
		case LOGLEVEL_VERBOSE:
			return "VERB";

		default:
		    return "????";
		}
    }

    void Logger::addListener(ILoggerListener *listener)
    {
        assert(listener != nullptr);
        
        mListeners.push_back(listener);
    }
    
    void Logger::removeListener(ILoggerListener *listener)
    {
        assert(listener != nullptr);
        
        std::vector<ILoggerListener*>::iterator it = std::find(mListeners.begin(), mListeners.end(), listener);
        
        if(it != mListeners.end())
        {
            mListeners.erase(it);
        }
    }

    Logger Logger::smDefaultLogger(LOGLEVEL_INFO);

    Logger &Logger::getDefaultLogger()
    {
        return smDefaultLogger;
    }

    template <>
    LoggerStreamProxy Logger::operator<< <Logger::LogLevel>(const Logger::LogLevel &t)
	{
		mStreamLogLevel = t;

		LoggerStreamProxy p(*this);
		return p;
	}

    LoggerStreamProxy Logger::info()
    {
    	getDefaultLogger().mStreamLogLevel = LOGLEVEL_INFO;

    	LoggerStreamProxy p(getDefaultLogger());
    	return p;
    }

	LoggerStreamProxy Logger::warn()
    {
    	getDefaultLogger().mStreamLogLevel = LOGLEVEL_WARNING;

    	LoggerStreamProxy p(getDefaultLogger());
    	return p;
    }

	LoggerStreamProxy Logger::error()
    {
    	getDefaultLogger().mStreamLogLevel = LOGLEVEL_ERROR;

    	LoggerStreamProxy p(getDefaultLogger());
    	return p;
    }

	LoggerStreamProxy Logger::verbose()
    {
    	getDefaultLogger().mStreamLogLevel = LOGLEVEL_VERBOSE;

    	LoggerStreamProxy p(getDefaultLogger());
    	return p;
    }

	LoggerStreamProxy Logger::debug()
    {
    	getDefaultLogger().mStreamLogLevel = LOGLEVEL_DEBUG;

    	LoggerStreamProxy p(getDefaultLogger());
    	return p;
    }

    void Logger::_flushLogStream()
    {
    	log(mStreamBuffer.str(), mStreamLogLevel);
    	mStreamBuffer.str("");
    }



    LoggerStreamProxy::~LoggerStreamProxy()
    {
    	// if this proxy was not copied, like what would happen if it was the last to be returned by
    	//  a statement like Logger << "foo" << 42 << "bar"; it has to notify it's associated Logger
    	//  to flush all stream output
    	if(!mIWasCopied)
    	{
    		mLogger._flushLogStream();
    	}
    }

    LoggerStreamProxy::LoggerStreamProxy(LoggerStreamProxy &p)
	: mLogger(p.mLogger)
	, mIWasCopied(false)
	{
		p.mIWasCopied = true;
	}

	LoggerStreamProxy::LoggerStreamProxy(LoggerStreamProxy &&p)
	: mLogger(p.mLogger)
	, mIWasCopied(false)
	{
		p.mIWasCopied = true;
	}

	LoggerStreamProxy::LoggerStreamProxy(Logger &l)
	: mLogger(l)
	, mIWasCopied(false)
	{
	}



    ILoggerListener::~ILoggerListener()
    {

    }

//}


