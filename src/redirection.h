#pragma once

#include <string>
#include <vector>

struct Redirection {

    bool redirectStdout = false;
    bool redirectStderr = false;

    bool appendStdout = false;
    bool appendStderr = false;

    std::string outputFile;

    int savedStdout = -1;
    int savedStderr = -1;
};

Redirection parseRedirection(std::vector<std::string>& args);

void setupRedirection(Redirection& redirection);

void restoreRedirection(Redirection& redirection);