#include <dev_tools/network/Network.hpp>
#include <dev_tools/commons/Utils.hpp>

#include <algorithm>
#include <iostream>
#include <cctype>
#include <chrono>

#ifdef __ANDROID__

#include <filesystem>

#endif

#include <curl/curl.h>

struct ProgressState {
    long startTime;
    long readTimeoutMillis;
};

namespace DevTools {
    std::string ReqMethodToStr(const RequestMethod& method) {
        switch (method) {
        case RequestMethod::REQ_GET:
            return "GET";
        case RequestMethod::REQ_POST:
            return "POST";
        case RequestMethod::REQ_PUT:
            return "PUT";
        case RequestMethod::REQ_DELETE:
            return "DELETE";
        case RequestMethod::REQ_PATCH:
            return "PATCH";
        case RequestMethod::REQ_HEAD:
            return "HEAD";
        case RequestMethod::REQ_CONNECT:
            return "CONNECT";
        case RequestMethod::REQ_OPTIONS:
            return "OPTIONS";
        case RequestMethod::REQ_TRACE:
            return "TRACE";
        default:
            return "EXTRA";
        }
    }

    static long nowMillis() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()
        ).count();
    }

    static size_t WCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        auto* response = static_cast<ResponseData*>(userp);
        size_t totalSize = size * nmemb;
        auto* data = static_cast<unsigned char*>(contents);

        if (std::holds_alternative<RawData>(response->body)) {
            auto& raw = std::get<RawData>(response->body);
            raw.insert(raw.end(), data, data + totalSize);
            return totalSize;
        }

        auto& bytes = std::get<ByteData>(response->body);
        for (size_t i = 0; i < totalSize; i++) {
            if (!std::isprint(data[i]) && data[i] != '\n' && data[i] != '\r' && data[i] != '\t') {
                RawData raw(bytes.begin(), bytes.end());
                raw.insert(raw.end(), data + i, data + totalSize);
                response->body = std::move(raw);
                return totalSize;
            }
            bytes += static_cast<char>(data[i]);
        }

        return totalSize;
    }

    static size_t HCallback(char* buffer, size_t size, size_t nitems, void* userp) {
        auto* headers = static_cast<Headers*>(userp);
        size_t totalSize = size * nitems;
        std::string headerLine(buffer, totalSize);

        size_t delimPos = headerLine.find(':');
        if (delimPos != std::string::npos) {
            std::string key = headerLine.substr(0, delimPos);
            std::string value = headerLine.substr(delimPos + 2, headerLine.size() - delimPos - 4);

            (*headers).emplace_back(key, value);
        }

        return totalSize;
    }

    static int PCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
        if (auto* state = static_cast<ProgressState*>(clientp);
            (nowMillis() - state->startTime) > state->readTimeoutMillis)
            return 1;

        return 0;
    }

    long MapHttpVersion(HttpVersion version) {
        switch (version) {
        case HttpVersion::HTTP_1_0:
            return CURL_HTTP_VERSION_1_0;
        case HttpVersion::HTTP_1_1:
            return CURL_HTTP_VERSION_1_1;
        case HttpVersion::HTTP_2_0:
            return CURL_HTTP_VERSION_2;
        case HttpVersion::HTTP_3_0:
            return CURL_HTTP_VERSION_3;
        default:
            return CURL_HTTP_VERSION_NONE;
        }
    }

    void CleanCurl(CURL* curl) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
    }

    ResponseData CreateRequest(const RequestData& request) {
        CURL* curl = curl_easy_init();
        ResponseData response;

        if (!curl)
            throw NetworkError("Could not initialize a Network Request Session");

        try {
            curl_easy_setopt(curl, CURLOPT_URL, request.url.c_str());

            const void* bodyPtr = nullptr;
            size_t bodySize = 0;
            if (std::holds_alternative<ByteData>(request.body)) {
                const auto& b = std::get<ByteData>(request.body);
                bodyPtr = b.data();
                bodySize = b.size();
            } else {
                const auto& b = std::get<RawData>(request.body);
                bodyPtr = b.data();
                bodySize = b.size();
            }

            switch (request.method) {
            case RequestMethod::REQ_GET:
                curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                break;
            case RequestMethod::REQ_POST:
                curl_easy_setopt(curl, CURLOPT_POST, 1L);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyPtr);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodySize);
                break;
            case RequestMethod::REQ_PUT:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyPtr);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodySize);
                break;
            case RequestMethod::REQ_DELETE:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
                break;
            case RequestMethod::REQ_PATCH:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, bodyPtr);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, bodySize);
                break;
            case RequestMethod::REQ_HEAD:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "HEAD");
                break;
            case RequestMethod::REQ_OPTIONS:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "OPTIONS");
                break;
            case RequestMethod::REQ_CONNECT:
                curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "CONNECT");
                break;
            case RequestMethod::REQ_TRACE:
                CleanCurl(curl);
                throw NetworkError(ReqMethodToStr(request.method) + " not implemented");
            }

            // CA store check (if given)
            if (!request.ca_path.empty())
                curl_easy_setopt(curl, CURLOPT_CAINFO, request.ca_path.c_str());

            // HTTP Version
            curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, MapHttpVersion(request.version));

            // Headers
            struct curl_slist* curlHeaders = nullptr;
            for (const auto& [key, value] : request.headers) {
                std::string header = key + ": " + value;
                curlHeaders = curl_slist_append(curlHeaders, header.c_str());
            }

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);

            // SSL Verification
            curl_easy_setopt(curl, CURLOPT_SSL_OPTIONS, CURLSSLOPT_NATIVE_CA);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, request.verifySSL ? 1L : 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, request.verifySSL ? 2L : 0L);

            // 3xx: Following Redirects
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, request.followRedirects ? 1L : 0L);

            // Response Data Callback
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Header Callback
            curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HCallback);
            curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);

            // Connection + Read Timeout
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, request.connTimeoutMillis);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, request.readTimeoutMillis + request.connTimeoutMillis);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

            ProgressState pState = { nowMillis(), request.readTimeoutMillis };
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, PCallback);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &pState);

            CURLcode res = curl_easy_perform(curl);
            if (res != CURLE_OK) {
                CleanCurl(curl);
                throw NetworkError("curl_easy_perform() failed, error " + std::string(curl_easy_strerror(res)));
            }

            long httpVer;
            curl_easy_getinfo(curl, CURLINFO_HTTP_VERSION, &httpVer);
            switch (httpVer) {
            case CURL_HTTP_VERSION_1_0:
                response.version = HttpVersion::HTTP_1_0;
                break;
            case CURL_HTTP_VERSION_1_1:
                response.version = HttpVersion::HTTP_1_1;
                break;
            case CURL_HTTP_VERSION_2:
                response.version = HttpVersion::HTTP_2_0;
                break;
            case CURL_HTTP_VERSION_3:
                response.version = HttpVersion::HTTP_3_0;
                break;
            default:
                response.version = HttpVersion::HTTP_1_1;
                break;
            }

            long httpStatusCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatusCode);
            response.statusCode = static_cast<int>(httpStatusCode);

            curl_slist_free_all(curlHeaders);
        } catch (const std::exception& ex) {
            CleanCurl(curl);
            throw NetworkError(ex.what());
        }

        CleanCurl(curl);

        return response;
    }

    const ByteData& ReadByteData(const Data& d) {
        if (!std::holds_alternative<ByteData>(d))
            throw std::runtime_error("ByteData is not being held via the given variant.");

        return std::get<ByteData>(d);
    }

    const RawData& ReadRawData(const Data& d) {
        if (!std::holds_alternative<RawData>(d))
            throw std::runtime_error("RawData is not being held via the given variant.");

        return std::get<RawData>(d);
    }
}
