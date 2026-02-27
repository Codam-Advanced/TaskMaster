#pragma once

#include <iostream>
#include <memory>
#include <unistd.h>
#include <vector>

#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <taskmasterd/include/jobs/Process.hpp>

namespace taskmasterd
{

class Process;
class JobManager;
class Job
{
public:
    enum class State
    {
        EMPTY,    // The first time job is created or the config has been reloaded
        STARTING, // the job is starting all its processes
        RUNNING,  // the job is running all its processes
        STOPPING, // the job is stopping all its processes
        STOPPED,  // the job is stopped all its processes
        REPLACE,  // the job is marked to be replaced by another job with a different config
        REMOVE,   // the job is marked to be removed from the job manager
    };

    /**
     * @brief Construct a new Job object.
     *
     * @param config The job configuration.
     */
    Job(const JobConfig& config, JobManager& manager);
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
     * @brief function that is called by a process when it exited
     *
     * This method will handle any exit removing or auto restarting a new process
     * 
     */
    void onExit(Process&, i32 status_code);

    /**
     * @brief function that is called by a process when it exited
     *
     * This method will handle any exit removing or auto restarting a new process
     */
    void onStop(Process&);

    /**
     * @brief function that is called by a process when it successfully started
     *
     * This method is needed to update the job state to running when all processes surpasses the start time 
     * @param proc 
     */
    void onProcessSurpassedStartTime();

    /**
     * @brief Get the job configuration.
     *
     * @return The job configuration.
     */
    const JobConfig& getConfig() const { return _config; }

    /**
     * @brief Mark the job to be replaced
     *
     */
    void replace() { _state = State::REPLACE; }

    /**
     * @brief Should the job be replaced
     *
     * @return true if it should be replaced
     */
    bool replaced() { return _state == State::REPLACE; }

    /**
     * @brief Mark the job to be removed
     *
     */
    void remove() { _state = State::REMOVE; }

    /**
     * @brief Should the job be removed
     *
     * @return true if it should be removed
     */
    bool removed() { return _state == State::REMOVE; }

    friend Process;

    /**
     * @brief Get the state of a process at a specific index belonging to this Job.
     */
    const std::unique_ptr<Process>& getProcess(const u32 index) const { return _processes.at(index); }

    /**
     * @brief Get the amount of processes belonging to this Job.
     */
    u32 getProcessCount() const { return _processes.size(); }

    /**
     * @brief Get the state of this job.
     */
    State getState() const { return _state; }

private:
    /**
     * @brief Helper method to create and start each process
     *
     */
    void startProcesses();

    /**
     * @brief Helper method to restart existing processes
     *
     */
    void restartProcesses();

    /**
     * @brief Helper method to check if all processes are in certain states.
     *
     * @param  Process::State vector state a process state
     * @return boolean
     */
    bool allProcessesInStates(std::vector<Process::State> state);

    /**
     * @brief Helper method to parse argument (argv, cmd)
     *
     */
    void parseArguments(const JobConfig& config);

    /**
     * @brief Helper method to parse Parse environment variables
     *
     */
    void parseEnvironment(const JobConfig& config);

    JobConfig                _config;
    JobManager&              _manager;
    std::vector<std::string> _args;
    std::vector<const char*> _argv;
    std::vector<const char*> _env;

    State                                 _state;
    pid_t                                 _pgid;
    std::vector<std::unique_ptr<Process>> _processes;
};

std::ostream& operator<<(std::ostream& os, const Job& job);

} // namespace taskmasterd
