#pragma once

#include <string_view>

#include <llvm/ADT/StringMap.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Value.h>
#include <onnx/onnx-ml.pb.h>


namespace aetherweave {

class OpEmitter {
  public:
    static bool from_node(const onnx::NodeProto&        node,
                          mlir::OpBuilder&              builder,
                          llvm::StringMap<mlir::Value>& value_map);

    virtual ~OpEmitter() = default;

  protected:
    virtual bool is_supported_node(std::string_view op_type) = 0;
    virtual bool run(const onnx::NodeProto&        node,
                     mlir::OpBuilder&              builder,
                     llvm::StringMap<mlir::Value>& value_map)           = 0;
};

} // namespace aetherweave
