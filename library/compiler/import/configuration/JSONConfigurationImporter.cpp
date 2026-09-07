#include <filesystem>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

#include <llvm/Support/Error.h>
#include <llvm/Support/JSON.h>
#include <llvm/Support/raw_ostream.h>

#include "compiler/import/configuration/JSONConfigurationImporter.h"


namespace aetherweave {

bool JSONConfigurationImporter::is_supported_extension(const std::filesystem::path& extension)
{
    return extension == ".json";
}


namespace {

std::optional<std::string> read_file(const std::filesystem::path& path)
{
    std::ifstream file(path);
    if (file.is_open() == false) {
        llvm::errs() << "JSONConfigurationImporter: cannot open '" << path.string() << "'\n";
        return std::nullopt;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


std::optional<Configuration::Target> parse_target(const llvm::json::Object& root)
{
    const auto* target = root.getObject("target");
    if (target == nullptr) {
        llvm::errs() << "JSONConfigurationImporter: missing 'target'\n";
        return std::nullopt;
    }

    auto backend = target->getString("backend");
    if (backend.has_value() == false) {
        llvm::errs() << "JSONConfigurationImporter: missing 'target.backend'\n";
        return std::nullopt;
    }

    auto arch = target->getString("arch");
    if (arch.has_value() == false) {
        llvm::errs() << "JSONConfigurationImporter: missing 'target.arch'\n";
        return std::nullopt;
    }

    return Configuration::Target{.arch = arch->str(), .backend = backend->str()};
}


std::optional<Configuration> parse_configuration(const llvm::json::Object& root)
{
    auto import_path = root.getString("import_path");
    if (import_path.has_value() == false) {
        llvm::errs() << "JSONConfigurationImporter: missing 'import_path'\n";
        return std::nullopt;
    }

    auto export_path = root.getString("export_path");
    if (export_path.has_value() == false) {
        llvm::errs() << "JSONConfigurationImporter: missing 'export_path'\n";
        return std::nullopt;
    }

    auto target = parse_target(root);
    if (target.has_value() == false) {
        return std::nullopt;
    }

    return Configuration{.export_path = export_path->str(), .import_path = import_path->str(), .target = *target};
}

} // namespace


std::optional<Configuration> JSONConfigurationImporter::run(const std::filesystem::path& path)
{
    auto content = read_file(path);
    if (content.has_value() == false) {
        return std::nullopt;
    }

    auto json = llvm::json::parse(*content);
    if (auto error = json.takeError()) {
        llvm::errs() << "JSONConfigurationImporter: invalid JSON in '" << path.string() << "'\n";
        llvm::consumeError(std::move(error));
        return std::nullopt;
    }

    auto* root = json->getAsObject();
    if (root == nullptr) {
        llvm::errs() << "JSONConfigurationImporter: config must be a JSON object\n";
        return std::nullopt;
    }
    return parse_configuration(*root);
}

} // namespace aetherweave
