#pragma once
#include <atomic>

namespace taskmasterd
{
inline std::atomic<bool> g_running{true};
}