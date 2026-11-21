#include <dev_tools/commons/Terminal.hpp>

#ifdef __linux__
#include <sys/ioctl.h>
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

void Terminal::print(std::ostream& stream, const TermColor& color, const std::string& msg, bool reset) {
    if (msg.empty())
        return;

    stream << translateColor(color) << msg;
    if (reset)
        stream << translateColor(TermColor::RESET);
}

void Terminal::print(std::ostream& stream, const TermColor& color, const std::stringstream& msg, bool reset) {
    print(stream, color, msg.str(), reset);
}

void Terminal::println(std::ostream& stream, const TermColor& color, const std::string& msg, bool reset) {
    print(stream, color, msg, reset);
    stream << "\n";
}

void Terminal::println(std::ostream& stream, const TermColor& color, const std::stringstream& msg, bool reset) {
    println(stream, color, msg.str(), reset);
}

std::string Terminal::translateColor(TermColor color) {
    switch (color) {
        case TermColor::RESET:   return "\x1B[0m";
        case TermColor::BLACK:   return "\x1B[0;30m";
        case TermColor::RED:     return "\x1B[0;31m";
        case TermColor::GREEN:   return "\x1B[0;32m";
        case TermColor::YELLOW:  return "\x1B[0;33m";
        case TermColor::BLUE:    return "\x1B[0;34m";
        case TermColor::PURPLE:  return "\x1B[0;35m";
        case TermColor::CYAN:    return "\x1B[0;36m";
        case TermColor::WHITE:   return "\x1B[0;37m";
        default:                 return "\x1B[0m";
    }
}

void Terminal::clearTerminal() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    COORD homeCoords = {0, 0};

    if (hConsole == INVALID_HANDLE_VALUE)
        return;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    FillConsoleOutputCharacter(hConsole, ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hConsole, homeCoords);
#else
    std::cout << "\x1b[H\x1b[2J\x1b[3J" << std::flush;
#endif
}

Terminal::Size Terminal::terminalSize() {
    int w = -1, h = -1;

#ifdef __linux__
    struct winsize s{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &s) == -1)
        return Size{-1, -1};

    w = s.ws_col;
    h = s.ws_row;
#endif

#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO s;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &s)) {
        w = s.srWindow.Right - s.srWindow.Left + 1;
        h = s.srWindow.Bottom - s.srWindow.Top + 1;
    }
#endif

    return Size{w, h};
}