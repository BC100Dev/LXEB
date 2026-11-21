#ifndef DEVTOOLS_DIALOG_HPP
#define DEVTOOLS_DIALOG_HPP

#include <functional>
#include <map>
#include <stdexcept>
#include <stdexcept>
#include <string>
#include <vector>

namespace DevTools {

    enum class TerminalDriver {
#ifdef __linux__
        NCURSES = 0x1f3e03,
#endif

        STANDARD_TERM = 0x9c31fa,
    };

    TerminalDriver DetermineTerminalDriver();

    class DialogError : public std::runtime_error {
    public:
        explicit DialogError(const std::string& message) : std::runtime_error(message) {}
    };

    class TerminalDialog {
    public:
        enum class DialogType {
            D_INFO,
            D_WARN,
            D_ERROR,
        };

        explicit TerminalDialog(const TerminalDriver& driver);

        virtual ~TerminalDialog();

        void SetTitle(const std::string& title);

        std::string GetTitle();

        void SetDialogMessage(const std::string& message);

        std::string GetDialogMessage();

        int AddInputNode(const std::string& hint);

        int AddButton(const std::string& text, const std::function<void()>& handler);

        std::string GetInputText(int nodeID);

        void RemoveButton(int buttonID);

        void RemoveInputNode(int nodeID);

        void EnableTerminalClearing(bool terminalClearing);

        void Show();
    protected:
        // params: id, text
        using InputNodeMap = std::map<int, std::string>;

        TerminalDriver m_driver;

        std::string m_title;
        std::string m_message;
        InputNodeMap m_inputNodes;
        std::map<int, std::pair<std::string, std::function<void()>>> m_buttons;
        bool m_standardShouldClearTerminal = false;
    };

}

#endif //DEVTOOLS_DIALOG_HPP
