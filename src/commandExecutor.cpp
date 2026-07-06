#include "commandExecutor.h"

#include "builtins.h"
#include "executeProgram.h"

#include <iostream>


bool executeCommand(const std::vector<std::string>& args, bool background, const std::string& command)
{
    std::string command = args[0];

    if(command == "exit"){
        return true;
    }

    if(command == "echo"){
        echoCommand(args);
        return false;
    }

    if(command == "type"){
        typeCommand(args[1]);
        return false;
    }

    if(command == "pwd"){
        pwdCommand();
        return false;
    }

    if(command == "cd"){
        cdCommand(args[1]);
        return false;
    }

    if(command == "complete"){
        completeCommand(args);
        return false;
    }

    if(command == "jobs"){
        jobCommand();
        return false;
    }

    std::string path = findExecutable(command);

    if(path.empty()){
        std::cout << command << ": not found" << std::endl;
    }
    else{
        
        executeProgram(path, args, background, command);
    }

    return false;
}