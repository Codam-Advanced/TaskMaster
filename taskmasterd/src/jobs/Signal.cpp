#include <taskmasterd/include/jobs/Signal.hpp>

namespace taskmasterd
{
std::ostream& operator<<(std::ostream& os, Signals signal)
{
    switch (signal) {
    case Signals::INT:
        os << "SIGINT";
        break;
    case Signals::TERM:
        os << "SIGTERM";
        break;
    case Signals::HUP:
        os << "SIGHUP";
        break;
    case Signals::QUIT:
        os << "SIGQUIT";
        break;
    case Signals::KILL:
        os << "SIGKILL";
        break;
    case Signals::USR1:
        os << "SIGUSR1";
        break;
    case Signals::USR2:
        os << "SIGUSR2";
        break;
    default:
        os << "UNKNOWN SIGNAL";
        break;
    }
    return os;
}

std::string to_string(Signals signal)
{
    std::ostringstream oss;
    oss << signal;
    return oss.str();
}
} // namespace taskmasterd