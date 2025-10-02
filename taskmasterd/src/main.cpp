#include "Logger.hpp"
#include "jobs/jobConfig.hpp"
#include "yaml-cpp/yaml.h"

int main(int argc, char** argv)
{
    if (argc > 2) {
        LOG_DEBUG("argc > 2")
        return 1;
    }

    try 
    {
        LOG_DEBUG("Start of Program");
        YAML::Node config = YAML::LoadFile(argv[1])["jobs"];

        taskmasterd::JobConfig jobConfig("nginx", config);


    }
    catch (std::exception e)
    {
        LOG_DEBUG(e.what());
    }
    return 0;
}
