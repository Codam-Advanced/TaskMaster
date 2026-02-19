#pragma once

#include <proto/taskmaster.pb.h>
#include <string>
#include <taskmasterd/include/jobs/Job.hpp>
#include <unordered_map>

namespace taskmasterd
{
class JobManager
{

public:
    JobManager() = delete;

    using ConfigMap = std::unordered_map<std::string, JobConfig>;
    using JobMap    = std::unordered_map<std::string, Job>;

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
     *
     */
    proto::CommandResponse reload();

    /**
     * @brief Returns the status of all jobs inside of a CommandResponse.
     */
    proto::CommandResponse status();

    /**
     * @brief Returns the status of a specific job inside of a CommandResponse.
     */
    proto::CommandResponse status(const std::string& job_name);

    /**
     * @brief Removes or replaces jobs marked as REMOVED or REPLACED
     */
    void update();

    /**
     * @brief This function is called by a job object once it's stopped. The manager can handle how it likes
     *
     * @param job_name
     */
    void onStop(const std::string job_name);

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
     * @brief Helper functon to create a new job
     *
     */
    void createJob(const std::string& job_name);

    JobMap      _jobs;
    ConfigMap   _config;
    std::string _config_path;
};

} // namespace taskmasterd