#pragma once

#include <string_view>

#include <llvm/ADT/StringMap.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Value.h>
#include <onnx/onnx-ml.pb.h>

#include "compiler/import/mlir/onnx/op_emitter/OpEmitter.h"


namespace aetherweave {

class AddOpEmitter : public OpEmitter {
  protected:
    bool is_supported_node(std::string_view op_type) override;
    bool run(const onnx::NodeProto& node, mlir::OpBuilder& builder, llvm::StringMap<mlir::Value>& value_map) override;
};

} // namespace aetherweave
