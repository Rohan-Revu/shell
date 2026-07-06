#include "executeProgram.h"
#include "jobs.h"

#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>



std::string findExecutable(const std::string &command) {
    std::string path = getenv("PATH");
    std::istringstream ss(path);
    std::string directory;

    while (getline(ss, directory, ':')) {
        std::string fullPath = directory + "/" + command;

        if (access(fullPath.c_str(), X_OK) == 0) {
            return fullPath;
        }
    }

    return "";
}




void executeProgram(const std::string &path, const std::vector<std::string> &args, bool background, const std::string& commandLine) {
  std::vector<char*> argv;

  static int nextJobNumber = 1;


  for(auto& arg : args){
    argv.push_back(const_cast<char*>(arg.c_str()));
  }
  argv.push_back(nullptr);


  pid_t pid = fork();


  if(pid == 0){
    execv(path.c_str(), argv.data());

    perror("execv");
    exit(1);
  }
  else
  {
      if(background)
      {
          int jobNumber = addJob(pid, commandLine);
          std::cout << "[" << jobNumber << "] "<< pid << std::endl;
      }
      else
      {
          waitpid(pid, nullptr, 0);
      }
  }
}