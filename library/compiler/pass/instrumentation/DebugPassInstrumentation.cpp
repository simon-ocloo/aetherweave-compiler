#include <filesystem>
#include <format>
#include <optional>
#include <string>
#include <system_error>

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/Operation.h>
#include <mlir/Pass/Pass.h>

#include "compiler/pass/instrumentation/DebugPassInstrumentation.h"


namespace aetherweave {

namespace {

std::optional<std::filesystem::path> reset_snapshot_directory(const std::filesystem::path& directory_path)
{
    auto            snapshot_directory_path = directory_path / "mlir_snapshots";
    std::error_code error_code;
    std::filesystem::remove_all(snapshot_directory_path, error_code);
    if (error_code) {
        llvm::errs() << "DebugPassInstrumentation: cannot clear '" << snapshot_directory_path.string()
                     << "': " << error_code.message() << ", MLIR snapshots are disabled\n";
        return std::nullopt;
    }
    std::filesystem::create_directories(snapshot_directory_path, error_code);
    if (error_code) {
        llvm::errs() << "DebugPassInstrumentation: cannot create '" << snapshot_directory_path.string()
                     << "': " << error_code.message() << ", MLIR snapshots are disabled\n";
        return std::nullopt;
    }
    return snapshot_directory_path;
}


void write_mlir_snapshot(const std::optional<std::filesystem::path>& snapshot_directory_path,
                         int                                         step,
                         llvm::StringRef                             snapshot_name,
                         mlir::Operation*                            operation)
{
    if (snapshot_directory_path.has_value() == false) {
        return;
    }

    std::string          file_name = std::format("{:02}.{}.mlir", step, snapshot_name.str());
    std::error_code      error_code;
    llvm::raw_fd_ostream file((snapshot_directory_path.value() / file_name).string(), error_code);
    if (error_code) {
        llvm::errs() << "DebugPassInstrumentation: cannot write '" << file_name << "': " << error_code.message()
                     << "\n";
        return;
    }
    operation->print(file);
    file.close();
    if (file.has_error() == true) {
        llvm::errs() << "DebugPassInstrumentation: cannot write '" << file_name << "': " << file.error().message()
                     << "\n";
        file.clear_error();
    }
}

} // namespace


DebugPassInstrumentation::DebugPassInstrumentation(const std::filesystem::path& directory_path,
                                                   mlir::Operation*             initial_operation)
    : snapshot_directory_path(reset_snapshot_directory(directory_path)), step(0)
{
    auto attribute = initial_operation->getAttrOfType<mlir::StringAttr>("debug.origin");
    auto origin_name = attribute ? attribute.getValue() : llvm::StringRef("unknown");
    write_mlir_snapshot(this->snapshot_directory_path, this->step++, origin_name, initial_operation);
}


void DebugPassInstrumentation::runAfterPass(mlir::Pass* pass, mlir::Operation* operation)
{
    write_mlir_snapshot(this->snapshot_directory_path, this->step++, pass->getName(), operation);
}


void DebugPassInstrumentation::runAfterPassFailed(mlir::Pass* pass, mlir::Operation* operation)
{
    std::string snapshot_name = std::format("{}.failed", pass->getName().str());
    write_mlir_snapshot(this->snapshot_directory_path, this->step++, snapshot_name, operation);
}


void DebugPassInstrumentation::runBeforePass(mlir::Pass* pass, mlir::Operation* operation)
{
    operation->setAttr("debug.argument", mlir::StringAttr::get(operation->getContext(), pass->getArgument()));
    operation->setAttr("debug.origin", mlir::StringAttr::get(operation->getContext(), pass->getName()));
}

} // namespace aetherweave
