#pragma once
#include <atomic>

namespace taskmasterd
{
enum class State
{
    RUNNING,
    RELOAD,
    TERMINATED,
};

inline std::atomic<State> g_state{State::RUNNING};
} // namespace taskmasterd