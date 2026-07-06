#include <iostream>
#include <iomanip>
#include <vector>
#include <sys/wait.h>

#include "jobs.h"

static std::vector<Job> jobs;
static int nextJobNumber = 1;

int addJob(pid_t pid, const std::string& command){
    jobs.push_back({ nextJobNumber++, pid, command, "Running" });
    return jobs.back().jobNumber;
}

void printJobs(){
    for (auto& job : jobs)
    {
        int status;
        pid_t result = waitpid(job.pid, &status, WNOHANG);
        if (result == job.pid && WIFEXITED(status)){
            job.status = "Done";
        }
    }
    for (size_t i = 0; i < jobs.size(); i++){
        char marker = ' ';

        if (i == jobs.size() - 1)
            marker = '+';
        else if (jobs.size() >= 2 && i == jobs.size() - 2)
            marker = '-';

        

        if (job.status == "Running")
            std::cout << "[" << job.jobNumber << "]" << marker << "  " << std::left << std::setw(24) << job.status << job.command << std::endl;
        else
            std::cout << "[" << job.jobNumber << "]" << marker << "  " << std::left << std::setw(24) << job.status << job.command.substr(0, job.command.size() - 2) << std::endl;
    }
}