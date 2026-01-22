#pragma once

#include <memory>
#include <unistd.h>
#include <iostream>
#include <vector>

#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <taskmasterd/include/jobs/Process.hpp>

namespace taskmasterd
{

class Process;
class Job
{
public:
    enum class State
    {
        EMPTY,     // The first time job is created or the config has been reloaded
        STARTING,  // the job is starting all its procceses
        RUNNING,   // the job is running all its procceses
        STOPPING,  // the job is stopping all its procceses
        STOPPED,   // the job is stopped all its procceses
        RELOADING, // the job is reloading its configuration file
    };

    /**
     * @brief Construct a new Job object.
     *
     * @param config The job configuration.
     */
    Job(const JobConfig& config);
    virtual ~Job() = default;

    /**
     * @brief Start all processes defined in the job configuration.
     *
     * This method forks and execs the specified command for 'numprocs' times
     */
    void start();

    /**
     * @brief Stop all running processes in the job.
     *
     * This method sends a SIGTERM signal to all processes in the job.
     * It starts a timer to wait for 'stop_time' seconds for each process to exit gracefully.
     */
    void stop();

    /**
     * @brief Reload the job with a new configurations.
     *
     * This method will stop all proccess and create new processes once all proccesses are stopped.
     * This event will set a reloading state untill all processes are restarted.
     */
    void reload(const JobConfig& config);

    /**
     * @brief function that is called by a process when it exited
     *
     * This method will handle any exit removing or auto restarting a new process
     */
    void onExit(Process&, i32 status_code);


    /**
     * @brief function that is called by a process when it exited
     *
     * This method will handle any exit removing or auto restarting a new process
     */
    void onStop(Process&);

    /**
     * @brief Get the job configuration.
     *
     * @return The job configuration.
     */
    const JobConfig& getConfig() const
    {
        return _config;
    }

    /**
     * @brief Get the state of a process at a specific index belonging to this Job.
     */
    const std::unique_ptr<Process>& getProcess(const u32 index) const { return _processes.at(index); }

    /**
     * @brief Get the amount of processes belonging to this Job.
     */
    u32 getProcessCount() const {return _processes.size(); }

private:
    /**
     * @brief Helper method to create and start each process
     *
     */
    void startProcesses();

    /**
     * @brief Helper method to restart existing proccesses
     *
     */
    void restartProcesses();

    /**
     * @brief Helper method to parse argument (argv, cmd)
     *
     */
    void parseArguments(const JobConfig& config);

    /**
     * @brief Helper method to parse Parse enviroment variables
     *
     */
    void parseEnviroment(const JobConfig& config);

    JobConfig                _config;
    std::vector<std::string> _args;
    std::vector<const char*> _argv;
    std::vector<const char*> _env;

    State                                 _state;
    pid_t                                 _pgid;
    i32                                   _stopped;
    std::vector<std::unique_ptr<Process>> _processes;
};

std::ostream& operator<<(std::ostream& os, const Job& job);

} // namespace taskmasterd
