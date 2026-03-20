#ifndef LXEB_IINTEGRITYMODULE_HPP
#define LXEB_IINTEGRITYMODULE_HPP

#include <string>

namespace SEB::Integrity {

    class IIntegrityModule {
    public:
        virtual ~IIntegrityModule() = default;

        virtual void CacheSession(const std::string& configurationKey) = 0;
        virtual void ClearSession(const std::string& configurationKey) = 0;
        virtual bool IsVirtualMachine(std::string& manufacturer, int& probability) = 0;
        virtual bool TryCalculateAppSignatureKey(const std::string& connectionToken, const std::string& salt, std::string& appSignatureKey) = 0;
        virtual bool TryCalculateBrowserExamKey(const std::string& configurationKey, const std::string& salt, std::string& browserExamKey) = 0;
        virtual bool TryGenerateVerificatorCode(const std::string& payload, std::string& code) = 0;
        virtual bool TryVerifyCodeSignature(bool& isValid) = 0;
        virtual bool TryVerifySessionIntegrity(const std::string& configurationKey, bool& isValid) = 0;
    };

}

#endif //LXEB_IINTEGRITYMODULE_HPP