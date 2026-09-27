#pragma once

#include <memory>

#include <mlir/Pass/Pass.h>
#include <mlir/Pass/PassInstrumentation.h>


namespace aetherweave::helper {

template <typename PassType, typename... Args>
std::unique_ptr<mlir::Pass> create_pass(Args... args)
{
    return std::make_unique<PassType>(args...);
}


template <typename PassType, typename... Args>
auto create_pass_factory(Args... args)
{
    return [args...]() -> std::unique_ptr<mlir::Pass> { return create_pass<PassType>(args...); };
}


template <typename PassInstrumentation, typename... Args>
std::unique_ptr<mlir::PassInstrumentation> create_pass_instrumentation(Args... args)
{
    return std::make_unique<PassInstrumentation>(args...);
}

} // namespace aetherweave::helper
