#ifndef LXEB_Lockdown_VTHandler_HPP
#define LXEB_Lockdown_VTHandler_HPP

#include <iostream>

#include <linux/vt.h>

namespace LXEB::Lockdown {

    class VTOperationError : public std::runtime_error {
    public:
        explicit VTOperationError(const std::string &what) : std::runtime_error(what) {}
    };

    enum VTLockType {
        // value assigned to VT_LOCKSWITCH (from "linux/vt.h")
        LOCK = VT_LOCKSWITCH,

        // value assigned to VT_UNLOCKSWITCH (from "linux/vt.h")
        UNLOCK = VT_UNLOCKSWITCH
    };

    int IdentifyCurrentVT();

    void SwitchVTLock(const VTLockType& type);

    void SwitchToVT(int targetVT);

}

#endif //LXEB_Lockdown_VTHandler_HPP
