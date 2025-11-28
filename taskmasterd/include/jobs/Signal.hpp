#pragma once

#include <signal.h>

namespace taskmasterd
{
enum class Signals : int
{
    INT  = SIGINT,
    TERM = SIGTERM,
    HUP  = SIGHUP,
    QUIT = SIGQUIT,
    KILL = SIGKILL,
    USR1 = SIGUSR1,
    USR2 = SIGUSR2
};


} // namespace taskmasterd
