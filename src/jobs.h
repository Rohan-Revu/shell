#pragma once

#include <string>
#include <vector>
#include <sys/types.h>

enum class JobStatus
{
    Running,
    Done
};

struct Job
{
    int jobNumber;
    pid_t pid;
    std::string command;
    JobStatus status;
};

int addJob(pid_t pid, const std::string& command);
void printJobs();
void reapJobs();