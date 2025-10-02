#pragma once

#include <optional>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#include "utils.hpp"
#include "Logger.hpp"
#include "yaml-cpp/yaml.h"


namespace taskmasterd
{
struct JobConfig
{
    JobConfig(const std::string& name, const YAML::Node& config);

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
    i32         numprocs;
    mode_t      umask;

    bool             autostart;
    RestartPolicy    restart_policy;
    std::vector<i32> exit_codes;

    i32 start_retries;
    i32 start_time;
    i32 stop_time;

    // Implement signal handling

    std::optional<std::string> out;
    std::optional<std::string> err;

    EnvMap _env;
};
} // namespace taskmasterd