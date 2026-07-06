#pragma once

#include <string>
#include <vector>




void registerCompleter(const std::string& command, const std::string& completer);

std::string getCompleter(const std::string& command);

std::vector<std::string> runCompleter(const std::string& command, const std::string& currentWord, const std::string& previousWord, const std::string& commandLines);

void unregisterCompleter(const std::string& command);