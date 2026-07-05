#include "completeRegistry.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <unordered_map>

static std::unordered_map<std::string, std::string> completions;

void registerCompleter(const std::string& command, const std::string& completer) {
    completions[command] = completer;
}


std::string getCompleter(const std::string& command){
    auto it = completions.find(command);

    if (it == completions.end())
        return "";

    return it->second;

}

std::vector<std::string> runCompleter(const std::string& command){
     std::vector<std::string> result;

    std::string script = getCompleter(command);

    if (script.empty())
        return result;

    //create a pipe to capture the output of the script
    int pipefd[2];

    if (pipe(pipefd) == -1)
        return result;

    //fork a child process to run the script
    pid_t pid = fork();

    if (pid == -1)
        return result;

    // In the child process, redirect stdout to the pipe and execute the script
    if (pid == 0)
    {
        dup2(pipefd[1], STDOUT_FILENO);

        close(pipefd[0]);
        close(pipefd[1]);

        execl(script.c_str(), script.c_str(), nullptr);

        exit(1);
    }
    //close parent pipe
    close(pipefd[1]);

    //convert the output of the script into a vector of strings
    char buffer[4096];
    ssize_t bytes = read(pipefd[0], buffer, sizeof(buffer)-1);
    waitpid(pid, nullptr, 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';

        std::stringstream ss(buffer);

        std::string line;

        while (std::getline(ss, line))
        {
            if (!line.empty())
                result.push_back(line);
        }
    }
    
    //close the read end of the pipe
    close(pipefd[0]);

    return result;
}