#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <optional>
#include <variant>
#include <unordered_map>

namespace quant {
namespace data {

using Timestamp = std::time_t;

// 价格数据结构
struct BarData {
    Timestamp timestamp;
    std::string symbol;
    double open;
    double high;
    double low;
    double close;
    double volume;
    
    // 替换C++20的默认比较运算符
    bool operator==(const BarData& other) const {
        return timestamp == other.timestamp && 
               symbol == other.symbol &&
               open == other.open &&
               high == other.high &&
               low == other.low &&
               close == other.close &&
               volume == other.volume;
    }
    
    bool operator!=(const BarData& other) const {
        return !(*this == other);
    }
};

// 订单簿数据
struct OrderBookLevel {
    double price;
    double volume;
};

struct OrderBook {
    Timestamp timestamp;
    std::string symbol;
    std::vector<OrderBookLevel> bids;
    std::vector<OrderBookLevel> asks;
};

// 交易数据
struct Trade {
    Timestamp timestamp;
    std::string symbol;
    double price;
    double volume;
    bool is_buyer_maker;
};

// 使用std::variant统一不同类型的市场数据
using MarketData = BarData;

} // namespace data
} // namespace quant 