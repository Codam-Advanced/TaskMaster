#include "job.hpp"
#include <iostream>

Job::Job(const std::string& name) : _name(name) {}

void Job::execute() {
    std::cout << "Executing job: " << _name << std::endl;
}
