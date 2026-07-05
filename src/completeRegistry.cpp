#include <completeRegistry.h>

#include <iostream>
#include <string>

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