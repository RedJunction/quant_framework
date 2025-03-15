#include "strategy/strategy.hpp"
#include <unordered_map>
#include <mutex>

namespace quant {
namespace strategy {

// 策略工厂实现
std::unordered_map<std::string, StrategyFactory::StrategyCreator>& StrategyFactory::get_registry() {
    static std::unordered_map<std::string, StrategyCreator> registry;
    return registry;
}

void StrategyFactory::register_strategy(const std::string& name, StrategyCreator creator) {
    get_registry()[name] = creator;
}

std::unique_ptr<Strategy> StrategyFactory::create_strategy(const std::string& name) {
    auto& registry = get_registry();
    auto it = registry.find(name);
    if (it != registry.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> StrategyFactory::get_registered_strategies() {
    std::vector<std::string> names;
    for (const auto& [name, _] : get_registry()) {
        names.push_back(name);
    }
    return names;
}

} // namespace strategy
} // namespace quant 