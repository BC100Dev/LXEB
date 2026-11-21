#ifndef DEVTOOLS_NETWORKING_HPP
#define DEVTOOLS_NETWORKING_HPP

#include <string>
#include <vector>
#include <iostream>
#include <variant>

namespace DevTools {
    using Headers = std::vector<std::pair<const std::string, const std::string>>;
    using ByteData = std::string;
    using RawData = std::vector<unsigned char>;
    using Data = std::variant<ByteData, RawData>;

    const ByteData& ReadByteData(const Data& d);

    const RawData& ReadRawData(const Data& d);

    enum class RequestMethod {
        REQ_GET = 0,
        REQ_POST = 1,
        REQ_PATCH = 2,
        REQ_HEAD = 3,
        REQ_PUT = 4,
        REQ_DELETE = 5,
        REQ_CONNECT = 6,
        REQ_OPTIONS = 7,
        REQ_TRACE = 8
    };

    enum class HttpVersion {
        HTTP_1_0 = 1,
        HTTP_1_1 = 2,
        HTTP_2_0 = 3,
        HTTP_3_0 = 4,
    };

    struct ResponseData {
        int statusCode = 0;
        Data body;
        Headers headers;
        HttpVersion version;
        std::string errorData;
    };

    struct RequestData {
        RequestMethod method = RequestMethod::REQ_GET;
        std::string url;
        Headers headers;
        HttpVersion version = HttpVersion::HTTP_1_1;
        Data body;
        std::string ca_path;

        long connTimeoutMillis = 30000;
        long readTimeoutMillis = 15000;

        bool followRedirects = true;
        bool verifySSL = true;
    };

    ResponseData CreateRequest(const RequestData& request);

    class NetworkError : public std::runtime_error {
    public:
        explicit NetworkError(const std::string& what) : std::runtime_error(what) {
        }
    };
}

#endif //DEVTOOLS_NETWORKING_HPP
