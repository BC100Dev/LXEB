#ifndef DEVTOOLS_I18N_HPP
#define DEVTOOLS_I18N_HPP

#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class I18n {
public:
    I18n(const std::string& lang, const std::string& ns);

    ~I18n();

    std::string GetText(const std::string& key);

    std::string GetNamespace();
private:
    std::string lang;
    std::string ns;

    json data;
};

#endif //DEVTOOLS_I18N_HPP
