#include <dev_tools/commons/AndroidEnv.hpp>

#ifdef __ANDROID__

#include <cstring>
#include <cstdlib>

fs::path get_env_value(const std::string& env, const std::string& defVal) {
    const char* v = std::getenv(env.c_str());
    if (v)
        return std::string(v);

    return defVal;
}

namespace AppAndroidEnv {

    fs::path ExternalStorage() {
        return get_env_value("EXTERNAL_STORAGE", "/sdcard");
    }

    fs::path AndroidStorage() {
        return get_env_value("ANDROID_STORAGE", "/storage");
    }

    fs::path AndroidRoot() {
        return get_env_value("ANDROID_ROOT", "/system");
    }

    fs::path AndroidData() {
        return get_env_value("ANDROID_DATA", "/data");
    }

    fs::path AndroidExpand() {
        return get_env_value("ANDROID_EXPAND", "/mnt/expand");
    }

    fs::path DownloadCache() {
        return get_env_value("DOWNLOAD_CACHE", "/cache");
    }

    fs::path OemRoot() {
        return get_env_value("OEM_ROOT", "/oem");
    }

    fs::path OdmRoot() {
        return get_env_value("ODM_ROOT", "/odm");
    }

    fs::path VendorRoot() {
        return get_env_value("VENDOR_ROOT", "/vendor");
    }

    fs::path ProductRoot() {
        return get_env_value("PRODUCT_ROOT", "/product");
    }

    fs::path SystemExtRoot() {
        return get_env_value("SYSTEM_EXT_ROOT", "/system_ext");
    }

    fs::path ApexRoot() {
        return get_env_value("APEX_ROOT", "/apex");
    }

}

#endif