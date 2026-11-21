#ifndef DEVTOOLS_LOGGER_HPP
#define DEVTOOLS_LOGGER_HPP

#include <filesystem>
#include <format>
#include <fstream>
#include <memory>
#include <string>

#ifdef _WIN32
#include <windows.h>

#define ENUM_CLASS_TYPE
#define ENUM_PREFIX_CONTENT
#else
#define ENUM_CLASS_TYPE class
#define ENUM_PREFIX_CONTENT LogLevel::
#endif

namespace fs = std::filesystem;

namespace DevTools {
    enum ENUM_CLASS_TYPE LogLevel {
        INFO = 0xE0,
        WARNING = 0xE1,
#ifndef _WIN32
        ERROR = 0xE2,
#endif
        FATAL = 0xE3,
        DEBUG = 0xE4,
        VERBOSE = 0xE5,
        TRACE = 0xE6,
        CRITICAL = 0xE7,
        GENERIC = 0xE8,
        USER = 0xE9
    };

    class Logger {
    public:
        explicit Logger(const std::string& name);
        ~Logger() = default;

        void log(const std::string& message);
        void log(const LogLevel& lvl, const std::string& message);

        template <typename... Args>
        void logf(const std::string& fmt, Args&&... args) {
            log(std::format(fmt, std::forward<Args>(args)...));
        }

        template <typename... Args>
        void logf(const LogLevel& lvl, const std::string& fmt, Args&&... args) {
            log(std::format(fmt, std::forward<Args>(args)...), lvl);
        }

        std::string& loggerName() {
            return logName;
        }

        void flush();

        void close();

    private:
        std::ofstream mainFileStream;
        std::string logName;

        // logger configuration (needs to be configured from the constructor)
        bool timestamp = true;
        std::string timestampFormat = "{Day}-{Month}-{Year}_{Hour}:{Minute}:{Second}";
        LogLevel defLogLevel = LogLevel::INFO;
        std::string logFormat = "[%Level%] %Timestamp% -- %Data%";
        bool appendContentUponStart = true;
        bool autoFlush = true;
    };

    std::string formatTimestamp(const std::string& in, const std::string& format);

    std::string formatLogData(const std::string& in, const std::string& format);

    inline std::string formatLogLevel(const LogLevel& lvl) {
        switch (lvl) {
        case ENUM_PREFIX_CONTENT INFO:
            return "INFO";
        case ENUM_PREFIX_CONTENT WARNING:
            return "WARNING";
        case ENUM_PREFIX_CONTENT ERROR:
            return "ERROR";
        case ENUM_PREFIX_CONTENT FATAL:
            return "FATAL";
        case ENUM_PREFIX_CONTENT DEBUG:
            return "DEBUG";
        case ENUM_PREFIX_CONTENT VERBOSE:
            return "VERBOSE";
        case ENUM_PREFIX_CONTENT TRACE:
            return "TRACE";
        case ENUM_PREFIX_CONTENT CRITICAL:
            return "CRITICAL";
        case ENUM_PREFIX_CONTENT GENERIC:
            return "GENERIC";
        case ENUM_PREFIX_CONTENT USER:
            return "USER";
        default: // unlikely to happen, but default value
            return formatLogLevel(LogLevel::DEBUG);
        }
    }

    namespace Instances {
        std::shared_ptr<Logger> getApplicationLoggingInstance();

        std::shared_ptr<Logger> getNetworkLoggingInstance();

        std::shared_ptr<Logger> getSecureLoggerInstance();

    }
}

#endif //DEVTOOLS_LOGGER_HPP
