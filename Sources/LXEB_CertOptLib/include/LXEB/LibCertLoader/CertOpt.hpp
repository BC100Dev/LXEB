#ifndef LXEB_CERTOPT_HPP
#define LXEB_CERTOPT_HPP

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/*
This option is deliberately meant to be open-sourced.
The underlying component that ends up relying, specifically for online checks, will end up being closed-source.
It will end up requiring the underlying servers to remotely validate actual checks with the providers that end up using the certs.
The reason is to establish trust across the end-user and the ones that develop a native port for LXEB.
Not only that, but providers will end up paying for a certificate via a B2B way.
Payment can only be initiated, when negotiated for a price and when all necessary information about a business is obtained.

On the other side, for testing / debugging side of things, no certificates are required, as seen from this code, it is guarded via HAS_PUBLIC_BUILD.
To use the public build, use CMAKE_BUILD_TYPE to Release. Otherwise, it will automatically default to RelWithDebInfo, if one is not defined.
 */
namespace LXEB::CertOpt {
#if HAS_PUBLIC_BUILD == 1
    enum class OnlineVerificationResult {
        VERIFIED = 1,
        FAILURE_NO_CA = 0xff0a,
        FAILURE_CA_MISMATCH = 0xfc0e,
        FAILURE_CERT_EXPIRY = 0xfacc,
        FAILURE_CERT_REVOKED = 0xfad0,
        FAILURE_NETWORK_UNREACHABLE = 0xfe10,
        FAILURE_SERVER_ERROR = 0xfe11,
        FAILURE_INVALID_RESPONSE = 0xfe12,
        FAILURE_CERT_NOT_YET_VALID = 0xfa01,

        FAILURE_NOT_IMPLEMENTED = 0,
    };
#endif

    enum class OfflineVerificationResult {
        VERIFIED = 1,
        FAILURE_NO_CA = 0xff0a,
        FAILURE_CA_MISMATCH = 0xfc0e,
        FAILURE_CERT_EXPIRY = 0xfacc,
        FAILURE_CERT_REVOKED = 0xfad0,
        FAILURE_CERT_NOT_YET_VALID = 0xfa01,

        FAILURE_NOT_IMPLEMENTED = 0,

#if HAS_PUBLIC_BUILD == 0
        TESTING_BUILD = -1
#endif
    };

    enum class CertificateRequestor {
        // the most basic certificates out there.
        // they can be used by any small businesses that do not require big use of LXEB
        BASIC,

        // ended up being used by medium to big businesses
        BUSINESS,

        // ended up being used by businesses that require extensive use of LXEB
        ENTERPRISE,

        SCHOOL,
        UNIVERSITY,
        TRAINING_CENTER,
        LEARNING_INSTITUTION
    };

    struct Certificate {
        CertificateRequestor requestor;
        std::string certAuthority;
        long validityStart;
        long validityEnd;
    };

    OfflineVerificationResult OffVerifySharedObject(const fs::path& sharedObjPath);

    Certificate CertificateDetailsFromSharedObject(const fs::path& sharedObjPath);

#if HAS_PUBLIC_BUILD == 1
    bool CanVerifyOnlineCerts();

    bool HasOnlineSharedObjectCached(const fs::path& sharedObjPath);

    bool IsCachedSharedObjectExpired(const fs::path& sharedObjPath);

    OnlineVerificationResult OnVerifySharedObject(const fs::path& sharedObjPath);
#endif
}

#endif //LXEB_CERTOPT_HPP
