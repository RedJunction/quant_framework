#include "strategy/moving_average_strategy.hpp"
#include "data/data_feed.hpp"
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

int main() {
    std::cout << "Simple Moving Average Strategy Example" << std::endl;
    
    // 创建移动平均策略
    auto strategy = std::make_shared<quant::strategy::MovingAverageStrategy>(10, 30);
    
    // 初始化策略
    strategy->initialize();
    
    std::cout << "Strategy initialized with parameters:" << std::endl;
    for (const auto& [key, value] : strategy->parameters()) {
        std::cout << "  " << key << ": " << value << std::endl;
    }
    
    std::cout << "This is a simple example showing how to create a strategy." << std::endl;
    std::cout << "For a complete backtest example, see simple_backtest.cpp" << std::endl;
    
    return 0;
} 