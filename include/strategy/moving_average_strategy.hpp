#pragma once

#include "strategy.hpp"
#include "indicators/moving_average.hpp"
#include <memory>
#include <string>

namespace quant {
namespace strategy {

class MovingAverageStrategy : public Strategy {
public:
    MovingAverageStrategy(int fast_period = 10, int slow_period = 30)
        : fast_period_(fast_period),
          slow_period_(slow_period),
          fast_ma_(std::make_shared<indicators::SimpleMovingAverage>(fast_period)),
          slow_ma_(std::make_shared<indicators::SimpleMovingAverage>(slow_period)) {}
    
    void initialize() override {
        // 重置指标
        fast_ma_->reset();
        slow_ma_->reset();
    }
    
    std::optional<Signal> on_data(const data::MarketData& data) override {
        // 更新指标
        fast_ma_->update(data.close);
        slow_ma_->update(data.close);
        
        // 检查是否有足够的数据点
        if (!fast_ma_->is_valid() || !slow_ma_->is_valid()) {
            return std::nullopt;
        }
        
        double fast_value = fast_ma_->get_value();
        double slow_value = slow_ma_->get_value();
        
        // 生成交易信号
        if (prev_fast_value_ < prev_slow_value_ && fast_value > slow_value) {
            // 金叉：快线上穿慢线，买入信号
            Signal signal;
            signal.type = SignalType::BUY;
            signal.symbol = data.symbol;
            signal.timestamp = data.timestamp;
            
            prev_fast_value_ = fast_value;
            prev_slow_value_ = slow_value;
            return signal;
        }
        else if (prev_fast_value_ > prev_slow_value_ && fast_value < slow_value) {
            // 死叉：快线下穿慢线，卖出信号
            Signal signal;
            signal.type = SignalType::SELL;
            signal.symbol = data.symbol;
            signal.timestamp = data.timestamp;
            
            prev_fast_value_ = fast_value;
            prev_slow_value_ = slow_value;
            return signal;
        }
        
        // 更新前值
        prev_fast_value_ = fast_value;
        prev_slow_value_ = slow_value;
        return std::nullopt;
    }
    
    std::string name() const override {
        return "MovingAverageStrategy";
    }
    
    std::unordered_map<std::string, std::string> parameters() const override {
        return {
            {"fast_period", std::to_string(fast_period_)},
            {"slow_period", std::to_string(slow_period_)}
        };
    }
    
private:
    int fast_period_;
    int slow_period_;
    std::shared_ptr<indicators::SimpleMovingAverage> fast_ma_;
    std::shared_ptr<indicators::SimpleMovingAverage> slow_ma_;
    double prev_fast_value_ = 0.0;
    double prev_slow_value_ = 0.0;
};

} // namespace strategy
} // namespace quant 