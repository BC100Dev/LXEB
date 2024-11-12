#include <iostream>
#include <vector>
#include <array>
#include <string>

#include <unistd.h>

#include <LXEB/Core/Utils.hpp>

using namespace LXEB::Core;

int do_parseArgs(const std::vector<std::string>& args) {
    for (const std::string& arg : args) {
        std::vector<std::string> opt(2);
        if (Contains(arg, "="))
            opt = SplitString(arg, "=", 2);
        else
            opt[0] = arg;

        if (opt[0] == "--automated") {
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        std::vector<std::string> args(argc - 1);
        for (int i = 1; i < argc; i++)
            args[i - 1] = argv[i];

        int rc = do_parseArgs(args);
        if (rc != 0) {
            std::cout << "Error in parsing argument" << std::endl;
            return rc;
        }
    }
}