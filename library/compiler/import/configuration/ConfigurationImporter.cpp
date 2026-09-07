#include <filesystem>
#include <optional>

#include <llvm/Support/raw_ostream.h>

#include "compiler/import/configuration/ConfigurationImporter.h"
#include "compiler/import/configuration/JSONConfigurationImporter.h"
#include "helper/make_handlers.h"


namespace aetherweave {

std::optional<Configuration> ConfigurationImporter::from_file(const std::filesystem::path& path)
{
    auto handlers = make_handlers<ConfigurationImporter, JSONConfigurationImporter>();
    for (auto& handler : handlers) {
        if (handler->is_supported_extension(path.extension())) {
            return handler->run(path);
        }
    }
    llvm::errs() << "ConfigurationImporter: unsupported format for '" << path.string() << "'\n";
    return std::nullopt;
}

} // namespace aetherweave
