#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>


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


void typeCommand(const std::string &input) {
    std::string cmd = input.substr(5);

    std::unordered_set<std::string> builtins = {
        "echo",
        "exit",
        "type",
        "pwd"
    };

    if (builtins.contains(cmd)) {
        std::cout << cmd << " is a shell builtin\n";
        return;
    }

    std::string path = findExecutable(cmd);

    if (!path.empty())
        std::cout << cmd << " is " << path << '\n';
    else
        std::cout << cmd << ": not found\n";
}


void executeProgram(const std::string &path, const std::string &input){
  std::vector<std::string> args;
  std::stringstream ss(input);
  std::string arg;

  while(ss >> arg){
    args.push_back(arg);
  }

  std::vector<char*> argv;

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
  else{
    waitpid(pid, nullptr, 0);
  }
}

void pwdCommand(){
  std::cout << getcwd(nullptr, 0) << std::endl;
}


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;


  while(true){
    std::cout << "$ ";

    // Read line
    std::string input;
    std::getline(std::cin, input);


    std::string command = input.substr(0, input.find(" "));


    if(command == "exit"){
      break;
    }
    else if(command == "echo"){
      std::cout << input.substr(5) << std::endl;
    }
    else if(command == "type"){
      typeCommand(input);
    }
    else if(command == "pwd"){
      pwdCommad();
    }
    else{
      std::string path = findExecutable(command);

      if(path.empty())
        std::cout << command << ": not found" << std::endl;
      else{
        executeProgram(path, input);
      }
    }
    
  }
}
