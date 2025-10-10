#include <utils.hpp>

#include <sys/syscall.h>
#include <wordexp.h>

i32 pidfd_send_signal(i32 pidfd, i32 sig, const siginfo_t* info, u32 flags)
{
    return syscall(SYS_pidfd_send_signal, pidfd, sig, info, flags);
}

i32 pidfd_open(pid_t pid, u32 flags)
{
    return syscall(SYS_pidfd_open, pid, flags);
}

std::vector<std::string> split_shell(const std::string& str)
{
    wordexp_t                p;
    std::vector<std::string> result;

    if (wordexp(str.c_str(), &p, 0) == 0) {
        for (size_t i = 0; i < p.we_wordc; i++) {
            result.emplace_back(p.we_wordv[i]);
        }
        wordfree(&p);
    }

    return result;
}
