#pragma once

#include "data/data_types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>

namespace quant {
namespace strategy {

// 交易信号类型
enum class SignalType {
    BUY,
    SELL,
    HOLD
};

// 交易信号
struct Signal {
    data::Timestamp timestamp;
    std::string symbol;
    SignalType type;
    double strength = 1.0;  // 信号强度，0.0-1.0
    std::unordered_map<std::string, std::string> metadata;
};

// 策略接口
class Strategy {
public:
    virtual ~Strategy() = default;
    
    // 初始化策略
    virtual void initialize() = 0;
    
    // 处理新的市场数据
    virtual std::optional<Signal> on_data(const data::MarketData& data) = 0;
    
    // 策略名称
    virtual std::string name() const = 0;
    
    // 策略参数
    virtual std::unordered_map<std::string, std::string> parameters() const = 0;
};

// 策略工厂，用于创建策略实例
class StrategyFactory {
public:
    using StrategyCreator = std::function<std::unique_ptr<Strategy>()>;
    
    // 注册策略
    static void register_strategy(const std::string& name, StrategyCreator creator);
    
    // 创建策略实例
    static std::unique_ptr<Strategy> create_strategy(const std::string& name);
    
    // 获取所有已注册的策略名称
    static std::vector<std::string> get_registered_strategies();
    
private:
    static std::unordered_map<std::string, StrategyCreator>& get_registry();
};

// 策略注册辅助宏
#define REGISTER_STRATEGY(StrategyClass) \
    static bool StrategyClass##_registered = []() { \
        quant::strategy::StrategyFactory::register_strategy( \
            #StrategyClass, \
            []() { return std::make_unique<StrategyClass>(); } \
        ); \
        return true; \
    }();

} // namespace strategy
} // namespace quant 