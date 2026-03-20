#ifndef SEB_VIRTUALMACHINEDETECTOR_HPP
#define SEB_VIRTUALMACHINEDETECTOR_HPP

#include <string>
#include <vector>

#include <SEB/Integrity/IIntegrityModule.hpp>
#include <SEB/Logging/ILogger.hpp>
#include <SEB/SystemComponents/IRegistry.hpp>
#include <SEB/SystemComponents/ISystemInfo.hpp>

namespace SEB::Monitoring {
    class VirtualMachineDetector {
    public:
        VirtualMachineDetector(
            Integrity::IIntegrityModule* integrityModule,
            Logging::ILogger* logger,
            System::IRegistry* registry,
            System::ISystemInfo* systemInfo);

        bool IsVirtualMachine();

    private:
        Integrity::IIntegrityModule* m_integrityModule;
        Logging::ILogger* m_logger;
        System::IRegistry* m_registry;
        System::ISystemInfo* m_systemInfo;

        static const std::vector<std::string> DeviceBlacklist;
        static const std::vector<std::string> DeviceWhitelist;

        bool hasNoSystemHardware() const;
        bool hasVirtualDevice() const;
        bool hasVirtualMacAddress() const;
        bool isVirtualCpu() const;
        static bool isVirtualSystem(const std::string& biosInfo, const std::string& manufacturer,
                                    const std::string& model);
    };
}

#endif //SEB_VIRTUALMACHINEDETECTOR_HPP
