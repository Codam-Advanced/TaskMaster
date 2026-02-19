#pragma once

#include <ipc/include/Socket.hpp>
#include <proto/taskmaster.pb.h>

#include <atomic>

namespace taskmasterctl
{

inline std::atomic<bool> g_exitChecker;

void checkSocketState(const ipc::Socket& socket);

} // namespace taskmasterctl
