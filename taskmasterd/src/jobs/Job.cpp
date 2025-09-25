#include <taskmasterd/include/jobs/Job.hpp>

#include <utils/include/utils.hpp>

namespace taskmasterd
{
Job::Job(const JobConfig& config) : _config(config), _pgid(0) {}

void Job::start()
{
    for (i32 i = 0; i < _config.numprocs; i++) {
        std::string proc_name = _config.name + "_" + std::to_string(i);
        Process&    proc      = _processes.emplace_back(proc_name, _pgid);

        proc.start(_config.cmd, nullptr, nullptr);
        if (_pgid == 0) {
            _pgid = proc.getPid();
        }
    }
}

void Job::stop()
{
    for (auto& proc : _processes) {
        proc.stop();
    }
}
} // namespace taskmasterd
