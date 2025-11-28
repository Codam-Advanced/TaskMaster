#pragma once

#include <ipc/include/Socket.hpp>
#include <proto/taskmaster.pb.h>

namespace taskmasterctl
{

ipc::Socket connectToDaemon();

void sendCommandToDaemon(ipc::Socket& socket, proto::Command& command);

void awaitDaemonResponse(ipc::Socket& socket);

} // namespace taskmasterctl
