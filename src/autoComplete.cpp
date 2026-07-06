#include "autoComplete.h"

#include <algorithm>
#include <dirent.h>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>


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
        "cd",
        "complete"
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

    // Remove trailing '/' if present
    if (!prefix.empty() && prefix.back() == '/')
        prefix.pop_back();

    for (size_t i = 1; i < matches.size(); i++) {

        std::string current = matches[i];

        // Remove trailing '/' if present
        if (!current.empty() && current.back() == '/')
            current.pop_back();

        while (!current.starts_with(prefix)) {
            prefix.pop_back();

            if (prefix.empty())
                return "";
        }
    }

    return prefix;
}

std::vector<std::string> getFileCompletions(const std::string& prefix)
{
    size_t slashPos = prefix.find_last_of('/');

    std::string directory;
    std::string filePrefix;

    if (slashPos == std::string::npos) {
        directory = ".";
        filePrefix = prefix;
    }
    else {
        directory = prefix.substr(0, slashPos + 1);
        filePrefix = prefix.substr(slashPos + 1);
    }

    std::vector<std::string> matches;

    DIR* dp = opendir(directory.c_str());

    if (dp == nullptr)
        return matches;

    dirent* entry;

    while ((entry = readdir(dp)) != nullptr)
    {
        std::string file = entry->d_name;

         if (file == "." || file == "..")
            continue;

        if (file.starts_with(filePrefix)){
            std::string fullPath;

            if (directory == ".")
                fullPath = file;
            else
                fullPath = directory + file;

            struct stat st;

            if (stat(fullPath.c_str(), &st) == 0 && S_ISDIR(st.st_mode)){
                matches.push_back(fullPath + "/");
            }
            else{
                matches.push_back(fullPath);
            }
        }
    }

    closedir(dp);

    std::sort(matches.begin(), matches.end());

    return matches;
}