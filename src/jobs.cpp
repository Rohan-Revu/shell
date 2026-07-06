#include "jobs.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sys/wait.h>

static std::vector<Job> jobs;
static int nextJobNumber = 1;

int addJob(pid_t pid, const std::string& command)
{
    jobs.push_back({nextJobNumber++, pid, command, JobStatus::Running});
    return jobs.back().jobNumber;
}

void printJobs()
{
    for (size_t i = 0; i < jobs.size(); i++){
        auto& job = jobs[i];

        char marker = ' ';

        if (i == jobs.size() - 1)
            marker = '+';
        else if (jobs.size() >= 2 && i == jobs.size() - 2)
            marker = '-';

        std::cout << "[" << job.jobNumber << "]" << marker << "  " << std::left << std::setw(24) << "Running" << job.command << std::endl;
    }
}

void reapJobs(){
    for (auto& job : jobs){
        int status;
        pid_t result = waitpid(job.pid, &status, WNOHANG);

        if (result == job.pid && WIFEXITED(status))
        {
            job.status = JobStatus::Done;
        }
    }
    for (size_t i = 0; i < jobs.size(); i++){
        if (jobs[i].status != JobStatus::Done)
            continue;

        char marker = ' ';

        if (i == jobs.size() - 1)
            marker = '+';
        else if (jobs.size() >= 2 && i == jobs.size() - 2)
            marker = '-';

        std::string command = jobs[i].command;

        if (command.size() >= 2 && command.substr(command.size() - 2) == " &") {
            command.erase(command.size() - 2);
        }

        std::cout << "[" << jobs[i].jobNumber << "]" << marker << "  " << std::left << std::setw(24) << "Done" << command << std::endl;
    }
    jobs.erase(
        std::remove_if(
            jobs.begin(),
            jobs.end(),
            [](const Job& job)
            {
                return job.status == JobStatus::Done;
            }),
        jobs.end());
}