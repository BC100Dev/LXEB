#ifndef LXEB_UTILS_HPP
#define LXEB_UTILS_HPP

#include <string>
#include <vector>

namespace LXEB::Core {

    bool StartsWith(const std::string &str, const std::string &prefix);

    bool EndsWith(const std::string &str, const std::string &suffix);

    bool Contains(const std::string &str, const std::string &ct);

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim, int limit);

    std::vector<std::string> SplitString(const std::string &str, const std::string &delim);

}

#endif //LXEB_UTILS_HPP
