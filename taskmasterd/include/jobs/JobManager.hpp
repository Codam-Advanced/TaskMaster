#pragma once

#include <string>
#include <proto/taskmaster.pb.h>
#include <taskmasterd/include/jobs/Job.hpp>
#include <unordered_map>

namespace taskmasterd
{
class JobManager
{

public:
    JobManager() = delete;

    /**
     * @brief Construct a job manager that accepts the config file path
     * it will parse and constuct the individual jobs
     *
     * @param config_path
     */
    JobManager(const std::string& config_path);

    ~JobManager();

    /**
     * @brief Start all programs that are marked with auto start
     * this will start the main job manager
     *
     */
    void start();

    /**
     * @brief Start a specific program specified by its name
     *
     * @param job_name
     * @throw std::runtime_error if the job cannot be found.
     */
    proto::CommandResponse start(const std::string& job_name);

    /**
     * @brief Stop a specific program specified by its name
     *
     * @param job_name
     * @throw std::runtime_error if the job cannot be found.
     */
    proto::CommandResponse stop(const std::string& job_name);

    /**
     * @brief Stop all programs as soon as possible this may be used
     * for reloading the config file and stopping the main program gracefully.
     */
    void kill();

    /**
     * @brief Restart a specific program specified by its name
     * if a program is not running this will start the program
     *
     * @param job_name
     * @throw std::runtime_error if the job cannot be found.
     */
    proto::CommandResponse restart(const std::string& job_name);

    /**
     * @brief Reload the default configuration file
     * this will stop all jobs and start all jobs with the autostart config
     */
    void reload();

    /**
     * @brief Reload the with a specific configuration file
     * this will stop all jobs and start all jobs with the autostart config
     */
    proto::CommandResponse reload(const std::string& config_path);

    /**
     * @brief Returns the status of all jobs inside of a CommandResponse.
     */
    proto::CommandResponse status();

    /**
     * @brief Returns the status of a specific job inside of a CommandResponse.
     */
    proto::CommandResponse status(const std::string& job_name);

private:
    /**
     * @brief Helper function to find a specific job in the map
     *
     * @param job_name
     * @return Job&
     * @throw std::runtime_error if the job cannot be found.
     */
    Job& findJob(const std::string& job_name);

    /**
     * @brief Helper function to rebuild all jobs in a map with a specific config
     *
     */
    void reloadJobs(const std::string& config_path);

    std::unordered_map<std::string, Job> _jobs;
    std::string                          _config;
};

} // namespace taskmasterd