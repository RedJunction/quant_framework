#pragma once

#include <vector>
#include <deque>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace quant {
namespace indicators {

// 简单移动平均线
class SimpleMovingAverage {
public:
    explicit SimpleMovingAverage(size_t period) : period_(period) {
        if (period == 0) {
            throw std::invalid_argument("Period must be greater than 0");
        }
    }
    
    void update(double value) {
        values_.push_back(value);
        
        // 保持队列大小不超过周期
        if (values_.size() > period_) {
            values_.pop_front();
        }
    }
    
    double get_value() const {
        if (values_.size() < period_) {
            throw std::runtime_error("Not enough data points");
        }
        
        return std::accumulate(values_.begin(), values_.end(), 0.0) / values_.size();
    }
    
    bool is_valid() const {
        return values_.size() >= period_;
    }
    
    void reset() {
        values_.clear();
    }
    
private:
    size_t period_;
    std::deque<double> values_;
};

// 指数移动平均线
class ExponentialMovingAverage {
public:
    explicit ExponentialMovingAverage(size_t period) : period_(period), alpha_(2.0 / (period + 1)), initialized_(false) {
        if (period == 0) {
            throw std::invalid_argument("Period must be greater than 0");
        }
    }
    
    void update(double value) {
        if (!initialized_) {
            current_value_ = value;
            initialized_ = true;
        } else {
            current_value_ = alpha_ * value + (1 - alpha_) * current_value_;
        }
        
        values_.push_back(value);
        
        // 保持队列大小不超过初始化所需的数据量
        if (values_.size() > period_) {
            values_.pop_front();
        }
    }
    
    double get_value() const {
        if (!initialized_) {
            throw std::runtime_error("EMA not initialized");
        }
        
        return current_value_;
    }
    
    bool is_valid() const {
        return initialized_;
    }
    
    void reset() {
        initialized_ = false;
        values_.clear();
    }
    
private:
    size_t period_;
    double alpha_;
    double current_value_ = 0.0;
    bool initialized_;
    std::deque<double> values_;
};

} // namespace indicators
} // namespace quant 