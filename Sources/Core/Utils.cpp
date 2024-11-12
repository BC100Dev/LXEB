#include <LXEB/Core/Utils.hpp>

namespace LXEB::Core {

    bool StartsWith(const std::string& str, const std::string& prefix) {
        if (str.size() < prefix.size())
            return false;

        return str.compare(0, prefix.size(), prefix) == 0;
    }

    bool EndsWith(const std::string& str, const std::string& suffix) {
        if (str.size() < suffix.size())
            return false;

        return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    bool Contains(const std::string& str, const std::string& ct) {
        return str.find(ct) != std::string::npos;
    }

    std::vector<std::string> SplitString(const std::string& str, const std::string& delim, int limit) {
        std::vector<std::string> tokens;
        size_t startIndex = 0;
        size_t delimPos = 0;
        int tokenIndex = 0;

        while ((delimPos = str.find(delim, startIndex)) != std::string::npos) {
            tokens.push_back(str.substr(startIndex, delimPos - startIndex));
            tokenIndex++;

            startIndex = delimPos + delim.length();

            if (limit > 0 && tokenIndex >= limit - 1)
                break;
        }

        tokens.push_back(str.substr(startIndex));

        return tokens;
    }

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim) {
        return SplitString(str, delim, 0);
    }

}