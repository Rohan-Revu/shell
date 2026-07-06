#include "jobs.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <sys/wait.h>

static std::vector<Job> jobs;
static int nextJobNumber = 1;

int addJob(pid_t pid, const std::string& command){
    int jobNumber = 1;
    if (!jobs.empty()){
        jobNumber = jobs.back().jobNumber + 1;
    }

    jobs.push_back({ jobNumber, pid, command, JobStatus::Running });

    return jobNumber;
}



static void updateJobStatus()
{
    for (auto& job : jobs)
    {
        int status;
        pid_t result = waitpid(job.pid, &status, WNOHANG);

        if (result == job.pid && WIFEXITED(status))
        {
            job.status = JobStatus::Done;
        }
    }
}


static void removeDoneJobs()
{
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

void printJobs()
{
    updateJobStatus();

    for (size_t i = 0; i < jobs.size(); i++)
    {
        auto& job = jobs[i];
        char marker = ' ';

        if (i == jobs.size() - 1)
            marker = '+';
        else if (jobs.size() >= 2 && i == jobs.size() - 2)
            marker = '-';

        std::cout << "[" << job.jobNumber << "]"
                  << marker << "  ";

        if (job.status == JobStatus::Running) {
            std::cout << std::left << std::setw(24) << "Running" << job.command;
        }
        else {
            std::string command = job.command;

            if (command.size() >= 2 &&
                command.substr(command.size() - 2) == " &")
            {
                command.erase(command.size() - 2);
            }

            std::cout << std::left << std::setw(24) << "Done" << command;
        }

        std::cout << std::endl;
    }

    removeDoneJobs();
}

void reapJobs(){
    updateJobStatus();

    for (size_t i = 0; i < jobs.size(); i++)
    {
        auto& job = jobs[i];

        if (job.status != JobStatus::Done)
            continue;

        char marker = ' ';

        if (i == jobs.size() - 1)
            marker = '+';
        else if (jobs.size() >= 2 && i == jobs.size() - 2)
            marker = '-';

        std::string command = job.command;

        if (command.size() >= 2 &&
            command.substr(command.size() - 2) == " &")
        {
            command.erase(command.size() - 2);
        }

        std::cout << "[" << job.jobNumber << "]" << marker << "  " << std::left << std::setw(24) << "Done" << command << std::endl;
    }

    removeDoneJobs();
}