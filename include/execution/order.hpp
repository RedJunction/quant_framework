#pragma once

#include "data/data_types.hpp"
#include <string>
#include <unordered_map>

namespace quant {
namespace execution {

// 订单类型
enum class OrderType {
    MARKET,     // 市价单
    LIMIT,      // 限价单
    STOP,       // 止损单
    STOP_LIMIT  // 止损限价单
};

// 订单方向
enum class OrderSide {
    BUY,        // 买入
    SELL        // 卖出
};

// 订单状态
enum class OrderStatus {
    PENDING,    // 等待中
    SUBMITTED,  // 已提交
    ACCEPTED,   // 已接受
    REJECTED,   // 已拒绝
    CANCELED,   // 已取消
    FILLED,     // 已成交
    PARTIALLY_FILLED  // 部分成交
};

// 订单结构
struct Order {
    std::string id;                  // 订单ID
    std::string symbol;              // 交易品种
    data::Timestamp timestamp;       // 订单时间
    OrderType type;                  // 订单类型
    OrderSide side;                  // 买卖方向
    double quantity;                 // 数量
    double price;                    // 价格
    double filled_quantity = 0.0;    // 已成交数量
    double average_price = 0.0;      // 平均成交价格
    OrderStatus status = OrderStatus::PENDING;  // 订单状态
    std::unordered_map<std::string, std::string> metadata;  // 元数据
};

} // namespace execution
} // namespace quant 