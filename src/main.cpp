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


void typeCommand(const std::string &cmd) {
    std::unordered_set<std::string> builtins = {
        "echo",
        "exit",
        "type",
        "pwd",
        "cd"
    };

    if (builtins.contains(cmd)) {
        std::cout << cmd << " is a shell builtin" << std::endl;
        return;
    }

    std::string path = findExecutable(cmd);

    if (!path.empty())
        std::cout << cmd << " is " << path << std::endl;
    else
        std::cout << cmd << ": not found" << std::endl;
}

void echoCommand(const std::vector<std::string> &args){
  for (size_t i = 1; i < args.size(); i++) {
    if (i > 1)
        std::cout << " ";

    std::cout << args[i];
  }
  std::cout << std::endl;
}


void executeProgram(const std::string &path, const std::vector<std::string> &args){
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

void cdCommand(std::string directory){
  if(directory == "~"){
    directory = getenv("HOME");
  }

  if(chdir(directory.c_str()) != 0){
    std::cout << "cd: " << directory << ": No such file or directory" << std::endl;
  }

}


std::vector<std::string> parseArguments(const std::string &input) {
    std::vector<std::string> args;
    std::string current;

    bool inSingleQuote = false;
    bool inDoubleQuote = false;

    for (int i=0; i < input.size(); i++) {
        char c = input[i];


        if(c=='\\' && !inSingleQuote){ 
          if(i + 1 < input.size()){
            current += input[i + 1];
            i++;
          }
        }
        else if (c == '\'' && !inDoubleQuote) {
            inSingleQuote = !inSingleQuote;
        }
        else if (c == '"' && !inSingleQuote) {
            inDoubleQuote = !inDoubleQuote;
        }
        else if (c == ' ' && !inSingleQuote && !inDoubleQuote) {
            if (!current.empty()) {
                args.push_back(current);
                current.clear();
            }
        }
        else {
            current += c;
        }
    }

    if (!current.empty())
        args.push_back(current);

    return args;
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


    std::vector<std::string> args = parseArguments(input);
    if (args.empty()) {
      continue;
    }
    std::string command = args[0];


    if(command == "exit"){
      break;
    }
    else if(command == "echo"){
      echoCommand(args);
    }
    else if(command == "type"){
      typeCommand(args[1]);
    }
    else if(command == "pwd"){
      pwdCommand();
    }
    else if(command == "cd"){
      cdCommand(args[1]);
    }
    else{
      std::string path = findExecutable(command);

      if(path.empty())
        std::cout << command << ": not found" << std::endl;
      else{
        executeProgram(path, args);
      }
    }
    
  }
}
