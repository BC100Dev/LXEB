#ifndef DEVTOOLS_URLPARAMS_HPP
#define DEVTOOLS_URLPARAMS_HPP

#include <string>
#include <optional>
#include <vector>

#define NO_URL_PARAMETER ""

namespace DevTools {

    struct URLParam {
        std::string key;
        std::optional<std::string> value;
    };

    std::string EncodeURLParams(const std::vector<URLParam>& params, const std::string& url, bool withQuestionMark);

    std::string EncodeURLParams(const std::vector<URLParam>& params, const std::string& url);

    std::string EncodeURLParams(const std::vector<URLParam>& params);

    std::vector<URLParam> DecodeURLParams(const std::string& url);

}

#endif //DEVTOOLS_URLPARAMS_HPP