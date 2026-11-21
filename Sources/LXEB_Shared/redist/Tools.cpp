#include <dev_tools/commons/Tools.hpp>

std::vector<std::string> DevTools::TranslateStrToCmdline(const std::string &cmdLine) {
    if (cmdLine.empty() || cmdLine.find_first_not_of(' ') == std::string::npos)
        return {};

    constexpr int normal = 0;
    constexpr int inQuote = 1;
    constexpr int inDoubleQuote = 2;
    int state = normal;

    std::vector<std::string> result;
    std::stringstream current;
    bool lastTokenHasBeenQuoted = false;

    for (char nextTok : cmdLine) {
        switch (state) {
            case inQuote:
                if (nextTok == '\'') {
                    lastTokenHasBeenQuoted = true;
                    state = normal;
                } else
                    current << nextTok;

                break;

            case inDoubleQuote:
                if (nextTok == '"') {
                    lastTokenHasBeenQuoted = true;
                    state = normal;
                } else
                    current << nextTok;

                break;

            default:
                if (nextTok == '\'')
                    state = inQuote;
                else if (nextTok == '"')
                    state = inDoubleQuote;
                else if (nextTok == ' ') {
                    if (lastTokenHasBeenQuoted || current.tellp() > 0) {
                        result.push_back(current.str());
                        current.str("");
                        current.clear();
                    }
                } else
                    current << nextTok;

                lastTokenHasBeenQuoted = false;
                break;
        }
    }

    if (lastTokenHasBeenQuoted || current.tellp() > 0)
        result.push_back(current.str());

    if (state == inQuote || state == inDoubleQuote)
        throw std::runtime_error("TranslateStrToCmdline: unbalanced quotes (\"" + cmdLine + "\")");

    return result;
}