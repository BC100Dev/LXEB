#include <dev_tools/commons/Utils.hpp>
#include <random>
#include <thread>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <fstream>

#include <iostream>
#include <sstream>

#define PAUSE_PROMPT_DEFAULT "Press any key to continue..."

#ifdef _WIN32

#include <windows.h>
#include <conio.h>
#include <shlobj.h>
#include <combaseapi.h>

#elif __linux__

#include <termios.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <csignal>

#endif

#define CHARS_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARS_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CHARS_NUMS "0123456789"
#define CHARS_SPECIAL "@!§%&.,"

namespace fs = std::filesystem;

namespace DevTools {
    std::string ToLowercase(const std::string& str) {
        std::string result = str;
        std::ranges::transform(result, result.begin(), [](unsigned char c) {
            return std::tolower(c);
        });
        return result;
    }

    std::string ToUppercase(const std::string& str) {
        std::string result = str;
        std::ranges::transform(result, result.begin(), [](unsigned char c) {
            return std::toupper(c);
        });
        return result;
    }

    bool StringContains(const std::string& str, const std::string& val) {
#if __cplusplus >= 202302L
        return str.contains(val);
#else
        return str.find(val) != std::string::npos;
#endif
    }

    std::string TrimString(const std::string& str) {
        return TrimString(str, " \t\r\n");
    }

    std::string TrimString(const std::string& str, const std::string& chars) {
        size_t start = str.find_first_not_of(chars);
        size_t end = str.find_last_not_of(chars);
        return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    std::vector<std::string> SplitString(const std::string& str, const std::string& delim, int limit) {
        std::vector<std::string> tokens;
        size_t startIndex = 0;
        size_t delimPos = 0;
        int tokenIndex = 0;

        while ((delimPos = str.find(delim, startIndex)) != std::string::npos) {
            tokens.push_back(str.substr(startIndex, delimPos - startIndex));
            tokenIndex++;

            startIndex = delimPos + delim.length();

            if (limit > 0 && tokenIndex >= limit - 1)
                break;
        }

        tokens.push_back(str.substr(startIndex));

        return tokens;
    }

    std::vector<std::string> SplitString(const std::string& str, const std::string& delim) {
        return SplitString(str, delim, 0);
    }

    bool StartsWith(const std::string& str, const std::string& prefix) {
#if __cplusplus >= 202002L
        return str.starts_with(prefix);
#else
        if (prefix.size() > str.size())
            return false;

        return std::equal(prefix.begin(), prefix.end(), str.begin());
#endif
    }

    bool EndsWith(const std::string& str, const std::string& suffix) {
#if __cplusplus >= 202002L
        return str.ends_with(suffix);
#else
        if (suffix.size() > str.size())
            return false;

        return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
#endif
    }

    // don't forget to pull up that Regex skill without googling it! (evil laughter)
    std::string ReplaceAll(const std::string& str, const std::string& pattern, const std::string& replacement) {
        return ReplaceAll(str, std::regex(pattern), replacement);
    }

    std::string ReplaceAll(const std::string& str, const std::regex& pattern, const std::string& replacement) {
        try {
            return std::regex_replace(str, pattern, replacement);
        } catch (const std::regex_error& ex) {
            std::cerr << "Utils.hpp#ReplaceAll - Regex Error: " << ex.what() << std::endl;
            return str;
        }
    }

    std::string ReplaceFirst(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        if (size_t pos = result.find(from); pos != std::string::npos)
            result.replace(pos, from.length(), to);

        return result;
    }

    std::string Replace(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t pos = 0;
        while ((pos = result.find(from, pos)) != std::string::npos) {
            result.replace(pos, from.length(), to);
            pos += to.length();
        }

        return result;
    }

    bool IsAdmin() {
        bool result = false;
#ifdef __linux__
        result = getuid() == 0 || geteuid() == 0;
#endif

#ifdef _WIN32
        HANDLE hToken = nullptr;
        TOKEN_ELEVATION elevation;
        DWORD dwSize;

        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
            if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize))
                result = elevation.TokenIsElevated;

            CloseHandle(hToken);
            hToken = nullptr;
        }
#endif

        return result;
    }

    std::string GetHostname() {
        std::string result;

#ifdef __linux__
        char hostname[HOST_NAME_MAX];
        if (gethostname(hostname, sizeof(hostname)) == 0)
            result = std::string(hostname);
#endif

#ifdef _WIN32
        char hostname[MAX_COMPUTERNAME_LENGTH + 1];
        DWORD size = sizeof(hostname) / sizeof(hostname[0]);
        if (GetComputerNameA(hostname, &size))
            result = std::string(hostname);
#endif

        return result;
    }

    std::string UserDomain() {
        std::string result;

#ifdef __linux__
        result = GetHostname() + "/" + CurrentUsername();
#elif _WIN32
        char username[256];
        DWORD username_len = sizeof(username);

        if (GetUserNameExA(NameSamCompatible, username, &username_len))
            result = username;
        else
            result = "LOCALHOST\\UNKNOWN-DOMAIN";
#endif

        return result;
    }

    fs::path CurrentWorkingDirectory() {
        return fs::current_path();
    }

    std::string CurrentUsername() {
        std::string result;

#ifdef __linux__
        struct passwd* pw = getpwuid(getuid());
        result = pw ? std::string(pw->pw_name) : "nobody";
#endif

#ifdef _WIN32
        char username[256];
        DWORD username_len = sizeof(username) / sizeof(username[0]);

        if (GetUserNameA(username, &username_len))
            result = std::string(username);
#endif

        return result;
    }

    long RandomLong(long min, long max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long> dis(min, max);
        return dis(gen);
    }

    long long RandomLLong(long long min, long long max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<long long> dis(min, max);
        return dis(gen);
    }

    unsigned long long RandomULLong(unsigned long long min, unsigned long long max) {
        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<unsigned long long> dis(min, max);
        return dis(gen);
    }

    double RandomDouble(double min, double max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_real_distribution<double> dis(min, max);
        return dis(gen);
    }

    float RandomFloat(float min, float max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(min, max);
        return dis(gen);
    }

    int RandomInteger(int min, int max) {
        if (min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(min, max);
        return dis(gen);
    }

    std::string RandomString(int len) {
        return RandomString(len, CHARS_UPPER CHARS_LOWER CHARS_NUMS CHARS_SPECIAL);
    }

    std::string RandomString(int len, const std::string& chars) {
        std::ostringstream oss;
        for (int i = 0; i < len; i++)
            oss << chars[RandomInteger(0, chars.length())];

        return oss.str();
    }

    long RandomSeed(long min, long max) {
        std::random_device rd;
        unsigned int seed = rd();

        std::mt19937_64 gen(seed);
        std::uniform_int_distribution<long> dis(min, max);

        long rnd = dis(gen);
        rnd *= RandomInteger(10000, 55000) + RandomLong(5000, 500000) + (RandomLong(500, 1000) * RandomLong(2, 100000))
            * 99999;
        return rnd;
    }

    void threadSleep(long duration) {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    }

    std::vector<char> Pause() {
        return Pause(PAUSE_PROMPT_DEFAULT, 1);
    }

    std::vector<char> Pause(const std::string& prompt) {
        return Pause(prompt, 1);
    }

    std::vector<char> Pause(const std::string& prompt, const ssize_t& count) {
        std::cout << prompt;
        std::vector<char> chArr(count);

#ifdef _WIN32
        for (size_t i = 0; i < count; ++i)
            chArr[i] = _getch();
#elif __linux__
        termios oldT{}, newT{};
        tcgetattr(STDIN_FILENO, &oldT);
        newT = oldT;
        newT.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newT);

        for (int i = 0; i < count; i++) {
            char _c;
            if (ssize_t nr = read(STDIN_FILENO, &_c, 1); nr == -1) {
                std::cerr << "Read Error" << std::endl;
                chArr.clear();
                break;
            }

            chArr[i] = _c;
        }

        tcsetattr(STDIN_FILENO, TCSANOW, &oldT);
#endif

        return chArr;
    }

    void ExitProgram(int code) {
        std::exit(code);
    }

    void MkFile(const fs::path& path, int mode) {
        if (fs::exists(path))
            return;

        if (fs::is_directory(path))
            return;

        if (!fs::exists(path.parent_path()))
            fs::create_directories(path.parent_path());

#ifdef _WIN32
        HANDLE hFile = ::CreateFileA(path.string().c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
                                     FILE_ATTRIBUTE_NORMAL,
                                     nullptr);
        if (hFile == INVALID_HANDLE_VALUE) {
            std::string error;
            {
                std::stringstream ss;
                ss << "Could not create file, error 0x" << std::hex << GetLastError();

                error = ss.str();
            }

            throw std::runtime_error(error);
        }

        CloseHandle(hFile);
#else
        int fd = open(path.c_str(), O_CREAT, mode);
        if (fd < 0) {
            fd = open(path.c_str(), O_WRONLY | O_CREAT, mode);

            if (fd < 0)
                throw std::runtime_error("Could not create file, error " + std::to_string(errno));
        }

        close(fd);
#endif
    }

    void MkFile(const fs::path& path) {
        MkFile(path, 0755);
    }

    fs::path ExecutableFile() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileName(nullptr, buffer, MAX_PATH);
        fs::path exePath = std::string(buffer);
        return exePath;
#else
        return ResolveUnixSymlink("/proc/self/exe");
#endif
    }

    fs::path ExecutableDirectory() {
        return ExecutableFile().parent_path();
    }

    long long nanoTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    }

    fs::path UserHomeDirectory() {
        return UserHomeDirectory({});
    }

    fs::path UserHomeDirectory(const HomeDirConfig& config) {
        fs::path result;

#ifdef __linux__
        if (!config.defHomeDir.empty())
            return config.defHomeDir;

        uid_t uid = getuid();
        if (config.userId.has_value())
            uid = config.userId.value();
        else {
            if (config.resolveSudoEnv) {
                if (const char* sudoUid = std::getenv("SUDO_UID"); sudoUid)
                    uid = std::stoi(sudoUid);
            }
            if (config.resolveDoasEnv) {
                if (const char* doasUser = std::getenv("DOAS_USER"); doasUser) {
                    if (const passwd* pw = getpwnam(doasUser))
                        uid = pw->pw_uid;
                }
            }
        }

        if (passwd* pw = getpwuid(uid); pw && pw->pw_dir)
            result = pw->pw_dir;
        else
            throw std::runtime_error("Failed to get user home for UID " + std::to_string(uid));
#elif defined(_WIN32)
        PWSTR widePath = nullptr;
        if (HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &widePath); SUCCEEDED(hr)) {
            std::wstring path(widePath);
            CoTaskMemFree(widePath);
            result = fs::path(path);
        } else
            throw std::runtime_error("Failed to get user home");
#endif

        return result;
    }

    std::wstring FromStrToWideStr(const std::string& data) {
        if (data.empty())
            return L"";

        const char* origLoc = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "");

        std::mbstate_t state = {};
        const char* src = data.c_str();
        size_t size = mbsrtowcs(nullptr, &src, 0, &state) + 1;

        if (size == static_cast<size_t>(-1))
            throw std::runtime_error("ToWideString: invalid multibyte sequence");

        std::wstring result(size, L'\0');
        src = data.c_str();
        mbsrtowcs(result.data(), &src, size, &state);
        result.resize(size - 1);

        std::setlocale(LC_ALL, origLoc);
        return result;
    }

    std::string FromWideStrToStr(const std::wstring& data) {
        if (data.empty())
            return "";

        const char* origLoc = std::setlocale(LC_ALL, nullptr);
        std::setlocale(LC_ALL, "");

        std::mbstate_t state = {};
        const wchar_t* src = data.c_str();
        size_t size = wcsrtombs(nullptr, &src, 0, &state) + 1;

        if (size == static_cast<size_t>(-1))
            throw std::runtime_error("FromWStrToStr: invalid wide character sequence");

        std::string result(size, '\0');
        src = data.c_str();
        wcsrtombs(result.data(), &src, size, &state);
        result.resize(size - 1);

        std::setlocale(LC_ALL, origLoc);
        return result;
    }

    std::string RandomUUID() {
        std::random_device rd;
        std::mt19937_64 gen(rd());
        std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);

        uint32_t a = dist(gen);
        uint32_t b = dist(gen);
        uint32_t c = dist(gen);
        uint32_t d = dist(gen);

        // Set version 4 bits (0100) in the 13th hex digit
        b = (b & 0xFFFF0FFF) | 0x00004000;
        // Set variant bits (10xx) in the 17th hex digit
        c = (c & 0x3FFFFFFF) | 0x80000000;

        std::ostringstream ss;
        ss << std::hex << std::setfill('0')
            << std::setw(8) << a << '-'
            << std::setw(4) << (b >> 16) << '-'
            << std::setw(4) << (b & 0xFFFF) << '-'
            << std::setw(4) << (c >> 16) << '-'
            << std::setw(4) << (c & 0xFFFF)
            << std::setw(8) << d;

        return ss.str();
    }

    std::string ConstructErrorMessage() {
        std::stringstream ss;

#ifdef _WIN32
        ss << "0x" << std::hex << GetLastError();
#else
        ss << std::strerror(errno);
#endif

        return ss.str();
    }

    void ForceCloseThread(const THREAD_HANDLE handle) {
#ifdef _WIN32
        TerminateThread(reinterpret_cast<HANDLE>(handle), 0);
#else
        pthread_kill(handle, SIGKILL);
#endif
    }

    void ForceCloseThread(std::thread& th) {
        ForceCloseThread(th.native_handle());
    }

#ifdef __linux__
    __mode_t GetPermissionMask(const fs::path& path) {
        if (!fs::exists(path))
            throw std::runtime_error("File does not exist");

        struct stat si = {};
        if (stat(path.c_str(), &si) == -1)
            throw std::runtime_error("Failed to get file status: " + std::string(std::strerror(errno)));

        return si.st_mode;
    }

    bool CanUserWrite(__mode_t pMask) {
        return pMask & S_IWUSR;
    }

    bool CanUserRead(__mode_t pMask) {
        return pMask & S_IRUSR;
    }

    bool CanUserExecute(__mode_t pMask) {
        return pMask & S_IXUSR;
    }

    bool CanGroupsWrite(__mode_t pMask) {
        return pMask & S_IWGRP;
    }

    bool CanGroupsRead(__mode_t pMask) {
        return pMask & S_IRGRP;
    }

    bool CanGroupsExecute(__mode_t pMask) {
        return pMask & S_IXGRP;
    }

    bool CanOthersWrite(__mode_t pMask) {
        return pMask & S_IWOTH;
    }

    bool CanOthersRead(__mode_t pMask) {
        return pMask & S_IROTH;
    }

    bool CanOthersExecute(__mode_t pMask) {
        return pMask & S_IXOTH;
    }

    bool CanChangeUid(__mode_t pMask) {
        return pMask & S_ISUID;
    }
#endif

    void RequireJsonKeys(const json& data, const std::vector<std::string>& keys) {
        return RequireJsonKeys(data, keys, "");
    }

    void RequireJsonKeys(const json& data, const std::vector<std::string>& keys, const std::string& errMsgPrefix) {
        for (const auto& key : keys) {
            if (!data.contains(key)) {
                std::stringstream ss;

                if (!errMsgPrefix.empty())
                    ss << errMsgPrefix << ": ";

                ss << "required JSON key \"" << key << "\" is missing";
                throw JsonKeyMissingError(ss.str());
            }
        }
    }

    fs::path ResolveUnixSymlink(const fs::path& path) {
        std::error_code ec;
        fs::file_status status = fs::symlink_status(path, ec);
        if (ec)
            throw std::runtime_error(std::format("Failed to stat path {}: {}", path.string(), ec.message()));

        if (status.type() == fs::file_type::not_found)
            throw std::runtime_error("Path does not exist");

        if (!fs::is_symlink(path))
            throw std::runtime_error("Given path is not a symlink");

        try {
            fs::path resolved = fs::read_symlink(path);
            return resolved;
        } catch (...) {
#ifdef _WIN32
            typedef char* (CDECL *PWineGetUnixFileName)(LPCWSTR);

            HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
            if (!hKernel32)
                throw std::runtime_error("GetModuleHandleA failed for kernel32.dll");

            auto wine_get_unix_file_name = reinterpret_cast<PWineGetUnixFileName>(GetProcAddress(hKernel32,
                "wine_get_unix_file_name"));

            if (!wine_get_unix_file_name)
                return "";

            char* unixPath = wine_get_unix_file_name(FromStrToWideStr(path.string()).c_str());
            if (!unixPath)
                throw std::runtime_error("wine_get_unix_file_name resolve error");

            auto resolvedPath = fs::path(unixPath);
            LocalFree(unixPath);
            return resolvedPath;
#else
            throw std::runtime_error(std::format("Unable to resolve symlink at {}", path.string()));
#endif
        }
    }

    fs::path TempDirectory() {
        return fs::temp_directory_path();
    }
}
