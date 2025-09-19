#include <iostream>
#include <yaml-cpp/yaml.h>
#include "job.hpp"

int main() {
  Job job("Example Job");
  std::cout << "Hello, Taskmaster!" << std::endl;

  // Load configuration from YAML file
  YAML::Node config = YAML::LoadFile("../config.yaml");

  job.execute();
  return 0;

}