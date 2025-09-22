# Cmake minimum version required 3.22
cmake_minimum_required(VERSION 3.22)

# Project name
project(taskmaster)

# include all libs
add_subdirectory(libs/logger)

# To add new lib: 
# add_subdirectory(libs/dir_name)

# include cmake module taskmasterd
add_subdirectory(taskmasterd)

# include cmake module taskmasterctl
add_subdirectory(taskmasterctl)