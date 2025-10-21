#pragma once

#include <optional>
#include <signal.h>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#include <logger/include/Logger.hpp>
#include <utils/include/utils.hpp>
#include <yaml-cpp/yaml.h>

namespace taskmasterd
{
struct JobConfig
{
    static std::unordered_map<std::string, JobConfig> getJobConfigs(const std::string& filename);

    using EnvMap = std::unordered_map<std::string, std::string>;

    enum class RestartPolicy
    {
        NEVER,
        ALWAYS,
        ON_FAILURE
    };
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

    std::string name;
    std::string cmd;
    std::string working_dir;
    i32         numprocs;
    mode_t      umask;

    bool             autostart;
    RestartPolicy    restart_policy;
    std::vector<i32> exit_codes;

    i32 start_retries;
    i32 start_time;
    i32 stop_time;

    Signals signal;

    std::optional<std::string> out;
    std::optional<std::string> err;

    EnvMap env;

private:
    // You are not supposed to create your own JobConfig objects, use the static method
    // getJobConfigs instead.
    JobConfig(const std::string& name, const YAML::Node& config);
};
} // namespace taskmasterd