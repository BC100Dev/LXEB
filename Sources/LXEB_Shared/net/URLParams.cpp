#include <algorithm>
#include <dev_tools/network/URLParams.hpp>

#include <sstream>
#include <iomanip>
#include <cctype>

#include "dev_tools/commons/Utils.hpp"

namespace DevTools {

    std::string url_encode_data(const std::string& data) {
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex;

        for (char c : data) {
            if (isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            else if (c == ' ') {
                escaped << '+';
            }
            else {
                escaped << std::uppercase;
                escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char>(c));
                escaped << std::nouppercase;
            }
        }

        return escaped.str();
    }

    std::string url_decode_data(const std::string& data) {
        std::ostringstream decoded;

        for (size_t i = 0; i < data.length(); ++i) {
            if (data[i] == '+')
                decoded << ' ';
            else if (data[i] == '%' && i + 2 < data.length()) {
                std::string hex = data.substr(i + 1, 2);
                int charCode = std::stoi(hex, nullptr, 16);
                decoded << static_cast<char>(charCode);
                i += 2;
            }
            else
                decoded << data[i];
        }

        return decoded.str();
    }

    std::string EncodeURLParams(const std::vector<URLParam>& params, const std::string& url, bool withQuestionMark) {
        std::stringstream ss;
        bool overridesQuestionMark = false;
        bool hasQuestionMark = url.find('?') != std::string::npos;

        if (!url.empty()) {
            ss << url;
            overridesQuestionMark = true;
        }

        bool firstParam = !hasQuestionMark;
        for (const auto& [key, value] : params) {
            if (key.empty())
                continue;

            if (!hasQuestionMark) {
                if (withQuestionMark || overridesQuestionMark) {
                    ss << "?";
                    hasQuestionMark = true;
                }
            } else if (!firstParam)
                ss << "&";

            ss << url_encode_data(key);
            if (value.has_value())
                ss << "=" << url_encode_data(value.value());

            firstParam = false;
        }

        return ss.str();
    }

    std::string EncodeURLParams(const std::vector<URLParam>& params, const std::string& url) {
        return EncodeURLParams(params, url, false);
    }

    std::string EncodeURLParams(const std::vector<URLParam>& params) {
        return EncodeURLParams(params, NO_URL_PARAMETER, false);
    }

    std::vector<URLParam> DecodeURLParams(const std::string& url) {
        std::vector<URLParam> params;
        std::string processor = url;

        if (StringContains(processor, "?"))
            processor = processor.substr(processor.find('?') + 1);

        std::vector<std::string> splits;
        if (StringContains(processor, "&")) {
            size_t start = 0;
            size_t end = processor.find('&');

            while (end != std::string::npos) {
                splits.emplace_back(processor.substr(start, end - start));
                start = end + 1;
                end = processor.find('&', start);
            }
            splits.emplace_back(processor.substr(start));
        } else
            splits.emplace_back(processor);

        for (const auto& split : splits) {
            if (size_t eqPos = split.find('='); eqPos != std::string::npos) {
                std::string key = url_decode_data(split.substr(0, eqPos));
                std::string value = url_decode_data(split.substr(eqPos + 1));
                params.push_back({key, value});
            } else
                params.push_back({split, std::nullopt});
        }

        return params;
    }

}
