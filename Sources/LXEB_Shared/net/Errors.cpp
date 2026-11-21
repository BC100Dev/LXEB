#include <dev_tools/network/Errors.hpp>
#include <typeinfo>
#include <iostream>
#include <sstream>

std::any fetchErrorFromInt(const int& var) {
    // do nothing for now (except to potentially segfault our life)
    return nullptr;
}

namespace DevTools {

    std::vector<CommonError> CommonErrorList{
            {"challenge_required",
             "Instagram is suspecting that your signed-in account(s) is doing malicious things.",
             "Stopping current user session",
             "Stopping all running user sessions",
             "Stopping its corresponding user session that is being flagged",
             true,
             true, 401},
            {"block_eu_user_login_in_old_app",
             "Instagram is stopping support for earlier API versions. Consider changing the Instagram version",
             "", "", "", false, false, 400}
    };

    ErrorData handleFromString(const std::string& error) {
        // do nothing for now, except for some casting
        ErrorData errData;
        errData.strErrorData = error;

        return errData;
    }

    ErrorData handleFromJson(const nlohmann::json& error) {
        // do nothing for now, except for some casting
        ErrorData errData;
        errData.jsonErrorData = error;

        return errData;
    }

    ErrorData HandleNetworkError(const std::any& error) {
        ErrorData data;

        try {
            if (error.type() == typeid(std::string)) {
                auto errorVar = std::any_cast<std::string>(error);
                return handleFromString(errorVar);
            } else if (error.type() == typeid(nlohmann::json)) {
                auto errorVar = std::any_cast<nlohmann::json>(error);
                return handleFromJson(errorVar);
            } else if (error.type() == typeid(int)) {
                auto errorVar = fetchErrorFromInt(std::any_cast<int>(error));
                if (errorVar.type() == typeid(std::string))
                    return handleFromString(std::any_cast<std::string>(errorVar));
                else if (errorVar.type() == typeid(nlohmann::json))
                    return handleFromJson(std::any_cast<nlohmann::json>(errorVar));

                std::stringstream ss;
                ss << "unknown data type from integer, " << std::any_cast<int>(error);

                throw std::runtime_error("unknown data type from integer, " + std::to_string(std::any_cast<int>(error)));
            } else {
                throw std::runtime_error("unknown data type");
            }
        } catch (const std::bad_any_cast& ex) {
            std::cerr << "BadAnyCast(AppNetworking/Errors.cpp): " << ex.what() << std::endl;
        } catch (const std::runtime_error& ex) {
            std::cerr << "RuntimeError(AppNetworking/Errors.cpp): " << ex.what() << std::endl;
        } catch (const std::exception& ex) {
            std::cerr << "Exception(AppNetworking/Errors.cpp): " << ex.what() << std::endl;
        }

        // this will always remain empty, if failed
        return data;
    }

}