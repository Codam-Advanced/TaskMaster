#include "jobs/jobConfig.hpp"
#include <iostream>
#include <functional>

namespace taskmasterd
{

void parseCmd(JobConfig* object, const YAML::Node& config)
{
    if (config.IsDefined())
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
    if (config.IsDefined())
    {
        // If number of procceses is undefined we fallback to default [1]
        object->numprocs = 1;
        
        // return early
        return; 

    }
    // Add amount of procceses run. (should we set a cap supervisor doesn't have a limit)
    object->numprocs = config.as<i32>();
}

void parseUnMask(JobConfig* object, const YAML::Node& config)
{
    if (config.IsDefined())
    {
        // If not present default value is set to 022
        object->umask = 022;
    }

    // convert to mode_t and store as umask
    // @todo maybe we should check for valid inputs only
    object->umask = config.as<mode_t>();
    
}

void parseWorkingDir(JobConfig* object, const YAML::Node& config)
{
    if (config.IsDefined())
    {
        // If not present default is current working directory
        object->working_dir = ".";
    }

    // covert to std::string and store in workingdir
    object->working_dir = config.as<std::string>();
}

void parseAutoStart(JobConfig* object, const YAML::Node& config)
{
    
}

void parseAutoRestart(JobConfig* object, const YAML::Node& config)
{
    
}

void parseExitCodes(JobConfig* object, const YAML::Node& config)
{
    
}

void parseStartRetries(JobConfig* object, const YAML::Node& config)
{
    
}

void parseStartTime(JobConfig* object, const YAML::Node& config)
{
    
}

void parseStopSignal(JobConfig* object, const YAML::Node& config)
{
    
}

void parseStopTime(JobConfig* object, const YAML::Node& config)
{
    
}

void parseSTDOUT(JobConfig* object, const YAML::Node& config)
{
    
}

void parseSTDERR(JobConfig* object, const YAML::Node& config)
{
    
}

void parseENV(JobConfig* object, const YAML::Node& config)
{
    
}

JobConfig::JobConfig(const std::string& name, const YAML::Node& config) : name(name)
{
    static const std::unordered_map<std::string, std::function<void(JobConfig*, YAML::Node&)>> nodes = {
        {"cmd", parseCmd},
        {"numprocs", parseNumberProcesses},
        {"unmask", parseUnMask},

    };

    if (!config[name].IsDefined())
    {
        throw std::runtime_error("Couldn't find job in config file");
    }
}


}