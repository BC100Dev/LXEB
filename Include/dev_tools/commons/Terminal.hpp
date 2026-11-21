#ifndef DEVTOOLS_TERMINAL_HPP
#define DEVTOOLS_TERMINAL_HPP

#include <iostream>
#include <string>
#include <sstream>

class Terminal {
public:
    enum class TermColor {
        RESET,
        BLACK,
        RED,
        GREEN,
        YELLOW,
        BLUE,
        PURPLE,
        CYAN,
        WHITE
    };

    struct Size {
        int width;
        int height;
    };

    static void clearTerminal();

    static void print(std::ostream& stream, const TermColor& color, const std::string& msg, bool reset = true);
    static void print(std::ostream& stream, const TermColor& color, const std::stringstream& msg, bool reset = true);

    static void println(std::ostream& stream, const TermColor& color, const std::string& msg, bool reset = true);
    static void println(std::ostream& stream, const TermColor& color, const std::stringstream& msg, bool reset = true);

    static Size terminalSize();

private:
    static std::string translateColor(TermColor color);
};

#endif //DEVTOOLS_TERMINAL_HPP
