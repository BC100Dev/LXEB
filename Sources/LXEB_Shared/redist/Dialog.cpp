#include <dev_tools/commons/Dialog.hpp>
#include <iostream>
#include <format>
#include <thread>
#include <csignal>

#ifdef __linux__
#include <dlfcn.h>
#include <unistd.h>

#if __has_include(<ncurses.h>)
#include <ncurses.h>
#undef wattron
#undef wattroff
#undef wgetch
#undef wrefresh
#undef box
#undef keypad
#undef mvwprintw
#undef wgetnstr
#undef wmove
#undef wclrtoeol
#undef delwin
#undef endwin
#else
#define KEY_UP    0403
#define KEY_DOWN  0402
#define A_REVERSE 0x00040000
#endif
#endif

#include <dev_tools/commons/Utils.hpp>
#include <dev_tools/commons/Terminal.hpp>

#define MIN_TERMINAL_WIDTH_SIZE 30
#define MIN_TERMINAL_HEIGHT_SIZE 10

namespace DevTools {
    std::thread standardTerminalThread;

#ifdef __linux__
    struct NCursesSymbols {
        void* handle = nullptr;

        void* (*initscr)() = nullptr;
        int (*endwin)() = nullptr;
        void* (*newwin)(int, int, int, int) = nullptr;
        int (*delwin)(void*) = nullptr;
        int (*wrefresh)(void*) = nullptr;
        int (*box)(void*, unsigned int, unsigned int) = nullptr;
        int (*mvwprintw)(void*, int, int, const char*, ...) = nullptr;
        int (*wgetnstr)(void*, char*, int) = nullptr;
        int (*wgetch)(void*) = nullptr;
        int (*keypad)(void*, bool) = nullptr;
        int (*cbreak)() = nullptr;
        int (*noecho)() = nullptr;
        int (*wmove)(void*, int, int) = nullptr;
        int (*wclrtoeol)(void*) = nullptr;
        int (*wattron)(void*, int) = nullptr;
        int (*wattroff)(void*, int) = nullptr;
        int (*start_color)() = nullptr;
        int (*init_pair)(short, short, short) = nullptr;

        bool Load() {
            handle = dlopen("libncurses.so", RTLD_LAZY);
            if (!handle)
                handle = dlopen("libncurses.so.6", RTLD_LAZY);
            if (!handle)
                handle = dlopen("libncurses.so.5", RTLD_LAZY);
            if (!handle)
                handle = dlopen("libncursesw.so", RTLD_LAZY);
            if (!handle)
                handle = dlopen("libncursesw.so.6", RTLD_LAZY);
            if (!handle)
                handle = dlopen("libncursesw.so.5", RTLD_LAZY);
            if (!handle)
                return false;

#define LOAD_SYM(name) name = reinterpret_cast<decltype(name)>(dlsym(handle, #name)); \
                if (!name) { dlclose(handle); handle = nullptr; return false; }

            LOAD_SYM(initscr)
            LOAD_SYM(endwin)
            LOAD_SYM(newwin)
            LOAD_SYM(delwin)
            LOAD_SYM(wrefresh)
            LOAD_SYM(box)
            LOAD_SYM(mvwprintw)
            LOAD_SYM(wgetnstr)
            LOAD_SYM(wgetch)
            LOAD_SYM(keypad)
            LOAD_SYM(cbreak)
            LOAD_SYM(noecho)
            LOAD_SYM(wmove)
            LOAD_SYM(wclrtoeol)
            LOAD_SYM(wattron)
            LOAD_SYM(wattroff)
            LOAD_SYM(start_color)
            LOAD_SYM(init_pair)

#undef LOAD_SYM

            return true;
        }

        void Unload() {
            if (handle) {
                dlclose(handle);
                handle = nullptr;
            }
        }
    };
#endif

    TerminalDriver DetermineTerminalDriver() {
#ifdef __linux__
        if (!isatty(STDOUT_FILENO))
            return TerminalDriver::STANDARD_TERM;

        if (const char* term = std::getenv("TERM"); !term || std::string(term) == "dumb")
            return TerminalDriver::STANDARD_TERM;

        if (auto [width, height] = Terminal::terminalSize();
            width < MIN_TERMINAL_WIDTH_SIZE && height < MIN_TERMINAL_HEIGHT_SIZE)
            return TerminalDriver::STANDARD_TERM;

        void* handle = dlopen("libncurses.so", RTLD_LAZY);
        if (!handle)
            handle = dlopen("libncurses.so.6", RTLD_LAZY);
        if (!handle)
            handle = dlopen("libncurses.so.5", RTLD_LAZY);
        if (!handle)
            handle = dlopen("libncursesw.so", RTLD_LAZY);
        if (!handle)
            handle = dlopen("libncursesw.so.6", RTLD_LAZY);
        if (!handle)
            handle = dlopen("libncursesw.so.5", RTLD_LAZY);

        if (handle) {
            dlclose(handle);
            return TerminalDriver::NCURSES;
        }
#endif

        return TerminalDriver::STANDARD_TERM;
    }

    TerminalDialog::TerminalDialog(const TerminalDriver& driver) : m_driver(driver) {
    }

    TerminalDialog::~TerminalDialog() = default;

    void TerminalDialog::SetTitle(const std::string& title) {
        m_title = title;
    }

    std::string TerminalDialog::GetTitle() {
        return m_title;
    }

    void TerminalDialog::SetDialogMessage(const std::string& message) {
        m_message = message;
    }

    std::string TerminalDialog::GetDialogMessage() {
        return m_message;
    }

    int TerminalDialog::AddInputNode(const std::string& hint) {
        int id = static_cast<int>(m_inputNodes.size());
        m_inputNodes[id] = hint;
        return id;
    }

    int TerminalDialog::AddButton(const std::string& text, const std::function<void()>& handler) {
        int id = static_cast<int>(m_buttons.size());
        m_buttons[id] = {text, handler};
        return id;
    }

    std::string TerminalDialog::GetInputText(int nodeID) {
        auto it = m_inputNodes.find(nodeID);
        if (it == m_inputNodes.end())
            throw DialogError(std::format("input node {} not found", nodeID));

        return it->second;
    }

    void TerminalDialog::RemoveButton(int buttonID) {
        m_buttons.erase(buttonID);
    }

    void TerminalDialog::RemoveInputNode(int nodeID) {
        m_inputNodes.erase(nodeID);
    }

    void TerminalDialog::EnableTerminalClearing(bool terminalClearing) {
        m_standardShouldClearTerminal = terminalClearing;
    }

    void StandardTerminalDialog_ForceCloseThread(int) {
        ForceCloseThread(standardTerminalThread);
    }

    void TerminalDialog::Show() {
#ifdef __linux__
        if (m_driver == TerminalDriver::NCURSES) {
            NCursesSymbols nc;
            if (!nc.Load())
                throw DialogError("Failed to load ncurses");

            nc.initscr();
            nc.cbreak();
            nc.noecho();
            nc.start_color();

            int height = 10 + static_cast<int>(m_inputNodes.size()) * 2;
            int width = 60;

            if (auto [tWidth, tHeight] = Terminal::terminalSize();
                tWidth >= MIN_TERMINAL_WIDTH_SIZE && tHeight >= MIN_TERMINAL_HEIGHT_SIZE) {
                width = tWidth - 2;
                height = tHeight - 2;
            }

            void* win = nc.newwin(height, width, 1, 1);
            nc.keypad(win, true);
            nc.box(win, 0, 0);

            nc.mvwprintw(win, 1, (width - static_cast<int>(m_title.size())) / 2, "%s", m_title.c_str());
            nc.mvwprintw(win, 3, 2, "%s", m_message.c_str());

            int row = 4;

            for (auto& [id, hint] : m_inputNodes) {
                nc.mvwprintw(win, row++, 2, "%s: ", hint.c_str());
                char buf[256] = {};
                nc.wmove(win, row - 1, static_cast<int>(hint.size()) + 4);
                nc.wgetnstr(win, buf, sizeof(buf) - 1);
                m_inputNodes[id] = std::string(buf);
                row++;
            }

            int selected = 0;
            const int buttonCount = static_cast<int>(m_buttons.size());
            const int buttonRow = row;

            auto renderButtons = [&]() {
                int r = buttonRow;
                for (auto& [id, btn] : m_buttons) {
                    if (id == selected) {
                        nc.wattron(win, A_REVERSE);
                        nc.mvwprintw(win, r++, 2, "[ %s ]", btn.first.c_str());
                        nc.wattroff(win, A_REVERSE);
                    } else
                        nc.mvwprintw(win, r++, 2, "[ %s ]", btn.first.c_str());
                }
                nc.wrefresh(win);
            };

            if (!m_buttons.empty()) {
                renderButtons();

                int ch;
                while ((ch = nc.wgetch(win)) != '\n' && ch != '\r') {
                    if (ch == KEY_UP)
                        selected = (selected - 1 + buttonCount) % buttonCount;
                    else if (ch == KEY_DOWN)
                        selected = (selected + 1) % buttonCount;

                    renderButtons();
                }

                if (auto it = m_buttons.find(selected); it != m_buttons.end())
                    it->second.second();
            } else {
                nc.wrefresh(win);
                nc.wgetch(win);
            }

            nc.delwin(win);
            nc.endwin();
            nc.Unload();
        }
#endif
        if (m_driver == TerminalDriver::STANDARD_TERM) {
            standardTerminalThread = std::thread([&]() {
                if (m_standardShouldClearTerminal)
                    Terminal::clearTerminal();

                // when std::getline fucks around, we fuck around too
                std::signal(SIGABRT, StandardTerminalDialog_ForceCloseThread);
                std::signal(SIGINT, StandardTerminalDialog_ForceCloseThread);
                std::signal(SIGTERM, StandardTerminalDialog_ForceCloseThread);

                std::cout << "=== " << m_title << " ===" << std::endl;
                std::cout << m_message << std::endl;

                for (auto& [id, hint] : m_inputNodes) {
                    std::cout << hint << ": ";

                    if (std::string input; std::getline(std::cin, input))
                        m_inputNodes[id] = input;
                }

                for (auto& [id, btn] : m_buttons)
                    std::cout << "[" << id << "] " << btn.first << std::endl;

                if (!m_buttons.empty()) {
                    int choice = -1;
                    std::cout << ">>> ";
                    std::cin >> choice;

                    auto it = m_buttons.begin();
                    std::advance(it, choice);
                    if (it != m_buttons.end())
                        it->second.second();
                }
            });
            standardTerminalThread.join();
        }
    }
}
