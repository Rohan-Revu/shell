#pragma once

#include <string>
#include <vector>
#include <sys/types.h>

struct Job {
    int jobNumber;
    pid_t pid;
    std::string command;
    std::string status;
};

std::int addJob(pid_t pid, const std::string& command);
void printJobs();