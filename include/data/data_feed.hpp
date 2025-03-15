#pragma once

#include "data_types.hpp"
#include <memory>
#include <functional>
#include <string>
#include <vector>
#include <future>

namespace quant {
namespace data {

// 数据源接口
class DataSource {
public:
    virtual ~DataSource() = default;
    
    // 获取历史数据
    virtual std::vector<BarData> get_historical_bars(
        const std::string& symbol,
        const Timestamp& start_time,
        const Timestamp& end_time,
        const std::string& timeframe) = 0;
        
    // 订阅实时数据
    virtual void subscribe_market_data(
        const std::string& symbol,
        std::function<void(const MarketData&)> callback) = 0;
        
    // 取消订阅
    virtual void unsubscribe_market_data(const std::string& symbol) = 0;
};

// 数据馈送类，管理多个数据源
class DataFeed {
public:
    // 添加数据源
    void add_data_source(std::shared_ptr<DataSource> source);
    
    // 获取历史数据，自动选择合适的数据源
    virtual std::vector<BarData> get_historical_bars(
        const std::string& symbol,
        const Timestamp& start_time,
        const Timestamp& end_time,
        const std::string& timeframe);
        
    // 订阅实时数据
    virtual void subscribe_market_data(
        const std::string& symbol,
        std::function<void(const MarketData&)> callback);
        
    // 取消订阅
    virtual void unsubscribe_market_data(const std::string& symbol);
    
private:
    std::vector<std::shared_ptr<DataSource>> data_sources_;
    std::unordered_map<std::string, std::shared_ptr<DataSource>> symbol_to_source_;
};

} // namespace data
} // namespace quant 