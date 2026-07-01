#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>



#include "parser.h"



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




std::vector<std::string> findMatchingExecutables(const std::string &prefix) {
    std::vector<std::string> matches;

    const char* pathEnv = getenv("PATH");
    if (!pathEnv)
        return matches;

    std::stringstream ss(pathEnv);
    std::string dir;

    while (getline(ss, dir, ':')) {

        DIR* dp = opendir(dir.c_str());

        if (dp == nullptr)
            continue;

        struct dirent* entry;

        while ((entry = readdir(dp)) != nullptr) {
            std::string file = entry->d_name;

            if (!file.starts_with(prefix))
                continue;

            std::string fullPath = dir + "/" + file;

            if (access(fullPath.c_str(), X_OK) == 0) {
                matches.push_back(file);
            }
        }
        closedir(dp);
    }
    return matches;
}


std::vector<std::string> getCompletions(const std::string &prefix) {

    std::vector<std::string> matches;

    static const std::vector<std::string> builtins = {
        "echo",
        "exit",
        "type",
        "pwd",
        "cd"
    };

    // Builtins
    for (const auto &cmd : builtins) {
        if (cmd.starts_with(prefix))
            matches.push_back(cmd);
    }

    // Executables
    auto execs = findMatchingExecutables(prefix);

    matches.insert(matches.end(), execs.begin(), execs.end());
    std::sort(matches.begin(), matches.end());

    matches.erase(std::unique(matches.begin(),matches.end()), matches.end());

    return matches;
}

std::string longestCommonPrefix(const std::vector<std::string>& matches) {
    if (matches.empty())
        return "";

    std::string prefix = matches[0];

    for (size_t i = 1; i < matches.size(); i++) {

        while (!matches[i].starts_with(prefix)) {
            prefix.pop_back();

            if (prefix.empty())
                return "";
        }
    }

    return prefix;
}




int main() {
  termios orig_termios;
  tcgetattr(STDIN_FILENO, &orig_termios);

  termios raw = orig_termios;
  raw.c_lflag &= ~(ICANON | ECHO);

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;


  while(true){
    std::cout << "$ ";

    // Read line
    std::string input;
    char c;
    bool lastWasTab = false;

    while (true) {
      if (read(STDIN_FILENO, &c, 1) <= 0)
          break;
       
        if (c == '\n'){
          std::cout << std::endl;
            break;
        }
        if (c == '\t') {

            auto matches = getCompletions(input);

            if (matches.empty()) {
                std::cout << '\a';
            }
            else if (matches.size() == 1) {
                std::string remain = matches[0].substr(input.size());
                std::cout << remain << ' ';
                input = matches[0] + ' ';
                lastWasTab = false;
            }
            else {
              std::string lcp = longestCommonPrefix(matches);
              if (lcp.size() > input.size()) {
                  std::string remain = lcp.substr(input.size());
                  std::cout << remain;
                  input = lcp;
                  lastWasTab = false;
              }
              else {
                  if (!lastWasTab) {
                      std::cout << '\a';
                      lastWasTab = true;
                  }
                  else {
                      std::cout << '\n';

                      for (const auto &m : matches)
                          std::cout << m << "  ";

                      std::cout << "\n$ " << input;

                      lastWasTab = false;
                  }
              }
          }
          continue;
        }

        input += c;
        std::cout << c;
        lastWasTab = false;
    }
    std::vector<std::string> args = parseArguments(input);
    std::string outputFile;
    bool redirectStdout = false;
    bool redirectStderr = false;
    bool appendStdout = false;
    bool appendStderr = false;

    for (size_t i = 0; i < args.size(); i++) {
      if (args[i] == ">" || args[i] == "1>") {
        
        if(i+1 < args.size()){
          redirectStdout = true;
          appendStdout = false;
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
          appendStderr = false;
          outputFile = args[i + 1];

          args.erase(args.begin() + i, args.begin() + i + 2);
          break;
        }

      }
      else if(args[i] == "2>>"){
        if(i+1 < args.size()){
          redirectStderr = true;
          appendStderr = true;
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
        int flags = O_WRONLY | O_CREAT;

        if (appendStderr) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
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
