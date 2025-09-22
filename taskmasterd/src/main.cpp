#include <logger/include/Logger.hpp>
#include <taskmasterd/include/example.hpp>

int main(int argc, char** argv)
{
    testFunc();

    if (argc > 2) {
        LOG_DEBUG("argc > 2")
        return 1;
    }

    LOG_DEBUG("argc <= 2")
    return 0;
}
