#pragma once

#include <filesystem>
#include <optional>
#include <string>


namespace aetherweave {

struct Configuration {
    struct Target {
        std::string device;
        std::string architecture;
    };

    bool                                 debug_mode;
    std::optional<std::filesystem::path> debug_directory_path;
    std::filesystem::path                import_path;
    std::filesystem::path                export_path;
    Target                               target;
};

} // namespace aetherweave
