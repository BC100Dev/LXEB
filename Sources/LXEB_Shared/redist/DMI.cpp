#include <dev_tools/commons/DMI.hpp>
#include <dev_tools/commons/Utils.hpp>

#include <iostream>
#include <any>
#include <format>
#include <map>
#include <signal.h>
#include <string>
#include <variant>
#include <vector>

#ifdef __linux__
#include <filesystem>
#include <fstream>
#endif

#ifdef _WIN32

#include <comdef.h>
#include <wbemidl.h>
#include <windows.h>
#include <cstdint>
#include <fstream>

#endif

namespace fs = std::filesystem;

namespace DevTools {
#ifdef _WIN32
    BSTR ConvertToBSTR(const std::string& input) {
        int wslen = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), NULL, 0);
        BSTR bstr = SysAllocStringLen(NULL, wslen);
        MultiByteToWideChar(CP_UTF8, 0, input.c_str(), (int)input.length(), bstr, wslen);
        return bstr;
    }

    std::string BSTRToString(BSTR bstr) {
        if (!bstr) return "";
        int len = WideCharToMultiByte(CP_UTF8, 0, bstr, -1, NULL, 0, NULL, NULL);
        std::string result(len - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, bstr, -1, result.data(), len, NULL, NULL);
        return result;
    }

    std::any VariantToAny(const VARIANT& v) {
        VARTYPE baseType = v.vt & ~VT_ARRAY;
        bool isArray = (v.vt & VT_ARRAY) != 0;

        if (isArray) {
            SAFEARRAY* sa = v.parray;

            if (baseType == VT_BSTR) {
                std::vector<std::string> result;
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound(sa, 1, &lBound);
                SafeArrayGetUBound(sa, 1, &uBound);
                for (LONG i = lBound; i <= uBound; ++i) {
                    BSTR elem = nullptr;
                    SafeArrayGetElement(sa, &i, &elem);
                    result.push_back(BSTRToString(elem));
                    SysFreeString(elem);
                }
                return result;
            }

            if (baseType == VT_UI2) {
                std::vector<uint16_t> result;
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound(sa, 1, &lBound);
                SafeArrayGetUBound(sa, 1, &uBound);
                for (LONG i = lBound; i <= uBound; ++i) {
                    USHORT elem = 0;
                    SafeArrayGetElement(sa, &i, &elem);
                    result.push_back(elem);
                }
                return result;
            }

            if (baseType == VT_UI4) {
                std::vector<uint32_t> result;
                LONG lBound = 0, uBound = 0;
                SafeArrayGetLBound(sa, 1, &lBound);
                SafeArrayGetUBound(sa, 1, &uBound);
                for (LONG i = lBound; i <= uBound; ++i) {
                    ULONG elem = 0;
                    SafeArrayGetElement(sa, &i, &elem);
                    result.push_back(elem);
                }
                return result;
            }

            // Fallback: unhandled array type
            return std::string("(unsupported array type)");
        }

        switch (v.vt) {
        case VT_NULL:
        case VT_EMPTY:
            return std::monostate{};

        case VT_BOOL:
            return v.boolVal != VARIANT_FALSE;

        case VT_I1:
            return static_cast<int8_t>(v.cVal);
        case VT_I2:
            return static_cast<int16_t>(v.iVal);
        case VT_I4:
            return static_cast<int32_t>(v.lVal);
        case VT_I8:
            return static_cast<int64_t>(v.llVal);

        case VT_UI1:
            return static_cast<uint8_t>(v.bVal);
        case VT_UI2:
            return static_cast<uint16_t>(v.uiVal);
        case VT_UI4:
            return static_cast<uint32_t>(v.ulVal);
        case VT_UI8:
            return static_cast<uint64_t>(v.ullVal);

        case VT_R4:
            return v.fltVal;
        case VT_R8:
            return v.dblVal;

        case VT_DATE:
            // VT_DATE is a double (OLE Automation date). Store as double or
            // convert with VariantTimeToSystemTime if you want a SYSTEMTIME.
            return v.date;

        case VT_BSTR:
            return BSTRToString(v.bstrVal);

        default:
            return std::string("(unsupported type: vt=" + std::to_string(v.vt) + ")");
        }
    }

    DeviceMap ExecuteWQL(const std::string& wql) {
        DeviceMap result;

        HRESULT hres = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hres))
            throw DeviceReadError(std::format("GetDeviceInformation#CoInitializeEx failed, 0x{:08X}", hres));

        hres = CoInitializeSecurity(NULL, -1, NULL, NULL,
                                    RPC_C_AUTHN_LEVEL_DEFAULT,
                                    RPC_C_IMP_LEVEL_IMPERSONATE,
                                    NULL, EOAC_NONE, NULL);
        if (FAILED(hres)) {
            CoUninitialize();
            throw DeviceReadError(std::format("GetDeviceInformation#CoInitializeSecurity failed, 0x{:08X}", hres));
        }

        IWbemLocator* pLoc = NULL;
        hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                                IID_IWbemLocator, (LPVOID*)&pLoc);
        if (FAILED(hres)) {
            CoUninitialize();
            throw DeviceReadError(std::format("GetDeviceInformation#CoCreateInstance failed, 0x{:08X}", hres));
        }

        IWbemServices* pSvc = NULL;
        hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, 0, 0, 0, &pSvc);
        if (FAILED(hres)) {
            pLoc->Release();
            CoUninitialize();
            throw DeviceReadError(std::format("GetDeviceInformation#pLoc->ConnectServer failed, 0x{:08X}", hres));
        }

        hres = CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                                 RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
                                 NULL, EOAC_NONE);
        if (FAILED(hres)) {
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            throw DeviceReadError(std::format("GetDeviceInformation#CoSetProxyBlanket failed, 0x{:08X}", hres));
        }

        IEnumWbemClassObject* pEnumerator = NULL;
        hres = pSvc->ExecQuery(ConvertToBSTR("WQL"), ConvertToBSTR(wql),
                               WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
        if (FAILED(hres)) {
            pSvc->Release();
            pLoc->Release();
            CoUninitialize();
            throw DeviceReadError(std::format("GetDeviceInformation#pSvc->ExecQuery failed, 0x{:08X}", hres));
        }

        IWbemClassObject* pClsObj = NULL;
        ULONG uRet = 0;

        while (pEnumerator) {
            HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pClsObj, &uRet);
            if (uRet == 0)
                break;

            pClsObj->BeginEnumeration(WBEM_FLAG_NONSYSTEM_ONLY);
            BSTR bstrName = NULL;
            VARIANT vtProp;
            VariantInit(&vtProp);

            while (pClsObj->Next(0, &bstrName, &vtProp, 0, 0) == WBEM_S_NO_ERROR) {
                std::string key = BSTRToString(bstrName);
                result[key] = VariantToAny(vtProp);

                VariantClear(&vtProp);
                SysFreeString(bstrName);
                bstrName = NULL;
            }

            pClsObj->EndEnumeration();
            pClsObj->Release();
        }

        pSvc->Release();
        pLoc->Release();
        pEnumerator->Release();
        CoUninitialize();

        return result;
    }
#endif

    DeviceMap GetDeviceInformation() {
        DeviceMap result;

#ifdef __linux__
        std::string dmiPath = "/sys/class/dmi/id";
        if (!fs::exists(dmiPath))
            throw DeviceReadError("/sys/class/dmi/id is not loaded");

        for (const auto& it : fs::directory_iterator(dmiPath)) {
            std::string fn = it.path().filename();
            std::ifstream fi(it.path());
            if (!fi.is_open())
                continue;

            std::string fc;
            std::getline(fi, fc);
            result[fn] = fc.empty() ? "(none)" : fc;
        }
#endif

#ifdef _WIN32
        result = ExecuteWQL("Select * FROM Win32_BIOS");
#endif

        return result;
    }

    std::optional<WineInfo> GetWineInfo() {
        std::optional<WineInfo> result = std::nullopt;

#ifdef _WIN32
        WineInfo info{};

        HMODULE hMod = GetModuleHandleA("ntdll.dll");
        if (!hMod)
            return result;

        if (GetProcAddress(hMod, "wine_get_version")) {
            info.winePresent = true;

            typedef void (CDECL
            *wine_get_host_version_t
            )
            (const char**
            sysname, const char * *release
            )
            ;
            if (auto hostVer = reinterpret_cast<wine_get_host_version_t>(
                GetProcAddress(hMod, "wine_get_host_version"))) {
                const char* sysName = nullptr;
                const char* release = nullptr;

                hostVer(&sysName, &release);

                std::string sys(sysName);
                if (sys == "Linux")
                    info.platform = WineInfo::HostPlatform::LINUX;
                else if (sys == "Darwin")
                    info.platform = WineInfo::HostPlatform::MAC_OS;
                else
                    info.platform = WineInfo::HostPlatform::OTHER;

                info.version = std::string(release);
            }
        }

        result = info;
#endif

        return result;
    }

    std::string GetWineRootDevice() {
        std::string realRoot;

        for (const auto& drive : GetMountpoints()) {
            if (std::string driveStr = drive.mountPath.string() + "\\";
                fs::exists(fs::path(driveStr) / "proc" / "self" / "status") &&
                fs::exists(fs::path(driveStr) / "proc" / "mounts")) {
                std::ifstream ifs(drive.mountPath.string() + R"(\proc\self\status)");
                if (!ifs.is_open())
                    continue;

                std::string line;
                while (std::getline(ifs, line)) {
                    if (line.starts_with("Pid:")) {
                        realRoot = drive.mountPath.string();
                        break;
                    }
                }

                ifs.close();
            }

            if (!realRoot.empty())
                break;
        }

        return realRoot;
    }

    std::vector<Mountpoint> GetMountpoints() {
        std::vector<Mountpoint> result;

#ifdef _WIN32
        DWORD driveMask = GetLogicalDrives();
        if (driveMask == 0)
            throw DeviceReadError("GetLogicalDrives failed: " + ConstructErrorMessage());

        for (int i = 0; i < 26; i++) {
            if (!(driveMask & (1 << i)))
                continue;

            std::string drive = std::string(1, static_cast<char>('A' + i)) + ":";

            Mountpoint mp;
            mp.mountPath = drive;

            switch (GetDriveTypeA(drive.c_str())) {
            case DRIVE_CDROM:
                mp.mountType = Mountpoint::Type::CDROM;
                break;
            case DRIVE_REMOVABLE: {
                mp.mountType = Mountpoint::Type::USB;

                char fsName[32] = {};
                if (GetVolumeInformationA(drive.c_str(), nullptr, 0,
                                          nullptr, nullptr,
                                          nullptr, fsName,
                                          sizeof(fsName))) {
                    if (std::string fs(fsName); fs == "CDFS" || fs == "UDF")
                        mp.mountType = Mountpoint::Type::CDROM;
                }
                break;
            }
            case DRIVE_FIXED:
            case DRIVE_REMOTE:
            default:
                mp.mountType = Mountpoint::Type::DISK;
                break;
            }

            result.push_back(std::move(mp));
        }
#else
        auto StripPartitionSuffix = [](const std::string& devName) -> std::string {
            std::string out = devName;

            while (!out.empty() && std::isdigit(out.back()))
                out.pop_back();

            if (out == devName)
                return devName;

            if (fs::exists("/sys/block/" + out))
                return out;

            if (!out.empty() && out.back() == 'p') {
                if (std::string candidate = out.substr(0, out.size() - 1); fs::exists("/sys/block/" + candidate))
                    return candidate;
            }

            return out;
        };

        std::ifstream mounts("/proc/mounts");
        if (!mounts.is_open())
            throw DeviceReadError("Unable to open /proc/mounts: " + ConstructErrorMessage());

        std::string line;
        while (std::getline(mounts, line)) {
            line = TrimString(line);
            if (line.empty() || line.starts_with("#"))
                continue;

            std::istringstream ss(line);
            std::string device, path, fsType, options;
            ss >> device >> path >> fsType >> options;

            if (device.empty() || path.empty() || fsType.empty())
                continue;

            Mountpoint mp;
            mp.mountPath = fs::path(path);

            std::string blockDev;
            if (device.starts_with("/dev/"))
                blockDev = StripPartitionSuffix(device.substr(5));

            fs::path sysBlockPath = "/sys/block/" + blockDev;
            if (blockDev.empty() || !fs::exists(sysBlockPath)) {
                mp.mountType = Mountpoint::Type::VIRTUAL;
                result.push_back(std::move(mp));
            }

            mp.mountType = Mountpoint::Type::DISK;
            mp.systemMountpoint = false;

            fs::path scsiTypePath = sysBlockPath / "device/type";
            if (fs::exists(scsiTypePath)) {
                std::ifstream scsiType(scsiTypePath);
                if (!scsiType.is_open())
                    throw DeviceReadError(std::format("Unable to determine block device type for {} ({})", blockDev,
                                                      ConstructErrorMessage()));

                int scsiTypeVal = -1;
                if (scsiType >> scsiTypeVal && scsiTypeVal == 5) {
                    mp.mountType = Mountpoint::Type::CDROM;
                    result.push_back(std::move(mp));
                    continue;
                }
            }

            fs::path removablePath = sysBlockPath / "removable";
            if (fs::exists(removablePath)) {
                std::ifstream removableFd(removablePath);
                if (!removableFd.is_open())
                    throw DeviceReadError(std::format("Unable to determine the removable state for blockdev {} ({})",
                                                      blockDev, ConstructErrorMessage()));

                int removable = 0;
                if (removableFd >> removable && removable == 1) {
                    mp.mountType = Mountpoint::Type::USB;
                    result.push_back(std::move(mp));
                    continue;
                }
            }

            result.push_back(std::move(mp));
        }

        mounts.close();
#endif

        return result;
    }
}
