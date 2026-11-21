#include <LXEB/LibCertLoader/CertOpt.hpp>

#include <dlfcn.h>

namespace LXEB::CertOpt {
    bool CanVerifyOnlineCerts() {
        void* hnd = dlopen("libLXEB-certloader.so", RTLD_LAZY);
        bool valid = hnd != nullptr;

        if (hnd)
            dlclose(hnd);

        return valid;
    }

    bool HasOnlineSharedObjectCached(const fs::path& sharedObjPath) {
        return false;
    }

    bool IsCachedSharedObjectExpired(const fs::path& sharedObjPath) {
        return false;
    }

    OfflineVerificationResult OffVerifySharedObject(const fs::path& sharedObjPath) {
        return OfflineVerificationResult::FAILURE_NOT_IMPLEMENTED;
    }

    OnlineVerificationResult OnVerifySharedObject(const fs::path& sharedObjPath) {
        return OnlineVerificationResult::FAILURE_NOT_IMPLEMENTED;
    }

    Certificate CertificateDetailsFromSharedObject(const fs::path& sharedObjPath) {
        return {};
    }

}