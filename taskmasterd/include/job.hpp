#pragma once

#include <string>
#include <vector>

class Job {
public:
    Job(const std::string& name);
    void execute();

private:
    // The name of the job
    std::string _name;

    // Process ID of the job
    std::vector<pid_t> _pid;
};
