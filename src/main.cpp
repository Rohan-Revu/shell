#include <iostream>
#include <string>




#include "parser.h"
#include "executeProgram.h"
#include "builtins.h"
#include "redirection.h"
#include "commandExecutor.h"
#include "autoComplete.h"
#include "terminal.h"


int main() {

    initializeTerminal();

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {

        std::cout << "$ ";

        std::string input = readCommand();

        auto args = parseArguments(input);

        Redirection redir = parseRedirection(args);

        setupRedirection(redir);

        if (args.empty()) {
            restoreRedirection(redir);
            continue;
        }

        if (executeCommand(args)) {
            restoreRedirection(redir);
            break;
        }

        restoreRedirection(redir);
    }

    restoreTerminal();
}