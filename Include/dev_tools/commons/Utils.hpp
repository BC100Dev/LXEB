#ifndef DEVTOOLS_UTILS_HPP
#define DEVTOOLS_UTILS_HPP

#include <string>
#include <regex>
#include <filesystem>
#include <thread>

#include <nlohmann/json.hpp>

#ifdef __linux__

#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <climits>

#define MAX_PATH_LIMIT PATH_MAX
#define THREAD_HANDLE pthread_t

#endif

#ifdef _WIN32
#define SECURITY_WIN32

#include <windows.h>
#include <secext.h>
#include <direct.h>

#define MAX_PATH_LIMIT MAX_PATH
#define THREAD_HANDLE unsigned long long
#endif

#ifdef __amd64__
#define SYSTEM_ARCHITECTURE "amd64"
#define CPU_ARCHITECTURE "x64"
#define LABEL_ARCH "x64"
#endif

#ifdef __aarch64__
#define SYSTEM_ARCHITECTURE "aarch64"
#define CPU_ARCHITECTURE "arm64"
#define LABEL_ARCH "aarch64"
#endif

#ifdef __i386__
#define SYSTEM_ARCHITECTURE "i386"
#define CPU_ARCHITECTURE "x32"
#define LABEL_ARCH "x32"
#endif

using namespace nlohmann;
namespace fs = std::filesystem;

namespace DevTools {
    class JsonKeyMissingError : public std::runtime_error {
    public:
        explicit JsonKeyMissingError(const std::string& msg) : runtime_error(msg) {
        }
    };

    struct HomeDirConfig {
#ifdef __linux__
        bool resolveSudoEnv = false;
        bool resolveDoasEnv = false;
        std::optional<uid_t> userId = std::nullopt;
        fs::path defHomeDir;
#endif
    };

    std::string ToLowercase(const std::string& str);

    std::string ToUppercase(const std::string& str);

    bool StringContains(const std::string& str, const std::string& val);

    std::string TrimString(const std::string& str);

    std::string TrimString(const std::string& str, const std::string& chars);

    std::vector<std::string> SplitString(const std::string& str, const std::string& delim, int limit);

    std::vector<std::string> SplitString(const std::string& str, const std::string& delim);

    bool StartsWith(const std::string& str, const std::string& prefix);

    bool EndsWith(const std::string& str, const std::string& suffix);

    // don't forget to pull up that Regex skill without googling it! (evil laughter)
    std::string ReplaceAll(const std::string& str, const std::string& pattern, const std::string& replacement);

    std::string ReplaceAll(const std::string& str, const std::regex& pattern, const std::string& replacement);

    std::string ReplaceFirst(const std::string& str, const std::string& from, const std::string& to);

    std::string Replace(const std::string& str, const std::string& from, const std::string& to);

    bool IsAdmin();

    std::string GetHostname();

    std::string UserDomain();

    fs::path CurrentWorkingDirectory();

    std::string CurrentUsername();

    long RandomLong(long min, long max);

    long long RandomLLong(long long min, long long max);

    unsigned long long RandomULLong(unsigned long long min, unsigned long long max);

    double RandomDouble(double min, double max);

    float RandomFloat(float min, float max);

    int RandomInteger(int min, int max);

    // this method, is what we call, a sacrificial lamp.
    // better hook this method up on a proper 64-bit CPU because we be generating high numbers.
    // I'd recommend at least sparing 512 MB for this project
    long RandomSeed(long min = 5, long max = 500);

    std::string RandomString(int len);

    std::string RandomString(int len, const std::string& chars);

    void threadSleep(long duration);

    std::vector<char> Pause(const std::string& prompt, const ssize_t& count);

    std::vector<char> Pause(const std::string& prompt);

    std::vector<char> Pause();

    void ExitProgram(int code);

    void MkFile(const fs::path& path, int mode);

    void MkFile(const fs::path& path);

    fs::path ExecutableFile();

    fs::path ExecutableDirectory();

    long long nanoTime();

    fs::path UserHomeDirectory();

    fs::path UserHomeDirectory(const HomeDirConfig& config);

    std::wstring FromStrToWideStr(const std::string& data);

    std::string FromWideStrToStr(const std::wstring& data);

    std::string RandomUUID();

    std::string ConstructErrorMessage();

    void ForceCloseThread(const THREAD_HANDLE handle);

    void ForceCloseThread(std::thread& th);

#ifdef __linux__
    __mode_t GetPermissionMask(const fs::path& path);

    bool CanUserWrite(__mode_t pMask);

    bool CanUserRead(__mode_t pMask);

    bool CanUserExecute(__mode_t pMask);

    bool CanGroupsWrite(__mode_t pMask);

    bool CanGroupsRead(__mode_t pMask);

    bool CanGroupsExecute(__mode_t pMask);

    bool CanOthersWrite(__mode_t pMask);

    bool CanOthersRead(__mode_t pMask);

    bool CanOthersExecute(__mode_t pMask);

    bool CanChangeUid(__mode_t pMask);
#endif

    void RequireJsonKeys(const json& data, const std::vector<std::string>& keys);

    void RequireJsonKeys(const json& data, const std::vector<std::string>& keys, const std::string& errMsgPrefix);

    fs::path ResolveUnixSymlink(const fs::path& path);

    fs::path TempDirectory();
}

#endif //DEVTOOLS_UTILS_HPP
