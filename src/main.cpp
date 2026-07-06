#include <iostream>
#include <string>




#include "parser.h"
#include "executeProgram.h"
#include "builtins.h"
#include "redirection.h"
#include "commandExecutor.h"
#include "autoComplete.h"
#include "terminal.h"
#include "jobs.h"

int main() {

    initializeTerminal();

    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    while (true) {
        reapJobs();

        std::cout << "$ ";

        std::string input = readCommand();

        ParsedCommand command = parseCommand(input);

        Redirection redir = parseRedirection(command.args);

        setupRedirection(redir);

        if (command.args.empty()) {
            restoreRedirection(redir);
            continue;
        }

        if (executeCommand(command.args, command.background, input)) {
            restoreRedirection(redir);
            break;
        }

        restoreRedirection(redir);
    }

    restoreTerminal();
}