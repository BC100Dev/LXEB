#include <iostream>
#include <vector>
#include <string>
#include <cerrno>
#include <filesystem>

#include "dev_tools/commons/Utils.hpp"

#ifdef __linux__
#include <dlfcn.h>
#endif

namespace fs = std::filesystem;

std::string find_library(const std::string& lib) {
#ifdef __linux__
    // 1. look for libraries within the cwd (current working directory) and the executables directory
    // 2. look for libraries in the "LD_LIBRARY_PATH" environment
    // 3. if still not found, initiate a dummy handle to figure out, where
    if (std::string cwd = DevTools::CurrentWorkingDirectory(); fs::exists(fs::path(cwd + "/" + lib)))
        return fs::absolute(cwd + "/" + lib);

    if (const char* ldEnv = getenv("LD_LIBRARY_PATH")) {
        if (std::string sLdEnv = ldEnv; sLdEnv.find(':') == std::string::npos) {
            if (fs::exists(sLdEnv + "/" + lib))
                return fs::absolute(sLdEnv + "/" + lib);
        }
        else {
            std::string ldEntry;

            while (std::getline(std::istringstream(sLdEnv), ldEntry, ':')) {
                if (std::string fileEntry = ldEntry / fs::path(lib); fs::exists(fileEntry))
                    return fs::absolute(fileEntry);
            }
        }
    }

    // check, whether there is LC_ALL, and if so, temporarily override it
    char* lc_all = getenv("LC_ALL");
    if (lc_all) {
        setenv("LC_ALL", "C", 1);
        dlerror(); // reset any possible errors
    }

    // initiate a dummy handle
    void* dummy = dlopen(lib.c_str(), RTLD_LAZY);
    std::string finalized = dummy ? "__system__" : "";
    if (!finalized.empty())
        dlclose(dummy);

    if (lc_all)
        setenv("LC_ALL", lc_all, 1);

    return finalized;
#else
    return "";
#endif
}

int main(int argc, char** argv, char** envp) {
    if (argc < 2)
        return 1;

    std::string libSymArg = argv[1];
    size_t sepPos = libSymArg.rfind(',');

    std::string libPath;
    std::string symbol;

    if (sepPos == std::string::npos) {
        std::cerr << "Error in " << argv[0] << std::endl;
        std::cerr << "Missing entry point" << std::endl;
    } else {
        libPath = libSymArg.substr(0, sepPos);
        symbol = libSymArg.substr(sepPos + 1);
    }

    std::string absLibPath = find_library(libPath);
    if (absLibPath.empty()) {
        std::cerr << "Error: could not find " << libPath << std::endl;
        return 1;
    }

#ifdef __linux__
    void* handle = dlopen(absLibPath == "__system__" ? libPath.c_str() : absLibPath.c_str(), RTLD_LAZY);
    if (!handle) {
        std::cerr << "Error: could not load " << libPath << " (" << dlerror() << ")" << std::endl;
        return 1;
    }

    using func_envp_t = int(*)(int, char**, char**);
    if (auto f_envp = reinterpret_cast<func_envp_t>(dlsym(handle, symbol.c_str()))) {
        int ret = f_envp(argc - 2, argv + 2, envp);
        dlclose(handle);
        return ret;
    }

    using func_argv_t = int(*)(int, char**);
    if (auto f_argv = reinterpret_cast<func_argv_t>(dlsym(handle, symbol.c_str()))) {
        int ret = f_argv(argc - 2, argv + 2);
        dlclose(handle);
        return ret;
    }

    using func_argc_t = int(*)(int);
    if (auto f_argc = reinterpret_cast<func_argc_t>(dlsym(handle, symbol.c_str()))) {
        int ret = f_argc(argc - 2);
        dlclose(handle);
        return ret;
    }

    using func_void_t = int(*)();
    if (auto f_void = reinterpret_cast<func_void_t>(dlsym(handle, symbol.c_str()))) {
        int ret = f_void();
        dlclose(handle);
        return ret;
    }

    std::cerr << "Error (" << libPath << "): " << symbol << " not found or mismatched signature" << std::endl;
    dlclose(handle);
    return 1;
#endif // __linux__

    return 0;
}
