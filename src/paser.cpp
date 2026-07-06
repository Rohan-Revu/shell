#include "parser.h"

#include <cctype>
#include <string>
#include <vector>

std::vector<std::string> parseArguments(const std::string &input) {
    std::vector<std::string> args;
    std::string current;

    bool inSingleQuote = false;
    bool inDoubleQuote = false;

    auto pushCurrent = [&]() {
        if (!current.empty()) {
            args.push_back(current);
            current.clear();
        }
    };

    for (size_t i = 0; i < input.size(); i++) {
        char c = input[i];

        // Backslash
        if (c == '\\' && !inSingleQuote) {
            if (i + 1 < input.size()) {
                current += input[++i];
            }
            continue;
        }

        // Single quotes
        if (c == '\'' && !inDoubleQuote) {
            inSingleQuote = !inSingleQuote;
            continue;
        }

        // Double quotes
        if (c == '"' && !inSingleQuote) {
            inDoubleQuote = !inDoubleQuote;
            continue;
        }

        // Redirection operators (only outside quotes)
        if (!inSingleQuote && !inDoubleQuote) {

            // 1>>
            if (i + 2 < input.size() &&
                input.substr(i, 3) == "1>>") {
                pushCurrent();
                args.push_back("1>>");
                i += 2;
                continue;
            }

            // 2>>
            if (i + 2 < input.size() &&
                input.substr(i, 3) == "2>>") {
                pushCurrent();
                args.push_back("2>>");
                i += 2;
                continue;
            }

            // >>
            if (i + 1 < input.size() &&
                input.substr(i, 2) == ">>") {
                pushCurrent();
                args.push_back(">>");
                i++;
                continue;
            }

            // 1>
            if (i + 1 < input.size() &&
                input.substr(i, 2) == "1>") {
                pushCurrent();
                args.push_back("1>");
                i++;
                continue;
            }

            // 2>
            if (i + 1 < input.size() &&
                input.substr(i, 2) == "2>") {
                pushCurrent();
                args.push_back("2>");
                i++;
                continue;
            }

            // >
            if (c == '>') {
                pushCurrent();
                args.push_back(">");
                continue;
            }

            // Whitespace
            if (isspace(static_cast<unsigned char>(c))) {
                pushCurrent();
                continue;
            }
        }

        current += c;
    }

    pushCurrent();

    return args;
}

ParsedCommand parseCommand(const std::string& input)
{
    ParsedCommand cmd;

    cmd.args = parseArguments(input);

    if (!cmd.args.empty() && cmd.args.back() == "&")
    {
        cmd.background = true;
        cmd.args.pop_back();
    }

    return cmd;
}