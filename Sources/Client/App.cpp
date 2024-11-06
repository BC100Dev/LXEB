#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <climits>
#include <sstream>

bool hasSetuid() {
    struct stat fileStat{};

    char execPath[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", execPath, sizeof(execPath) - 1);
    if (len == -1) {
        std::cerr << "LXEB_Client(Error): Executable Path Link resolve failed" << std::endl;
        return false;
    }

    execPath[len] = '\0';

    if (stat(execPath, &fileStat) != 0) {
        std::cerr << "LXEB_Client(Error): Could not retrieve file information for executable" << std::endl;
        return false;
    }

    return (fileStat.st_mode & S_ISUID) == 0;
}

int main(int argc, char** argv) {
    unsigned int uid = getuid();

    if (uid != 0) {
        if (!hasSetuid()) {
            std::cerr << "LXEB_Client(Error): Requires root to operate; no SetUID permissions" << std::endl;
            return 1;
        }

        if (setuid(0)) {
            std::cerr << "LXEB_Client(Error): Requires root to operate; SetUID failed" << std::endl;
            return 1;
        }
    }

    uid = getuid();
    if (uid != 0) {
        std::cerr << "LXEB_Client(Error): SetUID successful and failed; should have 0, reported " << uid << std::endl;
        return 1;
    }

    // proceed forward with initializing everything
}