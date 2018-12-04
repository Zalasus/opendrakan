/*
 * NuLogger.h
 *
 *  Created on: 4 Dec 2018
 *      Author: zal
 */

#ifndef INCLUDE_ODCORE_NULOGGER_H_
#define INCLUDE_ODCORE_NULOGGER_H_

#include <mutex>
#include <ostream>
#include <sstream>
#include <vector>

namespace od
{
    enum class LogLevel
    {
        Error,
        Warning,
        Info,
        Verbose,
        Debug
    };

    class LogListener
    {
    public:

        virtual ~LogListener() = default;

        virtual void onLog(LogLevel lvl, const std::string &message) = 0;
    };

    /**
     * @brief A simple logger for stream-like logging.
     *
     * Logging on a specific level is done similarly to outputting on std::cout:
     * @code
     *      logger.logInfo() << "Log message. This can be anything you could write into std::cout: " << 1234.0;
     * @endcode
     *
     * No std::endl is needed to terminate a log message.
     *
     * A default logger which will log to std::cout with the lowest possible level of LogLevel::Error by default is
     * available via the static method Logger::getDefaultLogger().
     *
     * Direct logging to the default logger can be achieved using the static methods Logger::error(), Logger::warn() etc:
     * @code
     *     Logger::info() << "This will be logged to the default logger";
     * @endcode
     *
     * The Logger class is fully synchronized and can be used from multiple threads.
     *
     * Loggers can relay their log messages to a listener implementing the LogListener interface. The logger class implements
     * this interface itself so it can receive messages of other loggers. One application of this could be to have the default
     * logger log to std::cout while creating a secondary Logger that outputs to a logfile. Using this pattern, one could have
     * the console logger not output timestamps while the other logger does, or log to the file using a different loglevel etc.
     */
    class Logger : public LogListener
    {
    public:

        class StreamProxy
        {
        public:

            StreamProxy(Logger &logger, LogLevel level);
            StreamProxy(const StreamProxy &proxy);
            ~StreamProxy();

            StreamProxy &operator=(const StreamProxy &proxy) = delete;

            template <typename T>
            StreamProxy &operator<<(const T &t)
            {
                mLogger.mProxyStream << t;

                return *this;
            }


        private:

            Logger &mLogger;
            LogLevel mLogLevel;

            mutable bool mCopied;

        };

        friend class StreamProxy;

        /**
         * Construct a Logger with default output level and output stream.
         *
         * @param outputLevel    Default log level of output.
         * @param outputStream   Pointer to output stream. Can be nullptr to disable output (listeners will still receive messages).
         */
        Logger(LogLevel outputLevel, std::ostream *outputStream);

        inline StreamProxy logDebug() { return _getProxyForLevel(LogLevel::Debug); }
        inline StreamProxy logVerbose() { return _getProxyForLevel(LogLevel::Verbose); }
        inline StreamProxy logInfo() { return _getProxyForLevel(LogLevel::Info); }
        inline StreamProxy logWarn() { return _getProxyForLevel(LogLevel::Warning); }
        inline StreamProxy logError() { return _getProxyForLevel(LogLevel::Error); }

        void setOutputStream(std::ostream *output);
        void setEnableTimestamps(bool b);
        void setOutputLogLevel(LogLevel level);
        void increaseOutputLogLevel();
        void decreaseOutputLogLevel();
        void addListener(LogListener *listener);
        void removeListener(LogListener *listener);

        // implement LogListener
        virtual void onLog(LogLevel lvl, const std::string &message) override;

        static Logger &getDefaultLogger();
        inline static StreamProxy debug() { return getDefaultLogger().logDebug(); }
        inline static StreamProxy verbose() { return getDefaultLogger().logVerbose(); }
        inline static StreamProxy info() { return getDefaultLogger().logInfo(); }
        inline static StreamProxy warn() { return getDefaultLogger().logWarn(); }
        inline static StreamProxy error() { return getDefaultLogger().logError(); }


    private:

        StreamProxy _getProxyForLevel(LogLevel level);
        const char *_getTagForLevel(LogLevel level);
        void _logLocked(LogLevel level, const std::string &message); ///< Call only with mutex held!

        LogLevel mOutputLevel;
        std::ostream *mOutputStream;
        bool mEnableTimestamps;
        std::vector<LogListener*> mLogListeners;

        std::mutex mLogMutex;
        std::ostringstream mProxyStream;
    };

}

#endif /* INCLUDE_ODCORE_NULOGGER_H_ */
