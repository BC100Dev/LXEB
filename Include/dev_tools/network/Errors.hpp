#ifndef DEVTOOLS_ERRORS_HPP
#define DEVTOOLS_ERRORS_HPP

#include <string>
#include <vector>
#include <any>
#include <nlohmann/json.hpp>

#define HTTP_STATUS_OK 200
#define HTTP_STATUS_Unauthorized 403
#define HTTP_STATUS_NotFound 404

namespace DevTools {

    struct CommonError {
        std::string type;
        std::string outMessage;
        std::string outMessageCurrentSession;
        std::string outMessageAllSessions;
        std::string outMessageOtherSession;
        bool combineMessage;
        bool userAffected;
        int httpStatusCode;
    };

    struct ErrorData {
        bool curlError;
        CommonError* commonError;
        nlohmann::json jsonErrorData;
        std::string strErrorData;
    };

    enum HttpStatusCodeOutput {

        STATUS_INFORMATION = 100,

        STATUS_SUCCESS = 200,

        STATUS_REDIRECTS = 300,

        STATUS_CLIENT_FAIL = 400,

        STATUS_SERVER_FAIL = 500,

    };

    extern std::vector<CommonError> CommonErrorList;

    ErrorData HandleNetworkError(const std::any& error);

}

#endif //DEVTOOLS_ERRORS_HPP
