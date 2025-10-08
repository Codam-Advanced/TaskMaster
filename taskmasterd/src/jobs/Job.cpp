#include <taskmasterd/include/jobs/Job.hpp>

#include <bits/stdc++.h>

#include <utils/include/utils.hpp>

namespace taskmasterd
{
Job::Job(const JobConfig& config) : _config(config), _pgid(0)
{
    _args = split_shell(_config.cmd);
    _argv.reserve(_args.size() + 1);
    for (auto& arg : _args) {
        _argv.push_back(strdup(arg.c_str()));
    }
    _argv.push_back(nullptr);

    _env.reserve(_config._env.size() + 1);
    for (auto& [key, value] : _config._env) {
        std::string env_entry = key + "=" + value;
        _env.push_back(const_cast<char*>(env_entry.c_str()));
    }
    _env.push_back(nullptr);
}

void Job::start()
{
    for (i32 i = 0; i < _config.numprocs; i++) {
        std::string proc_name = _config.name + "_" + std::to_string(i);
        Process&    proc      = _processes.emplace_back(proc_name, _pgid);

        proc.start(_argv[0], _argv.data(), _env.data());
        if (_pgid == 0) {
            _pgid = proc.getPid();
        }
    }
}

void Job::stop()
{
    for (auto& proc : _processes) {
        proc.stop(_config.stop_time);
    }
}
} // namespace taskmasterd
