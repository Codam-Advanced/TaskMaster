#include <taskmasterd/include/jobs/JobConfig.hpp>

namespace taskmasterd
{
JobConfig::JobConfig(const std::string& name, const std::string& cmd)
    : name(name), cmd(cmd), working_dir("."), numprocs(1), umask(022), autostart(false),
      restart_policy(RestartPolicy::NEVER), exit_codes({0}), start_retries(3), start_time(1),
      stop_time(5), stop_signal(Signal::TERMINATE), stdout_file(std::nullopt),
      stderr_file(std::nullopt), _env()
{
}
} // namespace taskmasterd
