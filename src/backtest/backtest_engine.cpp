#include "backtest/backtest_engine.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <cmath>

namespace quant {
namespace backtest {

BacktestEngine::BacktestEngine(
    std::shared_ptr<data::DataFeed> data_feed,
    std::shared_ptr<strategy::Strategy> strategy,
    BacktestConfig config)
    : data_feed_(std::move(data_feed)),
      strategy_(std::move(strategy)),
      config_(std::move(config)),
      cash_(config.initial_capital),
      equity_(config.initial_capital) {
    
    if (!data_feed_) {
        throw std::invalid_argument("Data feed cannot be null");
    }
    if (!strategy_) {
        throw std::invalid_argument("Strategy cannot be null");
    }
}

void BacktestEngine::run() {
    // 初始化策略
    strategy_->initialize();
    
    // 获取所有交易品种的历史数据
    // 这里简化为只处理一个品种
    std::string symbol = "BTCUSDT";  // 示例
    
    auto bars = data_feed_->get_historical_bars(
        symbol,
        config_.start_time,
        config_.end_time,
        "1d"  // 日线数据
    );
    
    // 按时间排序
    std::sort(bars.begin(), bars.end(), [](const auto& a, const auto& b) {
        return a.timestamp < b.timestamp;
    });
    
    // 遍历每个时间点的数据
    for (const auto& bar : bars) {
        // 将K线数据转换为MarketData
        data::MarketData market_data = bar;
        
        // 调用策略处理数据
        auto signal = strategy_->on_data(market_data);
        
        // 处理信号
        if (signal) {
            process_signal(*signal, bar);
        }
        
        // 更新投资组合
        update_portfolio(bar);
        
        // 记录资金曲线
        equity_curve_.emplace_back(bar.timestamp, equity_);
    }
    
    // 计算性能指标
    performance_report_ = analysis::calculate_performance(
        equity_curve_,
        order_history_,
        config_.initial_capital
    );
}

void BacktestEngine::process_signal(const strategy::Signal& signal, const data::BarData& bar) {
    if (signal.type == strategy::SignalType::BUY) {
        // 计算可用资金的90%用于买入
        double amount_to_invest = cash_ * 0.9;
        double price = bar.close;
        double quantity = amount_to_invest / price;
        
        if (!config_.use_fractional_shares) {
            quantity = std::floor(quantity);
        }
        
        if (quantity <= 0) {
            return;  // 资金不足
        }
        
        // 创建订单
        execution::Order order;
        order.symbol = signal.symbol;
        order.timestamp = signal.timestamp;
        order.type = execution::OrderType::MARKET;
        order.side = execution::OrderSide::BUY;
        order.quantity = quantity;
        order.price = price;
        order.status = execution::OrderStatus::FILLED;
        
        // 计算手续费
        double commission = amount_to_invest * config_.commission_rate;
        
        // 更新现金和持仓
        cash_ -= (quantity * price + commission);
        positions_[signal.symbol] += quantity;
        
        // 记录订单
        order_history_.push_back(order);
        
    } else if (signal.type == strategy::SignalType::SELL) {
        // 获取当前持仓
        auto it = positions_.find(signal.symbol);
        if (it == positions_.end() || it->second <= 0) {
            return;  // 没有持仓
        }
        
        double quantity = it->second;
        double price = bar.close;
        
        // 创建订单
        execution::Order order;
        order.symbol = signal.symbol;
        order.timestamp = signal.timestamp;
        order.type = execution::OrderType::MARKET;
        order.side = execution::OrderSide::SELL;
        order.quantity = quantity;
        order.price = price;
        order.status = execution::OrderStatus::FILLED;
        
        // 计算手续费
        double commission = quantity * price * config_.commission_rate;
        
        // 更新现金和持仓
        cash_ += (quantity * price - commission);
        positions_[signal.symbol] = 0;
        
        // 记录订单
        order_history_.push_back(order);
    }
}

void BacktestEngine::update_portfolio(const data::BarData& bar) {
    // 计算当前持仓市值
    double position_value = 0.0;
    for (const auto& [symbol, quantity] : positions_) {
        if (symbol == bar.symbol) {
            position_value += quantity * bar.close;
        }
    }
    
    // 更新总资产
    equity_ = cash_ + position_value;
}

analysis::PerformanceReport BacktestEngine::get_performance_report() const {
    return performance_report_;
}

const std::vector<execution::Order>& BacktestEngine::get_order_history() const {
    return order_history_;
}

const std::vector<std::pair<data::Timestamp, double>>& BacktestEngine::get_equity_curve() const {
    return equity_curve_;
}

} // namespace backtest
} // namespace quant 