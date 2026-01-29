#pragma once

#include <complex>
#include <optional>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>

#include <logger/include/Logger.hpp>
#include <taskmasterd/include/jobs/Signal.hpp>
#include <utils/include/utils.hpp>
#include <yaml-cpp/yaml.h>

namespace taskmasterd
{
struct JobConfig
{
    static std::unordered_map<std::string, JobConfig> getJobConfigs(const std::string& filename);

    /**
     * @brief A comparison operator overload to compare two configs. A default can be used since all members have the comparison operator
     */
    bool operator==(const JobConfig& obj) const = default;
    bool operator!=(const JobConfig& obj) const = default;

    enum class RestartPolicy
    {
        NEVER,
        ALWAYS,
        ON_FAILURE
    };

    using EnvMap    = std::unordered_map<std::string, std::string>;
    using SignalMap = std::unordered_map<std::string, Signals>;
    using PolicyMap = std::unordered_map<std::string, RestartPolicy>;

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

    Signals stop_signal;

    std::optional<std::string> out;
    std::optional<std::string> err;

    EnvMap env;

    inline static const SignalMap signals = {
        {"HUP", Signals::HUP},
        {"INT", Signals::INT},
        {"TERM", Signals::TERM},
        {"QUIT", Signals::QUIT},
        {"KILL", Signals::KILL},
        {"USR1", Signals::USR1},
        {"USR2", Signals::USR2},
    };

    inline static const PolicyMap policies = {{"true", RestartPolicy::ALWAYS}, {"unexpected", RestartPolicy::ON_FAILURE}, {"false", RestartPolicy::NEVER}};

private:
    // You are not supposed to create your own JobConfig objects, use the static method
    // getJobConfigs instead.
    JobConfig(const std::string& name, const YAML::Node& config);
};

} // namespace taskmasterd