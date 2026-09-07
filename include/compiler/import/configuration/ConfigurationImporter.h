#pragma once

#include <filesystem>
#include <optional>

#include "compiler/import/configuration/Configuration.h"


namespace aetherweave {

class ConfigurationImporter {
  public:
    static std::optional<Configuration> from_file(const std::filesystem::path& path);

    virtual ~ConfigurationImporter() = default;

  protected:
    virtual bool                         is_supported_extension(const std::filesystem::path& extension) const = 0;
    virtual std::optional<Configuration> run(const std::filesystem::path& path) const = 0;
};

} // namespace aetherweave
