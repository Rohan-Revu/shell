#include <iostream>
#include <unordered_map>
#include <string>



void registerCompleter(const std::string& command, const std::string& completer);

std::string getCompleter(const std::string& command);

std::vector<std::string> runCompleter(const std::string& command)
