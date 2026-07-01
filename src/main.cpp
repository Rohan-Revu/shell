#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <fcntl.h>


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

    auto pushCurrent = [&]() {
        if (!current.empty()) {
            args.push_back(current);
            current.clear();
        }
    };

    for (size_t i = 0; i < input.size(); i++) {
        char c = input[i];

        // Backslash
        if (c == '\\' && !inSingleQuote) {
            if (i + 1 < input.size()) {
                current += input[++i];
            }
            continue;
        }

        // Single quotes
        if (c == '\'' && !inDoubleQuote) {
            inSingleQuote = !inSingleQuote;
            continue;
        }

        // Double quotes
        if (c == '"' && !inSingleQuote) {
            inDoubleQuote = !inDoubleQuote;
            continue;
        }

        // Redirection operators (only outside quotes)
        if (!inSingleQuote && !inDoubleQuote) {

            // 1>>
            if (i + 2 < input.size() &&
                input.substr(i, 3) == "1>>") {
                pushCurrent();
                args.push_back("1>>");
                i += 2;
                continue;
            }

            // 2>>
            if (i + 2 < input.size() &&
                input.substr(i, 3) == "2>>") {
                pushCurrent();
                args.push_back("2>>");
                i += 2;
                continue;
            }

            // >>
            if (i + 1 < input.size() &&
                input.substr(i, 2) == ">>") {
                pushCurrent();
                args.push_back(">>");
                i++;
                continue;
            }

            // 1>
            if (i + 1 < input.size() &&
                input.substr(i, 2) == "1>") {
                pushCurrent();
                args.push_back("1>");
                i++;
                continue;
            }

            // 2>
            if (i + 1 < input.size() &&
                input.substr(i, 2) == "2>") {
                pushCurrent();
                args.push_back("2>");
                i++;
                continue;
            }

            // >
            if (c == '>') {
                pushCurrent();
                args.push_back(">");
                continue;
            }

            // Whitespace
            if (isspace(static_cast<unsigned char>(c))) {
                pushCurrent();
                continue;
            }
        }

        current += c;
    }

    pushCurrent();

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
    std::string outputFile;
    bool redirectStdout = false;
    bool redirectStderr = false;
    bool appendStdout = false;

    for (size_t i = 0; i < args.size(); i++) {
      if (args[i] == ">" || args[i] == "1>") {
        
        if(i+1 < args.size()){
          redirectStdout = true;
          outputFile = args[i + 1];

          args.erase(args.begin() + i, args.begin() + i + 2);
          break;
        }
        
      }
      else if(args[i] == ">>" || args[i] == "1>>") {
        
        if(i+1 < args.size()){
          redirectStdout = true;
          appendStdout = true;
          outputFile = args[i + 1];

          args.erase(args.begin() + i, args.begin() + i + 2);
          break;
        }
        
      }
      else if(args[i] == "2>"){

        if(i+1 < args.size()){
          redirectStderr = true;
          outputFile = args[i + 1];

          args.erase(args.begin() + i, args.begin() + i + 2);
          break;
        }

      }
    }

    int savedStdout = -1;
    if (redirectStdout) {
      int flags = O_WRONLY | O_CREAT;

      if (appendStdout)
        flags |= O_APPEND;
      else
        flags |= O_TRUNC;
      
        int fd = open(outputFile.c_str(), flags, 0644);

        savedStdout = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    int savedStderr = -1;
    if (redirectStderr) {
        int flags = O_WRONLY | O_CREAT | O_TRUNC;
        int fd = open(outputFile.c_str(), flags, 0644);

        savedStderr = dup(STDERR_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }


    

    if (args.empty()) {
      continue;
    }


    std::string command = args[0];

    bool shouldExit = false;
    if(command == "exit"){
      shouldExit = true;
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
    if (redirectStdout) {
      std::cout.flush();
      dup2(savedStdout, STDOUT_FILENO);
      close(savedStdout);
    }

    if (redirectStderr) {
      std::cerr.flush();
      dup2(savedStderr, STDERR_FILENO);
      close(savedStderr);
    }

    if (shouldExit) {
      break;
    }
  }
}
