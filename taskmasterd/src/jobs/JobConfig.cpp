#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <iostream>
#include <functional>

namespace taskmasterd
{

void parseCmd(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // CMD is required throw an error
        throw std::runtime_error("ERROR: CMD is required to be set for job" + object->name);
    }

    // Add command to the cmd
    // we dont care if command is invalid this will be catched at execve
    object->cmd = config.as<std::string>();
}

void parseNumberProcesses(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // If number of procceses is undefined we fallback to default [1]
        object->numprocs = 1;
        return;
    }

    // Add amount of procceses run. (should we set a cap supervisor doesn't have a limit)
    object->numprocs = config.as<i32>();
}

void parseUnMask(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // If not present default value is set to 022
        object->umask = 022;
        return;
    }

    // convert to mode_t and store as umask
    // @todo maybe we should check for valid inputs only
    object->umask = config.as<mode_t>();
}

void parseWorkingDir(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // If not present default is current working directory
        object->working_dir = ".";
        return;
    }

    // covert to std::string and store in workingdir
    object->working_dir = config.as<std::string>();
}

void parseAutoStart(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // if not present default is (true)
        object->autostart = true;
        return;
    }

    // convert to bool and store in autostart
    object->autostart = config.as<bool>();
}

void parseAutoRestart(JobConfig* object, const YAML::Node& config)
{
   static const std::unordered_map<std::string, JobConfig::RestartPolicy> policies =
   {
        {"true", JobConfig::RestartPolicy::ALWAYS},
        {"unexpected", JobConfig::RestartPolicy::ON_FAILURE},
        {"false", JobConfig::RestartPolicy::NEVER}
   };
   if (!config.IsDefined())
   {
        // if not present default is (unexpected)
        object->restart_policy = JobConfig::RestartPolicy::ON_FAILURE;
        return;
    }

    // get the values based on the config.
    // we dont care that this can throw since this will be called in the constructor anyways.
    object->restart_policy = policies.at(config.as<std::string>());
}

void parseExitCodes(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // if not present default is (0)
        object->exit_codes.push_back(0);
        return;
    }

    try
    {
        //this should work. we convert it straight into a vector<i32>
        object->exit_codes = config.as<std::vector<i32>>();
    }
    catch (const std::exception& e)
    {
        // if it fails we try to convert it into a single i32
        object->exit_codes.push_back(config.as<i32>());
    }
}

void parseStartRetries(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // if not present default is (3)
        object->start_retries = 3;
        return;
    }

    // convert it an i32. We dont limit the user to set an amount of retries.
    object->start_retries = config.as<i32>();
}

void parseStartTime(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // total of time a program is allowed to take before transforming into a running state
        // default here is 1 sec
        object->start_time = 1;
        return;
    }

    // convert to an i32 note that a value of 0 means don't check for start time
    object->start_time = config.as<i32>();
}

void parseStopSignal(JobConfig* object, const YAML::Node& config)
{
    static const std::unordered_map<std::string, JobConfig::Signals> signals =
   {
        {"HUP", JobConfig::Signals::HUP},
        {"INT", JobConfig::Signals::INT},
        {"TERM", JobConfig::Signals::TERM},
        {"QUIT", JobConfig::Signals::QUIT},
        {"KILL", JobConfig::Signals::KILL},
        {"USR1", JobConfig::Signals::USR1},
        {"USR2", JobConfig::Signals::USR2},
   };

   if (!config.IsDefined())
   {
        // is not defined default will be set to TERM like supervisor
        object->signal = JobConfig::Signals::TERM;
        return;
   }

   // conver to a string and the map will convert it into one of the signals
   object->signal = signals.at(config.as<std::string>());
}

void parseStopTime(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // The number of seconds to wait for the OS to return a SIGCHILD to taskmasterd after the program
        // has been sent a stopsignal. If this number of seconds has elasped then taskmasterd will attempt to kill it with a final SIGKILL
        // default is 10 seconds
        object->stop_time = 10;
        return;
    }

    // convert to a i32
    object->stop_time = config.as<i32>();
}

void parseSTDOUT(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // If not present default is no redirection
        object->out = std::nullopt;
        return;
    }

    // convert to std::string and store in out
    object->out = config.as<std::string>();
}

void parseSTDERR(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined())
    {
        // If not present default is no redirection
        object->err = std::nullopt;
        return;
    }

    // convert to std::string and store in err
    object->err = config.as<std::string>();
}

void parseENV(JobConfig* object, const YAML::Node& config)
{
    if (!config.IsDefined()) 
    {
        // If not present default is empty env
        object->env = JobConfig::EnvMap();
        return;
    }

    // we expect a map here so we iterate over the map and add it to our env map
    for (auto it = config.begin(); it != config.end(); ++it)
    {
        object->env[it->first.as<std::string>()] = it->second.as<std::string>();
    }
}

JobConfig::JobConfig(const std::string& name, const YAML::Node& config) : name(name)
{
    static std::unordered_map<std::string, std::function<void(JobConfig*, const YAML::Node&)>> nodes = {
        {"cmd", parseCmd},
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
        {"env", parseENV}
    };

    // iterate over all the nodes in the job and call the corresponding function
    for (auto it = nodes.begin(); it != nodes.end(); ++it)
    {
        // call the function with the current object and the node

        LOG_DEBUG(("Parsing node: " + it->first).c_str());
        it->second(this, config[it->first]);
    }
}

std::unordered_map<std::string, JobConfig> JobConfig::getJobConfigs(const std::string& filename)
{
    std::unordered_map<std::string, JobConfig> jobConfigs;

    try 
    {
        // Load the YAML file
        YAML::Node config = YAML::LoadFile(filename)["jobs"];

        // Check if the "jobs" node exists
        if (!config.IsDefined())
        {
            LOG_FATAL("ERROR: No 'jobs' node found in the configuration file.");
            return jobConfigs;
        }

        // Iterate over each job in the "jobs" node
        for (auto it = config.begin(); it != config.end(); ++it)
        {
            // Get the job name
            std::string name = it->first.as<std::string>();

            // Check if the job name already exists
            if (jobConfigs.find(name) != jobConfigs.end())
            {
                LOG_WARNING(("ERROR: Duplicate job name found: " + name + "Skipping...").c_str());
                continue;
            }

            try 
            {
                // Create a JobConfig object and add it to the map
                jobConfigs.emplace(name, JobConfig(name, config[name]));
            }
            catch (const std::exception& e)
            {
                LOG_ERROR(("ERROR: Failed to parse job '" + name + "': " + e.what() + " Skipping...").c_str());
                
                // Skip this job and continue with the next one
                continue;
            }
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());

        // clear the map
        jobConfigs.clear();
        
        // Return 
        return jobConfigs;
    }

    // Return the map of job configurations
    // {job name, job config object}
    return jobConfigs;
}
}