#pragma once

#include <filesystem>
#include <optional>

#include "compiler/import/configuration/Configuration.h"
#include "compiler/import/configuration/ConfigurationImporter.h"


namespace aetherweave {

class JSONConfigurationImporter : public ConfigurationImporter {
  protected:
    bool                         is_supported_extension(const std::filesystem::path& extension) const override;
    std::optional<Configuration> run(const std::filesystem::path& path) const override;
};

} // namespace aetherweave
