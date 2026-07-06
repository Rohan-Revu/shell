#pragma once

#include <string>
#include <vector>

void echoCommand(const std::vector<std::string> &args);

void pwdCommand();

void cdCommand(std::string directory);

void typeCommand(const std::string &cmd);

void completeCommand(const std::vector<std::string> &args);

void jobCommand();