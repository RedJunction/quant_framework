#pragma once

#include "data/data_types.hpp"
#include "execution/order.hpp"
#include <vector>
#include <string>
#include <unordered_map>

namespace quant {
namespace analysis {

// 性能报告
struct PerformanceReport {
    double total_return = 0.0;                // 总回报率
    double annualized_return = 0.0;           // 年化回报率
    double sharpe_ratio = 0.0;                // 夏普比率
    double max_drawdown = 0.0;                // 最大回撤
    double volatility = 0.0;                  // 波动率
    int total_trades = 0;                   // 总交易次数
    int winning_trades = 0;                 // 盈利交易次数
    int losing_trades = 0;                  // 亏损交易次数
    double win_rate = 0.0;                    // 胜率
    double profit_factor = 0.0;               // 盈亏比
    double average_profit = 0.0;              // 平均盈利
    double average_loss = 0.0;                // 平均亏损
    double largest_profit = 0.0;              // 最大盈利
    double largest_loss = 0.0;                // 最大亏损
    std::unordered_map<std::string, double> metrics; // 其他指标
};

// 计算性能指标
PerformanceReport calculate_performance(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve,
    const std::vector<execution::Order>& orders,
    double initial_capital);

// 计算回撤
std::vector<std::pair<data::Timestamp, double>> calculate_drawdowns(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve);

// 计算夏普比率
double calculate_sharpe_ratio(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve,
    double risk_free_rate = 0.0);

// 计算最大回撤
double calculate_max_drawdown(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve);

// 计算波动率
double calculate_volatility(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve);

} // namespace analysis
} // namespace quant 