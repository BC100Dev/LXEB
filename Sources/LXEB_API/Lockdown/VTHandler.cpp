#include <LXEB/Lockdown/VTHandler.hpp>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <iostream>
#include <filesystem>
#include <climits>
#include <cstring>
#include <sstream>

namespace fs = std::filesystem;

namespace LXEB::Lockdown {

    std::string ConstructErrorFromErrno() {
        return std::strerror(errno);
    }

    int OpenTTY(int tty) {
        if (!fs::exists("/dev/tty" + std::to_string(tty)))
            throw VTOperationError("Unable to open \"/dev/tty" + std::to_string(tty) + "\" (File not found)");

        int fd = open(std::string("/dev/tty" + std::to_string(tty)).c_str(), O_RDWR);
        if (fd < 0)
            throw VTOperationError("Unable to open \"/dev/tty" + std::to_string(tty) + "\" (" + ConstructErrorFromErrno() + ")");

        return fd;
    }

    int IdentifyCurrentVT() {
        int fd = OpenTTY(0);

        struct vt_stat vts{};
        if (ioctl(fd, VT_GETSTATE, &vts) == -1) {
            close(fd);
            throw VTOperationError("ioctl on \"/dev/tty0\" failed (" + ConstructErrorFromErrno() + ")");
        }

        int ident = vts.v_active;
        close(fd);
        return ident;
    }

    void SwitchVTLock(const VTLockType& type) {
        if (type != VT_LOCKSWITCH && type != VT_UNLOCKSWITCH)
            throw VTOperationError("Unknown lockswitch type received");

        int fd = OpenTTY(0);

        bool success = ioctl(fd, type, 1) != -1;
        close(fd);

        if (!success)
            throw VTOperationError("Unable to perform lockswitch on \"/dev/tty0\" (" + ConstructErrorFromErrno() + ")");
    }

    void SwitchToVT(int targetVT) {
        if (!fs::exists("/dev/console"))
            throw VTOperationError("Unable to open \"/dev/console\" (File not found)");

        int fd = open("/dev/console", O_WRONLY);
        if (fd < 0)
            throw VTOperationError("Unable to open \"/dev/console\" (" + ConstructErrorFromErrno() + ")");

        if (ioctl(fd, VT_ACTIVATE, targetVT) < 0) {
            close(fd);
            throw VTOperationError("Activation switch failed (" + ConstructErrorFromErrno() + ")");
        }

        if (ioctl(fd, VT_WAITACTIVE, targetVT) < 0) {
            close(fd);
            throw VTOperationError("Activation wait switch failed (" + ConstructErrorFromErrno() + ")");
        }

        close(fd);
    }

}