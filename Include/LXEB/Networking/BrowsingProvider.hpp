#ifndef LXEB_BrowsingProvider_HPP
#define LXEB_BrowsingProvider_HPP

#include <filesystem>
#include <string>
#include <optional>

namespace LXEB::API {

    struct BrowsingProvider {
        std::string name;
        std::string url;
        std::filesystem::path libEntryName;
        std::string libEntrySignatureHashValueUrl;
        std::string expectedHashValue;
        int providerVersionIdentifier;
        int lxebMinVersion;
        std::optional<int> lxebMaxVersion;
    };

    enum class SignatureVerificationStatus {

        VERIFY_SUCCESS = 0xA0,
        VERIFY_HASH_MISMATCH = 0xA1,
        VERIFY_INTEGRITY_MISMATCH = 0xA2,
        VERIFY_NO_CONNECTION = 0xA3,
        VERIFY_SIGNATURE_INVALID = 0xA4,
        VERIFY_SIGNATURE_EXPIRED = 0xA5,
        VERIFY_UNSUPPORTED_ALGORITHM = 0xA6

    };

}

#endif //LXEB_BrowsingProvider_HPP
