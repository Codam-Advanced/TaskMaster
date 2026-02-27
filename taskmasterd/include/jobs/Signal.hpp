#pragma once

#include <ostream>
#include <signal.h>
#include <sstream>

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

/**
 * @brief Updates the program state based on the given signal
 *
 * @param signum
 */
void signalHandler(int signum);

std::ostream& operator<<(std::ostream& os, Signals signal);

std::string to_string(Signals signal);
} // namespace taskmasterd
