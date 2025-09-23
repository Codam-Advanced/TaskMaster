#include <taskmasterd/include/jobs/Job.hpp>

namespace taskmasterd
{
Job::Job(const JobConfig& config) : _config(config), _pgid(0) {}

void Job::start()
{
    for (int i = 0; i < _config.numprocs; i++) {
        std::string proc_name = _config.name + "_" + std::to_string(i);
        Process&    proc      = _processes.emplace_back(proc_name, _pgid);

        proc.start(_config.cmd, nullptr, nullptr);
        if (_pgid == 0) {
            _pgid = proc.getPid();
        }
    }
}
} // namespace taskmasterd
