#ifndef LXEB_LINUXINTERACTIVE_HPP
#define LXEB_LINUXINTERACTIVE_HPP

#include <string>
#include <unistd.h>
#include <vector>

namespace LXEB::LinuxApi {

    enum DeviceMountType {
        UNKNOWN = 0,
        VIRTUAL = 1,
        USB = 2,
        SSD = 3,
        SSD_M2 = 4,
        HDD = 5,
        NVME = 6,
        CD_DVD = 7,
        FLOPPY = 8
    };

    std::string ExecutablePath();
    bool SetUID_Permission(const std::string& filePath);

    std::vector<DeviceMountType> MountedDevices();
}

#endif //LXEB_LINUXINTERACTIVE_HPP
