#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/Conversion/Passes.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Arith/Transforms/BufferizableOpInterfaceImpl.h>
#include <mlir/Dialect/Bufferization/IR/Bufferization.h>
#include <mlir/Dialect/Bufferization/Pipelines/Passes.h>
#include <mlir/Dialect/Bufferization/Transforms/FuncBufferizableOpInterfaceImpl.h>
#include <mlir/Dialect/Bufferization/Transforms/OneShotAnalysis.h>
#include <mlir/Dialect/Bufferization/Transforms/Passes.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/Index/IR/IndexDialect.h>
#include <mlir/Dialect/Linalg/IR/Linalg.h>
#include <mlir/Dialect/Linalg/Passes.h>
#include <mlir/Dialect/Linalg/Transforms/BufferizableOpInterfaceImpl.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/MemRef/IR/MemRef.h>
#include <mlir/Dialect/MemRef/Transforms/Passes.h>
#include <mlir/Dialect/SCF/IR/SCF.h>
#include <mlir/Dialect/Tensor/IR/Tensor.h>
#include <mlir/Dialect/Tensor/Transforms/BufferizableOpInterfaceImpl.h>
#include <mlir/IR/DialectRegistry.h>
#include <mlir/Pass/PassManager.h>
#include <mlir/Support/LogicalResult.h>
#include <mlir/Target/LLVMIR/Dialect/Builtin/BuiltinToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Dialect/LLVMIR/LLVMToLLVMIRTranslation.h>
#include <mlir/Target/LLVMIR/Export.h>

#include "compiler/dialect/aether/AetherDialect.h"
#include "compiler/import/configuration/Configuration.h"
#include "compiler/import/mlir/MLIRImporter.h"
#include "compiler/orchestrator/Orchestrator.h"
#include "compiler/pass/conversion/aether_to_linalg/AetherToLinalg.h"
#include "compiler/pass/instrumentation/DebugPassInstrumentation.h"
#include "helper/pass.h"


namespace aetherweave {

Orchestrator::Orchestrator() : llvm_context(), mlir_context(), registered_extensions()
{
}


void Orchestrator::load_dialects(const Configuration& configuration)
{
    this->mlir_context.loadDialect<AetherDialect>();
    this->mlir_context.loadDialect<mlir::func::FuncDialect>();
    this->mlir_context.loadDialect<mlir::LLVM::LLVMDialect>();
    if (configuration.target.device == "CPU") {
        this->mlir_context.loadDialect<mlir::arith::ArithDialect>();
        this->mlir_context.loadDialect<mlir::bufferization::BufferizationDialect>();
        this->mlir_context.loadDialect<mlir::cf::ControlFlowDialect>();
        this->mlir_context.loadDialect<mlir::index::IndexDialect>();
        this->mlir_context.loadDialect<mlir::linalg::LinalgDialect>();
        this->mlir_context.loadDialect<mlir::memref::MemRefDialect>();
        this->mlir_context.loadDialect<mlir::scf::SCFDialect>();
        this->mlir_context.loadDialect<mlir::tensor::TensorDialect>();
    }
}


void Orchestrator::register_extensions(const Configuration& configuration)
{
    bool                  any_new = false;
    mlir::DialectRegistry dialect_registry;

    auto register_one = [&](RegistrationFunction registration_function) {
        if (this->registered_extensions.count(registration_function) == 0) {
            registration_function(dialect_registry);
            this->registered_extensions.insert(registration_function);
            any_new = true;
        }
    };

    register_one(mlir::registerBuiltinDialectTranslation);
    register_one(mlir::registerLLVMDialectTranslation);
    if (configuration.target.device == "CPU") {
        register_one(mlir::arith::registerBufferizableOpInterfaceExternalModels);
        register_one(mlir::bufferization::func_ext::registerBufferizableOpInterfaceExternalModels);
        register_one(mlir::linalg::registerBufferizableOpInterfaceExternalModels);
        register_one(mlir::tensor::registerBufferizableOpInterfaceExternalModels);
    }

    if (any_new == true) {
        this->mlir_context.appendDialectRegistry(dialect_registry);
    }
}


namespace {

bool is_target_supported(const Configuration& configuration)
{
    if (configuration.target.device == "CPU" and configuration.target.architecture == "x86_64") {
        return true;
    }
    llvm::errs() << "Orchestrator: unsupported target '" << configuration.target.device << " / "
                 << configuration.target.architecture << "'\n";
    return false;
}


void build_pass_pipeline(mlir::PassManager& pass_manager, const Configuration& configuration)
{
    pass_manager.addPass(helper::create_pass<AetherToLinalgPass>());
    if (configuration.target.device == "CPU") {
        {
            mlir::bufferization::OneShotBufferizationOptions options;
            options.bufferizeFunctionBoundaries = true;
            pass_manager.addPass(mlir::bufferization::createOneShotBufferizePass(options));
        }
        {
            mlir::bufferization::BufferDeallocationPipelineOptions options;
            mlir::bufferization::buildBufferDeallocationPipeline(pass_manager, options);
        }
        pass_manager.addPass(mlir::createConvertLinalgToLoopsPass());
        pass_manager.addPass(mlir::createConvertSCFToCFPass());
        pass_manager.addPass(mlir::createConvertIndexToLLVMPass());
        pass_manager.addPass(mlir::createArithToLLVMConversionPass());
        pass_manager.addPass(mlir::memref::createExpandStridedMetadataPass());
        pass_manager.addPass(mlir::createFinalizeMemRefToLLVMConversionPass());
        pass_manager.addPass(mlir::createConvertFuncToLLVMPass());
        pass_manager.addPass(mlir::createConvertControlFlowToLLVMPass());
        pass_manager.addPass(mlir::createReconcileUnrealizedCastsPass());
    }
}

} // namespace


bool Orchestrator::run(const Configuration& configuration)
{
    if (is_target_supported(configuration) == false) {
        return false;
    }

    mlir::PassManager pass_manager(&this->mlir_context);
    this->load_dialects(configuration);
    this->register_extensions(configuration);
    build_pass_pipeline(pass_manager, configuration);

    auto mlir_module = MLIRImporter::from_file(this->mlir_context, configuration.import_path);
    if (static_cast<bool>(mlir_module) == false) {
        return false;
    }

    if (configuration.debug_mode == true and configuration.debug_directory_path.has_value() == true) {
        pass_manager.addInstrumentation(helper::create_pass_instrumentation<DebugPassInstrumentation>(
            configuration.debug_directory_path.value(), mlir_module->getOperation()));
    }

    if (mlir::failed(pass_manager.run(*mlir_module))) {
        return false;
    }

    auto llvm_module = mlir::translateModuleToLLVMIR(*mlir_module, this->llvm_context);
    if (llvm_module == nullptr) {
        llvm::errs() << "Orchestrator: LLVM IR translation failed\n";
        return false;
    }

    return true;
}

} // namespace aetherweave
