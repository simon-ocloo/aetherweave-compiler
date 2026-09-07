#include <string_view>

#include <llvm/ADT/StringMap.h>
#include <llvm/Support/raw_ostream.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/Value.h>
#include <onnx/onnx-ml.pb.h>

#include "compiler/import/mlir/onnx/op_emitter/AddOpEmitter.h"
#include "compiler/import/mlir/onnx/op_emitter/OpEmitter.h"
#include "helper/make_handlers.h"


namespace aetherweave {

bool OpEmitter::from_node(const onnx::NodeProto&        node,
                          mlir::OpBuilder&              builder,
                          llvm::StringMap<mlir::Value>& value_map)
{
    auto handlers = make_handlers<OpEmitter, AddOpEmitter>();
    for (auto& handler : handlers) {
        if (handler->is_supported_node(node.op_type())) {
            return handler->run(node, builder, value_map);
        }
    }
    llvm::errs() << "OpEmitter: unsupported op '" << node.op_type() << "'\n";
    return false;
}

} // namespace aetherweave
