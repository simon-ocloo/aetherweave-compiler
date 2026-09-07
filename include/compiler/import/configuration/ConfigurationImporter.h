#pragma once

#include <filesystem>
#include <optional>
#include <string>


namespace aetherweave {

struct Configuration {
    struct Target {
        std::string arch;
        std::string backend;
    };

    std::string export_path;
    std::string import_path;
    Target      target;
};


class ConfigurationImporter {
  public:
    static std::optional<Configuration> from_file(const std::filesystem::path& path);

    virtual ~ConfigurationImporter() = default;

  protected:
    virtual bool                         is_supported_extension(const std::filesystem::path& extension) = 0;
    virtual std::optional<Configuration> run(const std::filesystem::path& path) = 0;
};

} // namespace aetherweave
