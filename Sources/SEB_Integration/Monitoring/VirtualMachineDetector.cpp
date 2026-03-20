#include <SEB/Monitoring/VirtualMachineDetector.hpp>

#include <algorithm>
#include <cctype>
#include <complex>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace SEB::Monitoring {
    const std::vector<std::string> vmMacAddresses = {
        "000000000000",
        "525400",
        "080027"
    };

    const std::vector<std::string> VirtualMachineDetector::DeviceBlacklist = {
        "prod_virtual", "hyper_v",
        "qemu", "ven_1af4", "ven_1b36", "subsys_11001af4",
        "ven_vbox", "vid_80ee",
        "prod_vmware", "ven_vmware", "vmware_ide"
    };

    const std::vector<std::string> VirtualMachineDetector::DeviceWhitelist = {
        "prod_virtual_disk",
        "prod_virtual_dvd"
    };

    VirtualMachineDetector::VirtualMachineDetector(Integrity::IIntegrityModule* integrityModule,
                                                   Logging::ILogger* logger,
                                                   System::IRegistry* registry,
                                                   System::ISystemInfo* systemInfo) :
        m_integrityModule(integrityModule), m_logger(logger)
        , m_registry(registry), m_systemInfo(systemInfo) {
    }

    bool VirtualMachineDetector::IsVirtualMachine() {
        auto isVm = false;

        isVm |= hasNoSystemHardware();
        isVm |= hasVirtualDevice();
        isVm |= hasVirtualMacAddress();
        isVm |= isVirtualCpu();
        isVm |= isVirtualSystem(m_systemInfo->GetBiosInfo(), m_systemInfo->GetManufacturer(), m_systemInfo->GetModel());

        std::string manufacturer;
        int probability = 0;
        isVm = m_integrityModule->IsVirtualMachine(manufacturer, probability);

        m_logger->Debug(std::string("Computer '") + m_systemInfo->GetName() + "' appears " +
            (isVm ? "" : "not ") + "to be a virtual machine" +
            (isVm ? " (" + manufacturer + ", " + std::to_string(probability) + "%)" : "") + ".");

        return isVm;
    }

    bool VirtualMachineDetector::hasNoSystemHardware() const {
        const std::string hwmonPath = "/sys/class/hwmon";

        try {
            if (!fs::exists(hwmonPath) || fs::is_empty(hwmonPath)) {
                m_logger->Error("No hwmon entries found - possible VM");
                return true;
            }
        }
        catch (const std::exception& e) {
            m_logger->Error("Failed to query system hardware", e);
            return false;
        }

        return false;
    }

    bool VirtualMachineDetector::hasVirtualDevice() const {
        for (const auto devices = m_systemInfo->GetPlugAndPlayDeviceIds(); const auto& device : devices) {
            std::string lower = device;
            std::ranges::transform(lower, lower.begin(), ::tolower);

            bool blacklisted = std::ranges::any_of(DeviceBlacklist,
                                                   [&](const std::string& b) {
                                                       return lower.find(b) != std::string::npos;
                                                   });

            bool whitelisted = std::ranges::any_of(DeviceWhitelist,
                                                   [&](const std::string& w) {
                                                       return lower.find(w) != std::string::npos;
                                                   });

            if (blacklisted && !whitelisted)
                return true;
        }

        return false;
    }

    bool VirtualMachineDetector::hasVirtualMacAddress() const {
        const std::string mac = m_systemInfo->GetMacAddress();

        if (mac.size() < 6)
            return false;

        std::string stripped;
        for (char c : mac)
            if (c != ':') stripped += static_cast<char>(::toupper(c));

        auto sw = [&](const std::string& prefix) {
            return stripped.rfind(prefix, 0) == 0;
        };

        for (const auto& vmMacAddress : vmMacAddresses) {
            if (sw(vmMacAddress))
                return true;
        }

        return false;
    }

    bool VirtualMachineDetector::isVirtualCpu() const {
        std::string lower = m_systemInfo->GetCpuName();
        std::ranges::transform(lower, lower.begin(), ::tolower);

        return lower.find(" kvm ") != std::string::npos;
    }

    bool VirtualMachineDetector::isVirtualSystem(const std::string& biosInfo,
                                                 const std::string& manufacturer,
                                                 const std::string& model) {
        auto tl = [](std::string s) {
            std::ranges::transform(s, s.begin(), ::tolower);
            return s;
        };

        const std::string bios = tl(biosInfo);
        const std::string mfr = tl(manufacturer);
        const std::string mdl = tl(model);

        if (bios.find("hyper-v") != std::string::npos) return true;
        if (bios.find("virtualbox") != std::string::npos) return true;
        if (bios.find("vmware") != std::string::npos) return true;
        if (bios.find("ovmf") != std::string::npos) return true;
        if (bios.find("edk ii unknown") != std::string::npos) return true;
        if (mfr.find("parallels software") != std::string::npos) return true;
        if (mfr.find("qemu") != std::string::npos) return true;
        if (mfr.find("vmware") != std::string::npos) return true;
        if (mdl.find("virtualbox") != std::string::npos) return true;
        if (mdl.find("q35 +") != std::string::npos) return true;

        if (mfr.find("microsoft corporation") != std::string::npos &&
            mdl.find("surface") == std::string::npos)
            return true;

        return false;
    }
}
