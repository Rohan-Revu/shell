#pragma once

#include <string>
#include <vector>

std::string findExecutable(const std::string &command);
void executeProgram(const std::string &path, const std::vector<std::string> &args, const bool background, const std::string& commandLine);