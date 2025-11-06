#pragma once

#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#define COLOR_RESET   "\033[0m"
#define COLOR_FATAL   "\033[38;5;208m"
#define COLOR_ERROR   "\033[38;5;160m"
#define COLOR_WARNING "\033[38;5;226m"
#define COLOR_INFO    "\033[38;5;13m"
#define COLOR_DEBUG   "\033[38;5;21m"
#define COLOR_GRAY    "\033[38;5;232m"

namespace Logger
{

/**
 * @brief Enum describing the severity of the log.
 */
enum class LogType
{
    Fatal,
    Error,
    Warning,
    Info,
    Debug,
};

/**
 * @brief Enum describing how much to log.
 *
 * @note  Debug: Logs all log types.
 *        Normal: Logs everything except debug.
 *        Sparse: Only logs Error, Fatal and warnings.
 *        None: self explanatory.
 */
enum class LogLevel
{
    None,
    Sparse,
    Normal,
    Debug,
};

/**
 * @brief Singleton logger for a unified output.
 *
 * @note Debug output will be logged onto the stdout, other logs will be sent to the syslog.
 */
class LogInterface
{
    const bool _enableLoggingStdout;
    LogLevel   _logLevel;

    /**
     * @brief Hidden default constructor.
     */
    LogInterface(const char* processName, LogLevel logLevel, bool enableLoggingStdout);

    /**
     * @brief Deleted constructor and operator.
     */
    LogInterface(const LogInterface&)                             = delete;
    std::unique_ptr<LogInterface>& operator=(const LogInterface&) = delete;

    /**
     * @brief Returns true or false based on the given log leven and how the Interface is
     * initialzed.
     */
    bool CanLogLogWithSetLogLevel(LogType logType);

    /**
     * @brief Returns a string literal containing the color meant for the given LogType.
     */
    constexpr const char* GetLogColor(LogType LogType);

public:
    /**
     * @brief Destructor.
     */
    ~LogInterface();

    /**
     * @return The singleton instance.
     */
    static std::unique_ptr<LogInterface>& GetInstance();

    /**
     * @brief Initializes the instance, logging without initializing is not possible.
     *
     * @param processName The name of the process.
     *
     * @param logLevel The level at which output will be shown,
     * see the LogLevel enum for more details.
     *
     * @param enableLoggingStdout When enabled, will log the non-debug messages to the
     * stdout as well as the syslog.
     */
    static void Initialize(const char* processName, LogLevel logLevel, bool enableLoggingStdout);

    /**
     * @brief Function used for logging a message in the format for the LogType.
     */
    void Log(const std::string& msg, const LogType logType);
};

} /* namespace Logger */

#define LOG_ERROR(logMessage)   Logger::LogInterface::GetInstance()->Log(logMessage, Logger::LogType::Error);
#define LOG_FATAL(logMessage)   Logger::LogInterface::GetInstance()->Log(logMessage, Logger::LogType::Fatal);
#define LOG_WARNING(logMessage) Logger::LogInterface::GetInstance()->Log(logMessage, Logger::LogType::Warning);
#define LOG_INFO(logMessage)    Logger::LogInterface::GetInstance()->Log(logMessage, Logger::LogType::Info);
#define LOG_DEBUG(logMessage)   Logger::LogInterface::GetInstance()->Log(logMessage, Logger::LogType::Debug);
