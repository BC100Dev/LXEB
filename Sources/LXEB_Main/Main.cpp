#include "compositor/LX_Compositor.hpp"

#include <QGuiApplication>
#include <iostream>
#include <string>
#include <vector>
#include <csignal>
#include <fstream>

#include <LXEB/Lockdown/VTHandler.hpp>

static int g_originalVT = -1;

static std::atomic<bool> g_mainShutDown{false};

static void signalHandler(int signal) {
    Q_UNUSED(signal)

    if (g_mainShutDown.exchange(true)) {
        std::cerr << "you done fucked up now" << std::endl;
        _exit(1);
    }

    LXEB::Lockdown::SwitchVTLock(LXEB::Lockdown::UNLOCK);
    if (g_originalVT != -1)
        LXEB::Lockdown::SwitchToVT(g_originalVT);

    QGuiApplication::quit();
}

void parse_args(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
        }
    }
}

int main(int argc, char** argv) {
    if (argc > 1)
        parse_args(argc, argv);

    std::signal(SIGTERM, signalHandler);
    std::signal(SIGINT, signalHandler);
    std::signal(SIGHUP, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGSEGV, signalHandler);
    std::set_terminate([] { signalHandler(SIGSEGV); });

    if (getuid() != 0) {
        if (setuid(0) != 0) {
            std::cerr << "setuid() failed: " << strerror(errno) << std::endl;
            std::cerr << "LXEB cannot run without root permissions." << std::endl;
            return 1;
        }
    }

    {
        std::ofstream sysrq("/proc/sys/kernel/sysrq");
        sysrq << "1";
    }

    g_originalVT = LXEB::Lockdown::IdentifyCurrentVT();

    LXEB::Lockdown::SwitchToVT(10);
    LXEB::Lockdown::SwitchVTLock(LXEB::Lockdown::VTLockType::LOCK);

    qputenv("QT_QPA_PLATFORM", "eglfs");
    qputenv("QT_WAYLAND_CLIENT_BUFFER_INTEGRATION", "linux-dmabuf-unstable-v1");

    QGuiApplication app(argc, argv);

    LXEBCompositor compositor;
    compositor.initialize();

    return QGuiApplication::exec();
}