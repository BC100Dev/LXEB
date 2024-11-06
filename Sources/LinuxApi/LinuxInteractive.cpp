#include <LXEB/LinuxApi/LinuxInteractive.hpp>
#include <sys/stat.h>

#include <climits>
#include <iostream>
#include <fstream>

namespace LXEB::LinuxApi {

    std::string ExecutablePath() {
        char execPath[PATH_MAX];
        ssize_t len = readlink("/proc/self/exe", execPath, sizeof(execPath) - 1);
        if (len == -1) {
            std::cerr << "LXEB_LinuxApi(Error) # ExecutablePath() :: Read Link Error" << std::endl;
            return "";
        }

        execPath[len] = '\0';
        return execPath;
    }

    bool SetUID_Permission(const std::string& filePath) {
        struct stat fileStat{};

        if (stat(filePath.c_str(), &fileStat) != 0) {
            std::cerr << "LXEB_Client(Error): Could not retrieve file information for executable" << std::endl;
            return false;
        }

        return (fileStat.st_mode & S_ISUID) == 0;
    }

    std::vector<DeviceMountType> MountedDevices() {
        std::vector<DeviceMountType> mountedDevices;

        std::ifstream mountsFile("/proc/mounts");
        if (!mountsFile.is_open())
            return mountedDevices;

        return mountedDevices;
    }

}