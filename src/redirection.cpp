#include "redirection.h"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>

Redirection parseRedirection(std::vector<std::string>& args) {

    Redirection redir;

    for (size_t i = 0; i < args.size(); i++) {

        if ((args[i] == ">" || args[i] == "1>") && i + 1 < args.size()) {

            redir.redirectStdout = true;
            redir.appendStdout = false;
            redir.outputFile = args[i + 1];

            args.erase(args.begin() + i, args.begin() + i + 2);
            break;
        }

        else if ((args[i] == ">>" || args[i] == "1>>") && i + 1 < args.size()) {

            redir.redirectStdout = true;
            redir.appendStdout = true;
            redir.outputFile = args[i + 1];

            args.erase(args.begin() + i, args.begin() + i + 2);
            break;
        }

        else if (args[i] == "2>" && i + 1 < args.size()) {

            redir.redirectStderr = true;
            redir.appendStderr = false;
            redir.outputFile = args[i + 1];

            args.erase(args.begin() + i, args.begin() + i + 2);
            break;
        }

        else if (args[i] == "2>>" && i + 1 < args.size()) {

            redir.redirectStderr = true;
            redir.appendStderr = true;
            redir.outputFile = args[i + 1];

            args.erase(args.begin() + i, args.begin() + i + 2);
            break;
        }
    }

    return redir;
}

void setupRedirection(Redirection& redir) {

    if (redir.redirectStdout) {

        int flags = O_WRONLY | O_CREAT;

        if (redir.appendStdout)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;

        int fd = open(redir.outputFile.c_str(), flags, 0644);

        redir.savedStdout = dup(STDOUT_FILENO);
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    if (redir.redirectStderr) {

        int flags = O_WRONLY | O_CREAT;

        if (redir.appendStderr)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;

        int fd = open(redir.outputFile.c_str(), flags, 0644);

        redir.savedStderr = dup(STDERR_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
}

void restoreRedirection(Redirection& redir) {

    if (redir.redirectStdout) {

        std::cout.flush();

        dup2(redir.savedStdout, STDOUT_FILENO);
        close(redir.savedStdout);
    }

    if (redir.redirectStderr) {

        std::cerr.flush();

        dup2(redir.savedStderr, STDERR_FILENO);
        close(redir.savedStderr);
    }
}