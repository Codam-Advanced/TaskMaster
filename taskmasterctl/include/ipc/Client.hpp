#pragma once

#include <ipc/include/Socket.hpp>
#include <proto/taskmaster.pb.h>

namespace taskmasterctl
{

ipc::Socket connectToDaemon();

void sendCommandToDaemon(ipc::Socket& socket, proto::Command& command);

/**
 * @return True if ctl should exit after the received response
 */
bool awaitDaemonResponse(ipc::Socket& socket, proto::Command& command);

} // namespace taskmasterctl
