#include "data/data_feed.hpp"
#include <algorithm>
#include <stdexcept>

namespace quant {
namespace data {

void DataFeed::add_data_source(std::shared_ptr<DataSource> source) {
    if (source) {
        data_sources_.push_back(source);
    }
}

std::vector<BarData> DataFeed::get_historical_bars(
    const std::string& symbol,
    const Timestamp& start_time,
    const Timestamp& end_time,
    const std::string& timeframe) {
    
    // 如果有特定的数据源映射，使用它
    auto it = symbol_to_source_.find(symbol);
    if (it != symbol_to_source_.end() && it->second) {
        return it->second->get_historical_bars(symbol, start_time, end_time, timeframe);
    }
    
    // 否则尝试所有数据源
    for (auto& source : data_sources_) {
        try {
            auto bars = source->get_historical_bars(symbol, start_time, end_time, timeframe);
            if (!bars.empty()) {
                return bars;
            }
        } catch (const std::exception&) {
            // 忽略错误，尝试下一个数据源
        }
    }
    
    // 没有找到数据
    return {};
}

void DataFeed::subscribe_market_data(
    const std::string& symbol,
    std::function<void(const MarketData&)> callback) {
    
    // 如果有特定的数据源映射，使用它
    auto it = symbol_to_source_.find(symbol);
    if (it != symbol_to_source_.end() && it->second) {
        it->second->subscribe_market_data(symbol, callback);
        return;
    }
    
    // 否则尝试所有数据源
    for (auto& source : data_sources_) {
        try {
            source->subscribe_market_data(symbol, callback);
            return;
        } catch (const std::exception&) {
            // 忽略错误，尝试下一个数据源
        }
    }
}

void DataFeed::unsubscribe_market_data(const std::string& symbol) {
    // 如果有特定的数据源映射，使用它
    auto it = symbol_to_source_.find(symbol);
    if (it != symbol_to_source_.end() && it->second) {
        it->second->unsubscribe_market_data(symbol);
        return;
    }
    
    // 否则尝试所有数据源
    for (auto& source : data_sources_) {
        try {
            source->unsubscribe_market_data(symbol);
        } catch (const std::exception&) {
            // 忽略错误
        }
    }
}

} // namespace data
} // namespace quant 