#include <filesystem>
#include <iostream>
#include <optional>

#include <dev_tools/commons/HelpPage.hpp>

namespace fs = std::filesystem;

void showHelp(const std::optional<std::string>& arg0) {
    std::string out;
    if (arg0.has_value()) {
        auto in = fs::path(arg0.value());
        out = in.is_absolute() ? ".../" + in.filename().string() : in.string();
    } else
        out = "./LXEB-Creator";

    std::cout << "Usage: " << out << " [action] [params]\n" << std::endl;
    std::cout << "Provided actions: " << std::endl;

    HelpPage actions;
    actions.setSpaceWidth(3);
    actions.setStartSpaceWidth(2);
    actions.addArg("package", std::nullopt, "Create a LXEB-capable package");
    actions.addArg("extract", std::nullopt, "Extract the entire LXEB package or extract specific files from a package");
    actions.addArg("patch", std::nullopt, "Patch a LXEB package with new contents");
    actions.addArg("test-load", std::nullopt, "Validate a package by test-loading the libraries");
    actions.addArg("test-run", std::nullopt, "Validate a package by test-running into a specific web server");
    actions.addArg("sign", std::nullopt, "Self-sign a LXEB package or sign it with CA-authorized certificate");
    actions.display(std::cout);

    std::cout << "\nParams for \"package\":" << std::endl;
    HelpPage packageParams;
    packageParams.setSpaceWidth(5);
    packageParams.setStartSpaceWidth(2);
    packageParams.addArg("--outfile", "[file_path]", "The output file for the package");
    packageParams.addArg("--include", "[arch]=[input_dir]",
                         "Includes a directory with the shared libraries that contain architecture-specific code");
    packageParams.addArg("--manifest", "[file]", "The manifest file for the LXEB package");
    packageParams.addArg("--lxeb-pkgver", "[version_code]",
                         "Backward-compatibility for the LXEB packages on newer versions");
    packageParams.display(std::cout);

    std::cout << "\nParams for \"extract\":" << std::endl;
    HelpPage extractParams;
    extractParams.setSpaceWidth(5);
    extractParams.setStartSpaceWidth(2);
    extractParams.addArg("--infile", "[file_path]", "The LXEB package itself");
    extractParams.addArg("--arch-files", "[system_arch]",
                         "Extracts files only tailored to one specific system architecture");
    extractParams.addArg("--all-files", std::nullopt, "Extracts all files from the package");
    extractParams.addArg("--entry-file", std::nullopt,
                         "Extracts the entry library for the current system architecture");
    extractParams.addArg("--list", std::nullopt, "Lists out all files instead of extracting them");
    extractParams.addArg("--manifest", std::nullopt, "Extracts the manifest file");
    extractParams.addArg("--out", "[directory]", "Extracts to a specific directory");
    extractParams.display(std::cout);
}

int main(int argc, char** argv) {
    if (argc == 0 || argc == 1) {
        showHelp(argc == 1 ? std::optional<std::string>(argv[0]) : std::nullopt);
        return 1;
    }
    return 0;
}
