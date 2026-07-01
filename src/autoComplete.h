#pragma once

#include <string>
#include <vector>

std::vector<std::string> findMatchingExecutables(const std::string& prefix);

std::vector<std::string> getCompletions(const std::string& prefix);

std::string longestCommonPrefix(const std::vector<std::string>& matches);

std::vector<std::string> getFileCompletions(const std::string& prefix);