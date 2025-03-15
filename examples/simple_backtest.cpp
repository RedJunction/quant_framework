#include "backtest/backtest_engine.hpp"
#include "data/data_feed.hpp"
#include "strategy/strategy.hpp"
#include "indicators/moving_average.hpp"
#include <iostream>
#include <memory>
#include <iomanip>
#include <chrono>

// 简单的移动平均交叉策略
class MACrossStrategy : public quant::strategy::Strategy {
public:
    MACrossStrategy(int fast_period = 5, int slow_period = 20)
        : fast_ma_(std::make_shared<quant::indicators::SimpleMovingAverage>(fast_period)),
          slow_ma_(std::make_shared<quant::indicators::SimpleMovingAverage>(slow_period)),
          fast_period_(fast_period),
          slow_period_(slow_period) {}

    void initialize() override {
        std::cout << "Initializing MA Cross Strategy (Fast: " << fast_period_ 
                  << ", Slow: " << slow_period_ << ")" << std::endl;
    }

    std::optional<quant::strategy::Signal> on_data(const quant::data::MarketData& data) override {
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
            quant::strategy::Signal signal;
            signal.type = quant::strategy::SignalType::BUY;
            signal.symbol = data.symbol;
            signal.timestamp = data.timestamp;
            
            std::cout << "BUY Signal at " << data.timestamp << " - Price: " << data.close << std::endl;
            
            prev_fast_value_ = fast_value;
            prev_slow_value_ = slow_value;
            return signal;
        }
        else if (prev_fast_value_ > prev_slow_value_ && fast_value < slow_value) {
            // 死叉：快线下穿慢线，卖出信号
            quant::strategy::Signal signal;
            signal.type = quant::strategy::SignalType::SELL;
            signal.symbol = data.symbol;
            signal.timestamp = data.timestamp;
            
            std::cout << "SELL Signal at " << data.timestamp << " - Price: " << data.close << std::endl;
            
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
        return "MACrossStrategy";
    }
    
    std::unordered_map<std::string, std::string> parameters() const override {
        return {
            {"fast_period", std::to_string(fast_period_)},
            {"slow_period", std::to_string(slow_period_)}
        };
    }

private:
    std::shared_ptr<quant::indicators::SimpleMovingAverage> fast_ma_;
    std::shared_ptr<quant::indicators::SimpleMovingAverage> slow_ma_;
    int fast_period_;
    int slow_period_;
    double prev_fast_value_ = 0.0;
    double prev_slow_value_ = 0.0;
};

// 简单的CSV数据源
class CSVDataFeed : public quant::data::DataFeed {
public:
    CSVDataFeed(const std::string& filename) : filename_(filename) {
        // 在实际应用中，这里会加载CSV文件
        // 这里为了简单，我们生成一些模拟数据
        generate_sample_data();
    }

    std::vector<quant::data::BarData> get_historical_bars(
        const std::string& symbol,
        const quant::data::Timestamp& start_time,
        const quant::data::Timestamp& end_time,
        const std::string& timeframe) override {
        
        std::vector<quant::data::BarData> filtered_bars;
        
        for (const auto& bar : sample_data_) {
            if (bar.symbol == symbol && bar.timestamp >= start_time && bar.timestamp <= end_time) {
                filtered_bars.push_back(bar);
            }
        }
        
        return filtered_bars;
    }
    
    void subscribe_market_data(
        const std::string& symbol,
        std::function<void(const quant::data::MarketData&)> callback) override {
        // 简化实现，不做任何事情
    }
    
    void unsubscribe_market_data(const std::string& symbol) override {
        // 简化实现，不做任何事情
    }

private:
    void generate_sample_data() {
        // 生成200天的模拟数据
        std::string symbol = "BTCUSDT";
        double price = 10000.0;
        
        auto now = std::time(nullptr);
        auto start_time = now - 86400 * 200; // 200天前
        
        for (int i = 0; i < 200; ++i) {
            quant::data::BarData bar;
            bar.symbol = symbol;
            bar.timestamp = start_time + i * 86400; // 每天一个数据点
            
            // 生成一些波动的价格
            double change = (std::rand() % 200 - 100) / 100.0 * 2.0; // -2% 到 +2% 的变化
            price += price * change / 100.0;
            
            bar.open = price;
            bar.high = price * (1.0 + (std::rand() % 100) / 1000.0); // 最高价比开盘价高0-10%
            bar.low = price * (1.0 - (std::rand() % 100) / 1000.0);  // 最低价比开盘价低0-10%
            bar.close = price * (1.0 + (std::rand() % 200 - 100) / 1000.0); // 收盘价在开盘价的±10%范围内
            bar.volume = std::rand() % 1000 + 100; // 100-1100的成交量
            
            sample_data_.push_back(bar);
        }
    }

    std::string filename_;
    std::vector<quant::data::BarData> sample_data_;
};

// 打印性能报告
void print_performance_report(const quant::analysis::PerformanceReport& report) {
    std::cout << "\n===== Performance Report =====\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Return: " << report.total_return * 100 << "%\n";
    std::cout << "Annualized Return: " << report.annualized_return * 100 << "%\n";
    std::cout << "Sharpe Ratio: " << report.sharpe_ratio << "\n";
    std::cout << "Max Drawdown: " << report.max_drawdown * 100 << "%\n";
    std::cout << "Volatility: " << report.volatility * 100 << "%\n";
    std::cout << "Total Trades: " << report.total_trades << "\n";
    std::cout << "Win Rate: " << report.win_rate * 100 << "%\n";
    std::cout << "Profit Factor: " << report.profit_factor << "\n";
    std::cout << "Average Profit: " << report.average_profit << "\n";
    std::cout << "Average Loss: " << report.average_loss << "\n";
    std::cout << "==============================\n";
}

int main() {
    // 创建数据源
    auto data_feed = std::make_shared<CSVDataFeed>("dummy.csv");
    
    // 创建策略
    auto strategy = std::make_shared<MACrossStrategy>(5, 20);
    
    // 设置回测配置
    quant::backtest::BacktestConfig config;
    config.initial_capital = 100000.0;
    config.commission_rate = 0.001; // 0.1% 手续费
    config.use_fractional_shares = true;
    
    // 设置回测时间范围
    auto now = std::time(nullptr);
    config.start_time = now - 86400 * 200; // 200天前
    config.end_time = now;
    
    // 创建回测引擎
    quant::backtest::BacktestEngine engine(data_feed, strategy, config);
    
    std::cout << "Starting backtest...\n";
    
    // 运行回测
    engine.run();
    
    // 获取回测结果
    auto performance_report = engine.get_performance_report();
    auto order_history = engine.get_order_history();
    auto equity_curve = engine.get_equity_curve();
    
    // 打印回测结果
    std::cout << "\nBacktest completed.\n";
    std::cout << "Total orders: " << order_history.size() << "\n";
    
    // 打印性能报告
    print_performance_report(performance_report);
    
    // 打印最终资产
    if (!equity_curve.empty()) {
        std::cout << "Final equity: $" << equity_curve.back().second << "\n";
    }
    
    return 0;
} 