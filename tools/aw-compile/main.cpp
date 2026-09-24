#include <cstdio>
#include <cstdlib>
#include <memory>

#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Linalg/IR/Linalg.h>
#include <mlir/Dialect/Tensor/IR/Tensor.h>
#include <mlir/IR/MLIRContext.h>
#include <mlir/Pass/PassManager.h>

#include "compiler/dialect/aether/AetherDialect.h"
#include "compiler/import/configuration/ConfigurationImporter.h"
#include "compiler/import/mlir/MLIRImporter.h"
#include "compiler/pass/conversion/aether_to_linalg/AetherToLinalg.h"


int main(int argc, char** argv)
{
    if (argc == 2) {
        auto configuration = aetherweave::ConfigurationImporter::from_file(argv[1]);
        if (configuration.has_value() == false) {
            return EXIT_FAILURE;
        }

        mlir::MLIRContext context;
        context.loadDialect<aetherweave::AetherDialect,
                            mlir::arith::ArithDialect,
                            mlir::func::FuncDialect,
                            mlir::linalg::LinalgDialect,
                            mlir::tensor::TensorDialect>();

        auto mlir_module = aetherweave::MLIRImporter::from_file(context, configuration.value().import_path);
        if (static_cast<bool>(mlir_module) == false) {
            return EXIT_FAILURE;
        }
        mlir_module->dump();

        mlir::PassManager pass_manager(&context);
        pass_manager.addPass(std::make_unique<aetherweave::AetherToLinalgPass>());

        if (mlir::failed(pass_manager.run(*mlir_module))) {
            return EXIT_FAILURE;
        }

        mlir_module->dump();

        return EXIT_SUCCESS;
    }
    else {
        std::fprintf(stderr, "Usage: aw-compile <config.json>\n");
        return EXIT_FAILURE;
    }
}
