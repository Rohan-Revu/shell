#pragma once

#include <string>
#include <vector>

std::vector<std::string> parseArguments(const std::string &input);

struct ParsedCommand
{
    std::vector<std::string> args;
    bool background = false;
};

ParsedCommand parseCommand(const std::string& input);