#pragma once

#include <set>

#include <llvm/IR/LLVMContext.h>
#include <mlir/IR/DialectRegistry.h>
#include <mlir/IR/MLIRContext.h>

#include "compiler/import/configuration/Configuration.h"


namespace aetherweave {

class Orchestrator {
  public:
    Orchestrator();

    bool run(const Configuration& configuration);

  private:
    using RegistrationFunction = void (*)(mlir::DialectRegistry&);

    llvm::LLVMContext              llvm_context;
    mlir::MLIRContext              mlir_context;
    std::set<RegistrationFunction> registered_extensions;

    void load_dialects(const Configuration& configuration);
    void register_extensions(const Configuration& configuration);
};

} // namespace aetherweave
