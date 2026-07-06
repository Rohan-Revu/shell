#include <iostream>
#include <iomanip>
#include <vector>
#include "jobs.h"

static std::vector<Job> jobs;
static int nextJobNumber = 1;

int addJob(pid_t pid, const std::string& command){
    jobs.push_back({ nextJobNumber++, pid, command, "Running" });
    return jobs.back().jobNumber;
}

void printJobs(){
    for (const auto& job : jobs)
    {
        std::cout << "[" << job.jobNumber << "]+  " << std::left << std::setw(24) << job.status << job.command << std::endl;
    }
}