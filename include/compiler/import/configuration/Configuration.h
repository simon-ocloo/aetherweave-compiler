#pragma once

#include <filesystem>
#include <string>


namespace aetherweave {

struct Configuration {
    struct Target {
        std::string device;
        std::string architecture;
    };

    std::filesystem::path import_path;
    std::filesystem::path export_path;
    Target                target;
};

} // namespace aetherweave
