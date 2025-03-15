#pragma once

#include "data/data_feed.hpp"
#include "strategy/strategy.hpp"
#include "execution/order.hpp"
#include "analysis/performance.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace quant {
namespace backtest {

// 回测配置
struct BacktestConfig {
    data::Timestamp start_time;       // 回测开始时间
    data::Timestamp end_time;         // 回测结束时间
    double initial_capital = 100000;  // 初始资金
    double commission_rate = 0.0;     // 手续费率
    bool use_fractional_shares = false; // 是否使用分数股份
};

// 回测引擎
class BacktestEngine {
public:
    BacktestEngine(
        std::shared_ptr<data::DataFeed> data_feed,
        std::shared_ptr<strategy::Strategy> strategy,
        BacktestConfig config);
    
    // 运行回测
    void run();
    
    // 获取性能报告
    analysis::PerformanceReport get_performance_report() const;
    
    // 获取订单历史
    const std::vector<execution::Order>& get_order_history() const;
    
    // 获取资金曲线
    const std::vector<std::pair<data::Timestamp, double>>& get_equity_curve() const;
    
private:
    // 处理交易信号
    void process_signal(const strategy::Signal& signal, const data::BarData& bar);
    
    // 更新投资组合
    void update_portfolio(const data::BarData& bar);
    
    std::shared_ptr<data::DataFeed> data_feed_;
    std::shared_ptr<strategy::Strategy> strategy_;
    BacktestConfig config_;
    
    double cash_;                     // 当前现金
    double equity_;                   // 当前总资产
    std::unordered_map<std::string, double> positions_;  // 当前持仓
    
    std::vector<execution::Order> order_history_;  // 订单历史
    std::vector<std::pair<data::Timestamp, double>> equity_curve_;  // 资金曲线
    
    analysis::PerformanceReport performance_report_;  // 性能报告
};

} // namespace backtest
} // namespace quant 