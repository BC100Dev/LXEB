#ifndef DEVTOOLS_DMI_HPP
#define DEVTOOLS_DMI_HPP

#include <filesystem>
#include <map>
#include <string>
#include <any>
#include <variant>
#include <stdexcept>
#include <optional>

#include <nlohmann/json.hpp>

#ifdef _WIN32
#define MOUNTPOINT_SYSTEM_BLOCK false
#else
#define MOUNTPOINT_SYSTEM_BLOCK true
#endif

namespace fs = std::filesystem;

namespace DevTools {

    using DeviceMap = std::map<std::string, std::any>;

    class DeviceReadError : public std::runtime_error {
    public:
        explicit DeviceReadError(const std::string& m) : std::runtime_error(m) {}
    };

    struct WineInfo {
        enum class HostPlatform {
            LINUX = 0xff,
            MAC_OS = 0x1f,
            OTHER = 0x9a
        };

        bool winePresent;
        HostPlatform platform;
        std::string version;
    };

    struct Mountpoint {
        enum class Type {
            USB = 0x91c3,
            CDROM = 0x1c5a,
            DISK = 0x7d1e,
            FILE = 0x8cc1,

#ifndef _WIN32
            VIRTUAL = 0x06ac
#endif
        };

        bool systemMountpoint = MOUNTPOINT_SYSTEM_BLOCK;
        fs::path mountPath;
        Type mountType;

#ifndef _WIN32
        std::vector<std::string> mountOptions;
#endif
    };

#ifdef _WIN32
    DeviceMap ExecuteWQL(const std::string& wql);
#endif

    DeviceMap GetDeviceInformation();

    std::optional<WineInfo> GetWineInfo();

    std::string GetWineRootDevice();

    std::vector<Mountpoint> GetMountpoints();

}

NLOHMANN_JSON_NAMESPACE_BEGIN

template <>
struct adl_serializer<DevTools::DeviceMap> {
    static void to_json(json& j, const DevTools::DeviceMap& map) {
        for (const auto& [key, value] : map) {
            if (!value.has_value() || value.type() == typeid(std::monostate))
                j[key] = nullptr;
            else if (value.type() == typeid(bool))
                j[key] = std::any_cast<bool>(value);
            else if (value.type() == typeid(int8_t))
                j[key] = std::any_cast<int8_t>(value);
            else if (value.type() == typeid(int16_t))
                j[key] = std::any_cast<int16_t>(value);
            else if (value.type() == typeid(int32_t))
                j[key] = std::any_cast<int32_t>(value);
            else if (value.type() == typeid(int64_t))
                j[key] = std::any_cast<int64_t>(value);
            else if (value.type() == typeid(uint8_t))
                j[key] = std::any_cast<uint8_t>(value);
            else if (value.type() == typeid(uint16_t))
                j[key] = std::any_cast<uint16_t>(value);
            else if (value.type() == typeid(uint32_t))
                j[key] = std::any_cast<uint32_t>(value);
            else if (value.type() == typeid(uint64_t))
                j[key] = std::any_cast<uint64_t>(value);
            else if (value.type() == typeid(float))
                j[key] = std::any_cast<float>(value);
            else if (value.type() == typeid(double))
                j[key] = std::any_cast<double>(value);
            else if (value.type() == typeid(std::string))
                j[key] = std::any_cast<std::string>(value);
            else if (value.type() == typeid(std::vector<std::string>))
                j[key] = std::any_cast<std::vector<std::string>>(value);
            else if (value.type() == typeid(std::vector<uint16_t>))
                j[key] = std::any_cast<std::vector<uint16_t>>(value);
            else if (value.type() == typeid(std::vector<uint32_t>))
                j[key] = std::any_cast<std::vector<uint32_t>>(value);
            else
                j[key] = nullptr;
        }
    }
};

NLOHMANN_JSON_NAMESPACE_END

#endif //DEVTOOLS_DMI_HPP
