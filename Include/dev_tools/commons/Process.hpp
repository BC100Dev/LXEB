#ifndef DEVTOOLS_PROCESS_HPP
#define DEVTOOLS_PROCESS_HPP

#include <map>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace DevTools {

    struct ProcessRequest {
        fs::path binaryPath;
        bool arg0BinPath = true;
        std::vector<std::string> args;
        std::map<std::string, std::string> env_map;

        fs::path workingDirectory;

        bool redirectAllIoToSelf = true;
        int stdoutFd;
        int stderrFd;
        int stdinFd;

        bool waitUntilDone = true;
        long timeout = -1;
    };

    struct ProcessIdentifier {
        int pid;
        int ppid;
        std::string name;
        fs::path binaryPath;
        std::string user;
    };

    int SpawnProcess(const ProcessRequest& req);

    std::vector<ProcessIdentifier> EnumerateProcesses();

}

#endif //DEVTOOLS_PROCESS_HPP
