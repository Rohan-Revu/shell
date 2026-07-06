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
    for (size_t i = 0; i < jobs.size(); i++)
    {
        char marker = ' ';

        if (i == jobs.size() - 1)
            marker = '+';
        else if (jobs.size() >= 2 && i == jobs.size() - 2)
            marker = '-';

        std::cout << "[" << jobs[i].jobNumber << "]" << marker << "  " << std::left << std::setw(24) << jobs[i].status << jobs[i].command << std::endl;
    }
}