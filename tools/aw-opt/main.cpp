#include <mlir/Conversion/Passes.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/Arith/Transforms/BufferizableOpInterfaceImpl.h>
#include <mlir/Dialect/Bufferization/IR/Bufferization.h>
#include <mlir/Dialect/Bufferization/Pipelines/Passes.h>
#include <mlir/Dialect/Bufferization/Transforms/FuncBufferizableOpInterfaceImpl.h>
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
#include <mlir/Pass/PassRegistry.h>
#include <mlir/Tools/mlir-opt/MlirOptMain.h>
#include <mlir/Transforms/Passes.h>

#include "compiler/dialect/aether/AetherDialect.h"
#include "compiler/pass/conversion/aether_to_linalg/AetherToLinalg.h"
#include "helper/pass.h"


int main(int argc, char** argv)
{
    mlir::DialectRegistry registry;
    registry.insert<aetherweave::AetherDialect,
                    mlir::arith::ArithDialect,
                    mlir::bufferization::BufferizationDialect,
                    mlir::cf::ControlFlowDialect,
                    mlir::func::FuncDialect,
                    mlir::index::IndexDialect,
                    mlir::LLVM::LLVMDialect,
                    mlir::linalg::LinalgDialect,
                    mlir::memref::MemRefDialect,
                    mlir::scf::SCFDialect,
                    mlir::tensor::TensorDialect>();

    mlir::arith::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::bufferization::func_ext::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::linalg::registerBufferizableOpInterfaceExternalModels(registry);
    mlir::tensor::registerBufferizableOpInterfaceExternalModels(registry);

    mlir::registerPass(aetherweave::helper::create_pass_factory<aetherweave::AetherToLinalgPass>());
    mlir::registerArithToLLVMConversionPass();
    mlir::registerConvertControlFlowToLLVMPass();
    mlir::registerConvertFuncToLLVMPass();
    mlir::registerConvertIndexToLLVMPass();
    mlir::registerFinalizeMemRefToLLVMConversionPass();
    mlir::registerReconcileUnrealizedCastsPass();
    mlir::registerSCFToControlFlowPass();
    mlir::bufferization::registerBufferizationPasses();
    mlir::bufferization::registerBufferizationPipelines();
    mlir::registerLinalgPasses();
    mlir::memref::registerMemRefPasses();
    mlir::registerTransformsPasses();

    return mlir::asMainReturnCode(mlir::MlirOptMain(argc, argv, "aw-opt", registry));
}
