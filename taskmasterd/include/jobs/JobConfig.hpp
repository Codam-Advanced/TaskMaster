#pragma once

#include <optional>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#include <taskmasterd/include/jobs/Signal.hpp>
#include <utils/include/utils.hpp>

namespace taskmasterd
{
struct JobConfig
{
    using EnvMap = std::unordered_map<std::string, std::string>;

    enum class RestartPolicy
    {
        NEVER,
        ALWAYS,
        ON_FAILURE
    };

    std::string name;
    std::string cmd;
    std::string working_dir;
    int         numprocs;
    mode_t      umask;

    bool             autostart;
    RestartPolicy    restart_policy;
    std::vector<int> exit_codes;

    int start_retries;
    int start_time;
    int stop_time;

    Signal stop_signal;

    std::optional<std::string> stdout_file;
    std::optional<std::string> stderr_file;

    EnvMap _env;
};
} // namespace taskmasterd
