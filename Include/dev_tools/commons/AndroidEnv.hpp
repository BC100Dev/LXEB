#ifndef DEVTOOLS_ANDROIDENV_HPP
#define DEVTOOLS_ANDROIDENV_HPP

// sorry, this is literally Android only. Oh, wait, I'm not sorry because why should I be sorry?

#ifdef __ANDROID__

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace AppAndroidEnv {

    /**
     * @deprecated Use <code>AndroidStorage()</code> call
     * @return an empty string, if environment does not exist
     */
    fs::path ExternalStorage();

    fs::path AndroidStorage();

    fs::path AndroidRoot();

    fs::path AndroidData();

    fs::path AndroidExpand();

    fs::path DownloadCache();

    fs::path OemRoot();

    fs::path OdmRoot();

    fs::path VendorRoot();

    fs::path ProductRoot();

    fs::path SystemExtRoot();

    fs::path ApexRoot();

}

#endif

#endif //DEVTOOLS_ANDROIDENV_HPP
