#ifndef DEVTOOLS_PROXYCHAINS_HPP
#define DEVTOOLS_PROXYCHAINS_HPP

#include "Proxy.hpp"

#include <list>
#include <string>

namespace DevTools {

    enum PChainPriority {
        HIGHEST = 10,
        HIGHER = 7,
        HIGH = 3,
        NORMAL = 0,
        LOW = -3,
        LOWER = -7,
        LOWEST = -10
    };

    struct PChainsInfo {
        ProxyInfo pInfo;
        PChainPriority priority = NORMAL;
    };

    struct ChainsSettings {
        std::list<PChainsInfo> pInfoList;
    };

}

#endif //DEVTOOLS_PROXYCHAINS_HPP
