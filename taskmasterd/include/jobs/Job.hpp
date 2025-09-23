#pragma once

#include <unistd.h>
#include <vector>

#include <taskmasterd/include/jobs/JobConfig.hpp>
#include <taskmasterd/include/jobs/Process.hpp>

namespace taskmasterd
{
class Job
{
public:
    Job(const JobConfig& config);
    virtual ~Job() = default;

    void start();
    void stop();
    void restart();

private:
    JobConfig _config;

    pid_t                _pgid;
    std::vector<Process> _processes;
};
} // namespace taskmasterd
