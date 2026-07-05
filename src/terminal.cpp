#include "terminal.h"
#include "autoComplete.h"

#include <iostream>
#include <unistd.h>
#include <termios.h>

void initializeTerminal(){
    termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);

    termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void restoreTerminal(){
    termios orig_termios;
    tcgetattr(STDIN_FILENO, &orig_termios);

    orig_termios.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

std::string readCommand(){
    std::string input;
    char c;
    bool lastWasTab = false;
    while (true) {
      if (read(STDIN_FILENO, &c, 1) <= 0)
          break;
       
        if (c == '\n'){
          std::cout << std::endl;
            break;
        }
        if (c == '\t') {

            size_t pos = input.find_last_of(' ');
            std::string prefix;

            std::vector<std::string> matches;

            if (pos == std::string::npos)
            {
                // Completing command
                prefix = input;
                matches = getCompletions(prefix);
            }
            else
            {
                
                prefix = input.substr(pos + 1);

                std::string command = input.substr(0, pos);

                if (!getCompleter(command).empty())
                    // Completing command
                    matches = runCompleter(command);
                else
                    // Completing filename
                    matches = getFileCompletions(prefix);
                }

            if (matches.empty()) {
                std::cout << '\a';
            }
            else if (matches.size() == 1) {
                std::string remain = matches[0].substr(prefix.size());

                bool isDirectory =
                    !matches[0].empty() &&
                    matches[0].back() == '/';

                if (isDirectory){
                    std::cout << remain;

                    if (pos == std::string::npos)
                        input = matches[0];
                    else
                        input = input.substr(0, pos + 1) + matches[0];
                }
                else{
                    std::cout << remain << ' ';

                    if (pos == std::string::npos)
                        input = matches[0] + ' ';
                    else
                        input = input.substr(0, pos + 1) + matches[0] + ' ';
                }
                lastWasTab = false;
            }
            else {
              std::string lcp = longestCommonPrefix(matches);
                if (lcp.size() > prefix.size()) {

                    std::string remain = lcp.substr(prefix.size());
                    std::cout << remain;

                    if (pos == std::string::npos)
                        input = lcp;
                    else
                        input = input.substr(0, pos + 1) + lcp;

                    lastWasTab = false;
            }
            else {
                if (!lastWasTab) {
                    std::cout << '\a';
                    lastWasTab = true;
                  }
                else {
                    std::cout << '\n';
                    for (const auto &m : matches)
                        std::cout << m << "  ";
                    std::cout << "\n$ " << input;
                    lastWasTab = false;
                }
            }
          }
          continue;
        }

        input += c;
        std::cout << c;
        lastWasTab = false;
    }
    return input;
}