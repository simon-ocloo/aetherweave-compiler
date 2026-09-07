#include <cstdio>
#include <cstdlib>

#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/IR/MLIRContext.h>

#include "compiler/dialect/aether/AetherDialect.h"
#include "compiler/import/configuration/ConfigurationImporter.h"
#include "compiler/import/mlir/MLIRImporter.h"


int main(int argc, char** argv)
{
    if (argc == 2) {
        auto configuration = aetherweave::ConfigurationImporter::from_file(argv[1]);
        if (configuration.has_value() == false) {
            return EXIT_FAILURE;
        }

        mlir::MLIRContext context;
        context.loadDialect<aetherweave::AetherDialect, mlir::func::FuncDialect>();

        auto mlir_module = aetherweave::MLIRImporter::from_file(context, configuration->import_path);
        if (static_cast<bool>(mlir_module) == false) {
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
