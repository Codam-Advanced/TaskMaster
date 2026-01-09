#pragma once

#include <memory>
#include <unistd.h>
#include <iostream>
#include <vector>

#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <taskmasterd/include/jobs/Process.hpp>

namespace taskmasterd
{
class Job
{
public:
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
     * @brief function that is called by a process when it exited
     * 
     * This method will handle any exit removing or auto restarting a new process
     */
    void onExit(Process&, i32 status_code);

    /**
     * @brief Get the job configuration.
     *
     * @return The job configuration.
     */
    const JobConfig& getConfig() const { return _config; }

    /**
     * @brief Get the state of a process at a specific index belonging to this Job.
     */
    const std::unique_ptr<Process>& getProcess(const u32 index) const { return _processes.at(index); }

    /**
     * @brief Get the amount of processes belonging to this Job.
     */
    u32 getProcessCount() const {return _processes.size(); }

private:

    JobConfig                _config;
    std::vector<std::string> _args;
    std::vector<const char*> _argv;
    std::vector<const char*> _env;

    pid_t                _pgid;
    std::vector<std::unique_ptr<Process>> _processes;
};

std::ostream& operator<<(std::ostream& os, const Job& job);

} // namespace taskmasterd
