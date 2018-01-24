/*
 * Logger.h
 *
 *  Created on: Nov 18, 2015
 *      Author: zalasus
 */

#ifndef INCLUDE_LOGGER_H_
#define INCLUDE_LOGGER_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace od
{
    class ILoggerListener;

    class Logger;

    class LoggerStreamProxy
	{
    	friend class Logger;

	public:

		~LoggerStreamProxy();

		template <typename T>
		LoggerStreamProxy operator<<(const T &t);


	private:

		LoggerStreamProxy(Logger &l);
		LoggerStreamProxy(LoggerStreamProxy &p);


		Logger &mLogger;
		bool mIWasCopied;
	};



	class Logger
	{
    	friend class LoggerStreamProxy;

	public:

		enum LogLevel
		{
			LOGLEVEL_INFO,
			LOGLEVEL_WARNING,
			LOGLEVEL_SEVERE,
			LOGLEVEL_DEBUG
		};
		static const LogLevel DEFAULT_LOGLEVEL = LOGLEVEL_INFO;


		Logger(const std::string &name, std::ostream *stream = &std::cout);

		void log(const std::string &msg, LogLevel level = DEFAULT_LOGLEVEL);

        std::string getLoggerName();
        void setEnableTimestamp(bool ts);
        void setPrintDebug(bool b);
        void setOutputStream(std::ostream *s);
        void setChildLogger(Logger *l);
        void addListener(ILoggerListener *listener);
        void removeListener(ILoggerListener *listener);
        /**
         * Should be 4 characters (looks nice and tidy)
         */
        std::string getLabelForLevel(LogLevel level);

        template <typename T>
        LoggerStreamProxy operator<<(const T &t);

        static Logger &getDefaultLogger();
		static inline void info(const std::string &msg) { getDefaultLogger().log(msg, LOGLEVEL_INFO); }
		static inline void warn(const std::string &msg) { getDefaultLogger().log(msg, LOGLEVEL_WARNING); }
		static inline void severe(const std::string &msg) { getDefaultLogger().log(msg, LOGLEVEL_SEVERE); }
		static inline void debug(const std::string &msg) { getDefaultLogger().log(msg, LOGLEVEL_DEBUG); }

		// default stream getters
		static LoggerStreamProxy info();
		static LoggerStreamProxy warn();
		static LoggerStreamProxy severe();
		static LoggerStreamProxy debug();

	private:

		void _flushLogStream();

		std::string mName;
		bool mEnableTimestamp;
		bool mPrintDebugInfo;
		std::ostream *mStream;
		Logger *mChildLogger;
		std::vector<ILoggerListener*> mListeners;
		LogLevel mStreamLogLevel;
		std::ostringstream mStreamBuffer;

		static Logger smDefaultLogger;

	};



	//TODO: Merge child logger and listener interfaces or remove one of them
	class ILoggerListener
	{
    public:

		virtual ~ILoggerListener();

        virtual void onLog(const std::string &msg, Logger::LogLevel level) = 0;
	};



	template <typename T>
	LoggerStreamProxy LoggerStreamProxy::operator<<(const T &t)
	{
		mLogger.mStreamBuffer << t;

		return *this;
	}



	// specialization for setting log level by sending it to the stream
	template <> LoggerStreamProxy Logger::operator<< <Logger::LogLevel>(const Logger::LogLevel &t);

	template <typename T>
    LoggerStreamProxy Logger::operator<<(const T &t)
	{
		mStreamBuffer << t;

		return LoggerStreamProxy(*this);
	}
}

#endif /* INCLUDE_LOGGER_H_ */
