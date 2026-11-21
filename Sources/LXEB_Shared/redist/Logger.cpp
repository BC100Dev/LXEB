#include <dev_tools/logging/Logger.hpp>

namespace DevTools {

    Logger::Logger(const std::string& name) {
        logName = name;
        timestamp = true;
        timestampFormat = "{Day}-{Month}-{Year}_{Hour}:{Minute}:{Second}";
        defLogLevel = LogLevel::DEBUG;
        logFormat = "[%Level%] %Timestamp% -- %Data%";
        appendContentUponStart = true;
        autoFlush = true;

        try {
            fs::path fp;

#ifdef __linux__
#endif
        } catch (...) {
        }
    };

    class AppLogger : public Logger {
    public:
        explicit AppLogger(const std::string& name)
            : Logger(name) {
        }

        void log(const std::string& message) {
        }

        void log(const LogLevel& lvl, const std::string& message) {
        }

        void flush() {
        }

        void close() {
        }
    };

    class NetLogger : public Logger {
    public:
        explicit NetLogger(const std::string& name)
            : Logger(name) {
        }

        void log(const std::string& message) {
        }

        void log(const LogLevel& lvl, const std::string& message) {
        }

        void flush() {
        }

        void close() {
        }
    };

    class SecurityLogger : public Logger {
    public:
        explicit SecurityLogger(const std::string& name)
            : Logger(name) {
        }

        void log(const std::string& message) {
        }

        void log(const LogLevel& lvl, const std::string& message) {
        }

        void flush() {
        }

        void close() {
        }
    };

    static std::shared_ptr<Logger> appLogger = nullptr;
    static std::shared_ptr<Logger> networkLogger = nullptr;
    static std::shared_ptr<Logger> secureLogger = nullptr;

    bool initializationComplete = false;

    namespace Instances {
        std::shared_ptr<Logger> getApplicationLoggingInstance() {
            if (appLogger == nullptr)
                appLogger = std::make_shared<AppLogger>("OsintgramCXX-core");

            return appLogger;
        }

        std::shared_ptr<Logger> getNetworkLoggingInstance() {
            if (networkLogger == nullptr)
                networkLogger = std::make_shared<NetLogger>("OsintgramCXX-net");

            return networkLogger;
        }

        std::shared_ptr<Logger> getSecureLoggerInstance() {
            if (secureLogger == nullptr)
                secureLogger = std::make_shared<SecurityLogger>("OsintgramCXX-storage");

            return secureLogger;
        }
    }
}
