#include <format>
#include <fstream>
#include <iostream>
#include <dev_tools/commons/User.hpp>

namespace User {
    HomeDirConfig homeConf = {};

#ifdef __linux__
    bool knownElevationMethodUsed;
    uid_t realUserId = -1;
    std::string realUserName;

    struct ProcStatus {
        pid_t pid;
        pid_t ppid;
        uid_t uid;
    };

    std::optional<ProcStatus> readProcStatus(pid_t pid) {
        std::ifstream file(std::format("/proc/{}/status", pid));
        if (!file.is_open())
            return std::nullopt;

        ProcStatus status{};
        status.pid = pid;

        std::string line;
        while (std::getline(file, line)) {
            if (line.starts_with("PPid:")) {
                std::istringstream iss(line.substr(5));
                iss >> status.ppid;
            } else if (line.starts_with("Uid:")) {
                std::istringstream iss(line.substr(4));
                iss >> status.uid;
            }
        }

        return status;
    }

    std::optional<uid_t> TraverseParentProcesses(uid_t nonMatchingId) {
        pid_t current = getppid();

        while (current > 1) {
            auto status = readProcStatus(current);
            if (!status.has_value())
                break;

            if (status->uid != nonMatchingId)
                return status->uid;

            current = status->ppid;
        }

        return std::nullopt;
    }
#endif

    void PopulateUserInfo() {
#ifdef __linux__
        homeConf.resolveDoasEnv = true;
        homeConf.resolveSudoEnv = true;

        bool homePreDefined = false;
        if (const char* PREDEF_HOME = getenv("PREDEF_HOME"); PREDEF_HOME && fs::exists(PREDEF_HOME)) {
            homeConf.defHomeDir = PREDEF_HOME;
            homePreDefined = true;
        }

        // safety measure: if root, automatically resolve UIDs
        // however, if not root, let <UserHomeDirectory()> populate the real dir via the resolvers
        if (getuid() == 0) {
            if (const char* sudoUid = getenv("SUDO_UID"); sudoUid) {
                knownElevationMethodUsed = true;

                try {
                    int searchableUid = std::stoi(sudoUid);

                    if (const passwd* pw = getpwuid(searchableUid); pw) {
                        realUserId = pw->pw_uid;
                        realUserName = pw->pw_name;
                    }
                } catch (...) {
                    std::cerr << "malformed value of SUDO_UID" << std::endl;
                    _exit(1);
                }
            } else if (const char* doasUser = getenv("DOAS_USER"); doasUser) {
                knownElevationMethodUsed = true;

                if (const passwd* pw = getpwnam(doasUser); pw) {
                    realUserId = pw->pw_uid;
                    realUserName = pw->pw_name;
                }
            } else if (const char* pkexecUid = getenv("PKEXEC_UID"); pkexecUid) {
                knownElevationMethodUsed = true;

                try {
                    int searchableUid = std::stoi(pkexecUid);

                    if (const passwd* pw = getpwuid(searchableUid); pw) {
                        realUserId = pw->pw_uid;
                        realUserName = pw->pw_name;
                    }
                } catch (...) {
                    std::cerr << "malformed value of PKEXEC_UID" << std::endl;
                    _exit(1);
                }
            } else {
                // fallback: attempt to figure out via processes
                if (!homePreDefined) {
                    if (auto foundUid = TraverseParentProcesses(0); foundUid.has_value()) {
                        if (const passwd* pw = getpwuid(foundUid.value()); pw) {
                            realUserId = pw->pw_uid;
                            realUserName = pw->pw_name;
                        }
                    }
                }
            }

            if (realUserId == static_cast<uid_t>(-1)) {
                if (const passwd* pw = getpwuid(getuid()); pw) {
                    realUserName = pw->pw_name;
                    realUserId = pw->pw_uid;
                }
            } else
                homeConf.userId = realUserId;
        }
#endif
    }
}
