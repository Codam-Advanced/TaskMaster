#include <Logger.hpp>
#include <iostream>
#include <syslog.h>

namespace Logger
{
LogInterface::LogInterface(const char* processName, LogLevel logLevel, bool enableLoggingStdout)
    : _logLevel(logLevel), _enableLoggingStdout(enableLoggingStdout)
{
    openlog(processName, LOG_NOWAIT, LOG_NOWAIT);
}

LogInterface::~LogInterface()
{
    try {
        closelog();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }
}

std::unique_ptr<LogInterface>& LogInterface::GetInstance()
{
    static std::unique_ptr<LogInterface> logger;
    return logger;
}

void LogInterface::Initialize(const char* processName, LogLevel logLevel, bool enableLoggingStdout)
{
    auto& logger = GetInstance();

    if (logger != nullptr) {
        throw std::runtime_error("Logger is already initialized.");
    }

    logger =
        std::unique_ptr<LogInterface>(new LogInterface(processName, logLevel, enableLoggingStdout));
}

bool LogInterface::CanLogLogWithSetLogLevel(LogType logType)
{
    switch (_logLevel) {
    case LogLevel::None:
        return false;
    case LogLevel::Sparse:
        return (logType != LogType::Debug && logType != LogType::Info);
    case LogLevel::Normal:
        return (logType != LogType::Debug);
    case LogLevel::Debug:
        return true;
    default:
        return false;
    }
}

constexpr const char* LogInterface::GetLogColor(LogType LogType)
{
    switch (LogType) {
    case LogType::Fatal:
        return COLOR_FATAL;
    case LogType::Error:
        return COLOR_ERROR;
    case LogType::Warning:
        return COLOR_WARNING;
    case LogType::Info:
        return COLOR_INFO;
    case LogType::Debug:
        return COLOR_DEBUG;
    default:
        return "";
    }
}

void LogInterface::Log(const std::string& logMessage, const LogType logType)
{
    if (!CanLogLogWithSetLogLevel(logType)) {
        return;
    }

    if (_enableLoggingStdout || logType == LogType::Debug) {
        std::cout << GetLogColor(logType) << logMessage << COLOR_RESET << "\n";
    }

    try {
        switch (logType) {
        case LogType::Fatal:
            syslog(LOG_DAEMON | LOG_ERR, "%s", logMessage.c_str());
            break;
        case LogType::Error:
            syslog(LOG_DAEMON | LOG_ERR, "%s", logMessage.c_str());
            break;
        case LogType::Warning:
            syslog(LOG_DAEMON | LOG_WARNING, "%s", logMessage.c_str());
            break;
        case LogType::Info:
            syslog(LOG_DAEMON | LOG_INFO, "%s", logMessage.c_str());
            break;
        case LogType::Debug:
            syslog(LOG_DAEMON | LOG_DEBUG, "%s", logMessage.c_str());
            break;
        default:
            break;
        }
    } catch (const std::exception& e) {
        std::cerr << GetLogColor(LogType::Error) << "Error using syslog: " << e.what()
                  << COLOR_RESET "\n";
    }
}

} // namespace Logger
