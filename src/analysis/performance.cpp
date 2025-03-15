#include "analysis/performance.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace quant {
namespace analysis {

PerformanceReport calculate_performance(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve,
    const std::vector<execution::Order>& orders,
    double initial_capital) {
    
    PerformanceReport report;
    
    if (equity_curve.empty()) {
        return report;
    }
    
    // 计算总回报率
    double final_equity = equity_curve.back().second;
    report.total_return = (final_equity - initial_capital) / initial_capital;
    
    // 计算年化回报率
    auto start_time = equity_curve.front().first;
    auto end_time = equity_curve.back().first;
    double duration_seconds = difftime(end_time, start_time);
    double years = duration_seconds / (365.25 * 24 * 60 * 60);
    if (years > 0) {
        report.annualized_return = std::pow(1 + report.total_return, 1 / years) - 1;
    }
    
    // 计算最大回撤
    report.max_drawdown = calculate_max_drawdown(equity_curve);
    
    // 计算波动率
    report.volatility = calculate_volatility(equity_curve);
    
    // 计算夏普比率
    report.sharpe_ratio = calculate_sharpe_ratio(equity_curve);
    
    // 计算交易统计
    report.total_trades = orders.size();
    
    double total_profit = 0.0;
    double total_loss = 0.0;
    report.largest_profit = 0.0;
    report.largest_loss = 0.0;
    
    for (size_t i = 0; i < orders.size(); i += 2) {
        if (i + 1 >= orders.size()) {
            break;  // 忽略未平仓的交易
        }
        
        const auto& entry = orders[i];
        const auto& exit = orders[i + 1];
        
        double entry_value = entry.quantity * entry.price;
        double exit_value = exit.quantity * exit.price;
        
        double profit = 0.0;
        if (entry.side == execution::OrderSide::BUY) {
            profit = exit_value - entry_value;
        } else {
            profit = entry_value - exit_value;
        }
        
        if (profit > 0) {
            report.winning_trades++;
            total_profit += profit;
            report.largest_profit = std::max(report.largest_profit, profit);
        } else {
            report.losing_trades++;
            total_loss += std::abs(profit);
            report.largest_loss = std::max(report.largest_loss, std::abs(profit));
        }
    }
    
    // 计算胜率
    if (report.total_trades > 0) {
        report.win_rate = static_cast<double>(report.winning_trades) / report.total_trades;
    }
    
    // 计算盈亏比
    if (report.losing_trades > 0 && total_loss > 0) {
        report.profit_factor = total_profit / total_loss;
    }
    
    // 计算平均盈利和亏损
    if (report.winning_trades > 0) {
        report.average_profit = total_profit / report.winning_trades;
    }
    
    if (report.losing_trades > 0) {
        report.average_loss = total_loss / report.losing_trades;
    }
    
    return report;
}

std::vector<std::pair<data::Timestamp, double>> calculate_drawdowns(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve) {
    
    std::vector<std::pair<data::Timestamp, double>> drawdowns;
    
    if (equity_curve.empty()) {
        return drawdowns;
    }
    
    double peak = equity_curve[0].second;
    
    for (const auto& [timestamp, equity] : equity_curve) {
        if (equity > peak) {
            peak = equity;
        }
        
        double drawdown = (peak - equity) / peak;
        drawdowns.emplace_back(timestamp, drawdown);
    }
    
    return drawdowns;
}

double calculate_max_drawdown(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve) {
    
    if (equity_curve.empty()) {
        return 0.0;
    }
    
    double max_drawdown = 0.0;
    double peak = equity_curve[0].second;
    
    for (const auto& [_, equity] : equity_curve) {
        if (equity > peak) {
            peak = equity;
        } else {
            double drawdown = (peak - equity) / peak;
            max_drawdown = std::max(max_drawdown, drawdown);
        }
    }
    
    return max_drawdown;
}

double calculate_sharpe_ratio(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve,
    double risk_free_rate) {
    
    if (equity_curve.size() < 2) {
        return 0.0;
    }
    
    // 计算每日回报率
    std::vector<double> returns;
    for (size_t i = 1; i < equity_curve.size(); ++i) {
        double prev_equity = equity_curve[i - 1].second;
        double curr_equity = equity_curve[i].second;
        
        if (prev_equity > 0) {
            double daily_return = (curr_equity - prev_equity) / prev_equity;
            returns.push_back(daily_return);
        }
    }
    
    if (returns.empty()) {
        return 0.0;
    }
    
    // 计算平均回报率
    double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    // 计算回报率标准差
    double sum_squared_diff = 0.0;
    for (double r : returns) {
        double diff = r - mean_return;
        sum_squared_diff += diff * diff;
    }
    
    double std_dev = std::sqrt(sum_squared_diff / returns.size());
    
    if (std_dev == 0.0) {
        return 0.0;
    }
    
    // 计算夏普比率
    return (mean_return - risk_free_rate) / std_dev;
}

double calculate_volatility(
    const std::vector<std::pair<data::Timestamp, double>>& equity_curve) {
    
    if (equity_curve.size() < 2) {
        return 0.0;
    }
    
    // 计算每日回报率
    std::vector<double> returns;
    for (size_t i = 1; i < equity_curve.size(); ++i) {
        double prev_equity = equity_curve[i - 1].second;
        double curr_equity = equity_curve[i].second;
        
        if (prev_equity > 0) {
            double daily_return = (curr_equity - prev_equity) / prev_equity;
            returns.push_back(daily_return);
        }
    }
    
    if (returns.empty()) {
        return 0.0;
    }
    
    // 计算平均回报率
    double mean_return = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    // 计算回报率标准差
    double sum_squared_diff = 0.0;
    for (double r : returns) {
        double diff = r - mean_return;
        sum_squared_diff += diff * diff;
    }
    
    double variance = sum_squared_diff / returns.size();
    double std_dev = std::sqrt(variance);
    
    // 年化波动率（假设交易日为252天）
    return std_dev * std::sqrt(252);
}

} // namespace analysis
} // namespace quant 