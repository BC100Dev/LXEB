#ifndef SEB_ISYSTEMINFO_HPP
#define SEB_ISYSTEMINFO_HPP

#include <string>
#include <vector>

namespace SEB::System {

    class ISystemInfo {
    public:
        virtual ~ISystemInfo() = default;

        virtual std::string GetBiosInfo() const = 0;
        virtual std::string GetCpuName() const = 0;
        virtual bool HasBattery() const = 0;
        virtual std::string GetMacAddress() const = 0;
        virtual std::string GetManufacturer() const = 0;
        virtual std::string GetModel() const = 0;
        virtual std::string GetName() const = 0;
        virtual std::string GetOperatingSystemInfo() const = 0;
        virtual std::vector<std::string> GetPlugAndPlayDeviceIds() const = 0;
    };

}

#endif //SEB_ISYSTEMINFO_HPP