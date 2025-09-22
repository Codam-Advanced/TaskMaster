#pragma once

#include <string>

namespace Logger
{

void Log(const std::string& message);

}

#define LOG_DEBUG(logMessage) Logger::Log(logMessage);
