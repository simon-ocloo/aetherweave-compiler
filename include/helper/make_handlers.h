#pragma once

#include <memory>
#include <vector>


namespace aetherweave {

template <typename Base, typename... Derived>
std::vector<std::unique_ptr<Base>> make_handlers()
{
    std::vector<std::unique_ptr<Base>> handlers;

    handlers.reserve(sizeof...(Derived));
    (handlers.push_back(std::make_unique<Derived>()), ...);

    return handlers;
}

} // namespace aetherweave
