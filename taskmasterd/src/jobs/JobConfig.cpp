#include "logger/include/Logger.hpp"
#include <filesystem>
#include <functional>
#include <optional>
#include <stdexcept>
#include <taskmasterd/include/jobs/JobConfig.hpp>

namespace taskmasterd
{

void parseCmd(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // CMD is required throw an error
        throw std::runtime_error("ERROR: CMD is required to be set for job" + object->name);
    }

    // we dont care if command is invalid this will be catched at execve
    object->cmd = config.as<std::string>();
}

void parseNumberProcesses(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // If number of procceses is undefined we fallback to default [1]
        object->numprocs = 1;
        return;
    }
    object->numprocs = config.as<i32>();
}

void parseUnMask(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // If not present default value is set to 022 (octal)
        object->umask = 022;
        return;
    }

    std::string result = config.as<std::string>();

    // umask must be a 3 digit octal number
    if (result.length() != 3 || result.find_first_not_of("01234567") != std::string::npos) {
        throw std::runtime_error("ERROR: Invalid umask value for job " + object->name);
    }
    object->umask = static_cast<mode_t>(std::stoi(result, nullptr, 8));
}

void parseWorkingDir(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // If not present default is current working directory
        object->working_dir = ".";
        return;
    }
    object->working_dir = config.as<std::string>();
}

void parseAutoStart(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // if not present default is (true)
        object->autostart = true;
        return;
    }

    object->autostart = config.as<bool>();
}

void parseAutoRestart(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // if not present default is (unexpected)
        object->restart_policy = JobConfig::RestartPolicy::ON_FAILURE;
        return;
    }

    // we dont care that this can throw since this will be called in the constructor anyways.
    object->restart_policy = JobConfig::policies.at(config.as<std::string>());
}

void parseExitCodes(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // if not present default is (0)
        object->exit_codes.push_back(0);
        return;
    }

    // Scalar is a single value so we convert it to an single integer and push it to the vector
    if (config.IsScalar()) {
        object->exit_codes.push_back(config.as<i32>());
    } else {
        object->exit_codes = config.as<std::vector<i32>>();
    }

    // sort the vector so that it can be properly compared
    std::sort(object->exit_codes.begin(), object->exit_codes.end());
}

void parseStartRetries(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // if not present default is (3)
        object->start_retries = 3;
        return;
    }

    // convert it an i32. We dont limit the user to set an amount of retries.
    object->start_retries = config.as<i32>();
}

void parseStartTime(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // total of time a program is allowed to take before transforming into a running state
        // default here is 1 sec
        object->start_time = 1;
        return;
    }
    object->start_time = config.as<i32>();
}

void parseStopSignal(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // is not defined default will be set to TERM like supervisor
        object->stop_signal = Signals::TERM;
        return;
    }
    object->stop_signal = JobConfig::signals.at(config.as<std::string>());
}

void parseStopTime(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // The number of seconds to wait for the OS to return a SIGCHILD to taskmasterd after the program
        // has been sent a stopsignal. If this number of seconds has elasped then taskmasterd will attempt to kill it with a final SIGKILL
        // default is 10 seconds
        object->stop_time = 10;
        return;
    }
    object->stop_time = config.as<i32>();
}

void parseSTDOUT(JobConfig* object, const YAML::Node& config)
{

    if (!config.IsDefined()) {
        // If not present default is no redirection
        object->out = std::nullopt;
        return;
    }

    std::filesystem::path path = config.as<std::string>();
    std::filesystem::path dir  = path.parent_path();

    if (!std::filesystem::exists(dir)) {
        LOG_WARNING("Directory of Path: " + path.string() + "Doesn't exists! setting to null..");
        object->out = std::nullopt;
    } else {
        object->out = config.as<std::string>();
    }
}

void parseSTDERR(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // If not present default is no redirection
        object->err = std::nullopt;
        return;
    }
    std::filesystem::path path = config.as<std::string>();
    std::filesystem::path dir  = path.parent_path();

    if (!std::filesystem::exists(dir)) {
        LOG_WARNING("Option STDERR: Directory of Path: " + path.string() + "Doesn't exists! setting to null..");
        object->err = std::nullopt;
    } else {
        object->err = config.as<std::string>();
    }
}

void parseENV(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) {
        // If not present default is empty env
        object->env = JobConfig::EnvMap();
        return;
    }

    // we expect a map here so we iterate over the map and add it to our env map
    for (auto it = config.begin(); it != config.end(); ++it) {
        object->env[it->first.as<std::string>()] = it->second.as<std::string>();
    }
}

JobConfig::JobConfig(const std::string& name, const YAML::Node& config)
    : name(name)
{
    static std::unordered_map<std::string, std::function<void(JobConfig*, const YAML::Node&)>> nodes = {{"cmd", parseCmd},
                                                                                                        {"numprocs", parseNumberProcesses},
                                                                                                        {"umask", parseUnMask},
                                                                                                        {"workingdir", parseWorkingDir},
                                                                                                        {"autostart", parseAutoStart},
                                                                                                        {"autorestart", parseAutoRestart},
                                                                                                        {"exitcodes", parseExitCodes},
                                                                                                        {"startretries", parseStartRetries},
                                                                                                        {"starttime", parseStartTime},
                                                                                                        {"stopsignal", parseStopSignal},
                                                                                                        {"stoptime", parseStopTime},
                                                                                                        {"stdout", parseSTDOUT},
                                                                                                        {"stderr", parseSTDERR},
                                                                                                        {"env", parseENV}};

    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        LOG_DEBUG(("Parsing node: " + it->first).c_str());
        it->second(this, config[it->first]);
    }
}

std::unordered_map<std::string, JobConfig> JobConfig::getJobConfigs(const std::string& filename)
{
    std::unordered_map<std::string, JobConfig> jobConfigs;
    YAML::Node config = YAML::LoadFile(filename)["jobs"];

    if (!config.IsDefined()) {
        LOG_FATAL("ERROR: No 'jobs' node found in the configuration file.");
        throw std::runtime_error("ERROR: No 'jobs' node found in the configuration file.");
    }

    for (auto it = config.begin(); it != config.end(); ++it) {
        std::string name = it->first.as<std::string>();

        if (jobConfigs.find(name) != jobConfigs.end()) {
            LOG_WARNING(("ERROR: Duplicate job name found: " + name + "Skipping...").c_str());
            continue;
        }

        try {
            // Create a JobConfig object and add it to the map
            jobConfigs.emplace(name, JobConfig(name, config[name]));
        } catch (const std::exception& e) {
            LOG_ERROR(("ERROR: Failed to parse job '" + name + "': " + e.what() + " Skipping...").c_str());
            continue;
        }
    }

    // Return the map of job configurations
    // {job name, job config object}
    return jobConfigs;
}
} // namespace taskmasterd