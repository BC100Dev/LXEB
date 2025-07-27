#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
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
    if (argc == 1) {
        std::cerr << "usage: " << argv[0] << " [unlock|lock]" << std::endl;
        return 1;
    }

    int fd = open("/dev/tty0", O_RDWR);
    if (fd < 0) {
        std::cerr << "/dev/tty0 open failed, error " << errno << std::endl;
        return 1;
    }

    struct vt_stat vts{};
    if (ioctl(fd, VT_GETSTATE, &vts) == -1) {
        std::cerr << "ioctl failed, error " << errno << std::endl;
        close(fd);

        return 1;
    }

    std::cout << "Current active VT: /dev/tty" << vts.v_active << std::endl;

    std::string arg = argv[1];
    unsigned long req;
    if (arg == "lock")
        req = VT_LOCKSWITCH;
    else if (arg == "unlock")
        req = VT_UNLOCKSWITCH;
    else {
        std::cerr << "unknown action " << arg << std::endl;
        close(fd);
        return 1;
    }

    bool success = ioctl(fd, req, 1) != -1;
    std::cout << (success ? "VT switch success" : "VT switch failed") << std::endl;

    close(fd);
    return 0;
}