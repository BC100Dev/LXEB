#ifndef SEB_ILOGGER_HPP
#define SEB_ILOGGER_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include "LogLevel.hpp"
#include "ILogContent.hpp"

namespace SEB::Logging {

    class ILogger {
    public:
        virtual ~ILogger() = default;

        virtual LogLevel GetLogLevel() const = 0;
        virtual void SetLogLevel(LogLevel level) = 0;

        virtual void Debug(const std::string& message) = 0;
        virtual void Info(const std::string& message) = 0;
        virtual void Warn(const std::string& message) = 0;
        virtual void Error(const std::string& message) = 0;
        virtual void Error(const std::string& message, const std::exception& exception) = 0;
        virtual void Log(const std::string& message) = 0;

        virtual std::vector<std::shared_ptr<ILogContent>> GetLog() const = 0;
    };

}

#endif //SEB_ILOGGER_HPP