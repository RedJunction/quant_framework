#pragma once

#include <vector>
#include <deque>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <cmath>

namespace quant {
namespace utils {

// 简单移动平均线
class SMA {
public:
    explicit SMA(size_t period) : period_(period) {
        if (period == 0) {
            throw std::invalid_argument("Period must be greater than 0");
        }
    }
    
    double calculate(const std::vector<double>& data) const {
        if (data.size() < period_) {
            throw std::invalid_argument("Not enough data points");
        }
        
        return std::accumulate(data.end() - period_, data.end(), 0.0) / period_;
    }
    
    double calculate(const std::deque<double>& data) const {
        if (data.size() < period_) {
            throw std::invalid_argument("Not enough data points");
        }
        
        return std::accumulate(data.end() - period_, data.end(), 0.0) / period_;
    }
    
    void reset() {
        // 无状态，不需要重置
    }
    
private:
    size_t period_;
};

// 指数移动平均线
class EMA {
public:
    explicit EMA(size_t period) : period_(period), alpha_(2.0 / (period + 1)), initialized_(false) {
        if (period == 0) {
            throw std::invalid_argument("Period must be greater than 0");
        }
    }
    
    double calculate(double value) {
        if (!initialized_) {
            current_value_ = value;
            initialized_ = true;
        } else {
            current_value_ = alpha_ * value + (1 - alpha_) * current_value_;
        }
        
        return current_value_;
    }
    
    double calculate(const std::vector<double>& data) {
        if (data.empty()) {
            throw std::invalid_argument("Data cannot be empty");
        }
        
        if (!initialized_ && data.size() >= period_) {
            // 使用SMA初始化
            current_value_ = std::accumulate(data.begin(), data.begin() + period_, 0.0) / period_;
            initialized_ = true;
            
            // 计算剩余数据的EMA
            for (size_t i = period_; i < data.size(); ++i) {
                current_value_ = alpha_ * data[i] + (1 - alpha_) * current_value_;
            }
        } else if (initialized_) {
            // 继续计算EMA
            for (double value : data) {
                current_value_ = alpha_ * value + (1 - alpha_) * current_value_;
            }
        } else {
            throw std::invalid_argument("Not enough data points for initialization");
        }
        
        return current_value_;
    }
    
    void reset() {
        initialized_ = false;
    }
    
private:
    size_t period_;
    double alpha_;
    double current_value_ = 0.0;
    bool initialized_;
};

// 相对强弱指标 (RSI)
class RSI {
public:
    explicit RSI(size_t period) : period_(period), initialized_(false) {
        if (period == 0) {
            throw std::invalid_argument("Period must be greater than 0");
        }
    }
    
    double calculate(double value) {
        if (!initialized_) {
            prev_value_ = value;
            initialized_ = true;
            return 50.0;  // 初始值
        }
        
        double change = value - prev_value_;
        prev_value_ = value;
        
        if (change > 0) {
            avg_gain_ = (avg_gain_ * (period_ - 1) + change) / period_;
            avg_loss_ = (avg_loss_ * (period_ - 1)) / period_;
        } else {
            avg_gain_ = (avg_gain_ * (period_ - 1)) / period_;
            avg_loss_ = (avg_loss_ * (period_ - 1) - change) / period_;
        }
        
        if (avg_loss_ == 0.0) {
            return 100.0;
        }
        
        double rs = avg_gain_ / avg_loss_;
        return 100.0 - (100.0 / (1.0 + rs));
    }
    
    double calculate(const std::vector<double>& data) {
        if (data.size() < period_ + 1) {
            throw std::invalid_argument("Not enough data points");
        }
        
        if (!initialized_) {
            // 初始化平均涨跌幅
            double total_gain = 0.0;
            double total_loss = 0.0;
            
            for (size_t i = 1; i <= period_; ++i) {
                double change = data[i] - data[i - 1];
                if (change > 0) {
                    total_gain += change;
                } else {
                    total_loss -= change;
                }
            }
            
            avg_gain_ = total_gain / period_;
            avg_loss_ = total_loss / period_;
            prev_value_ = data[period_];
            initialized_ = true;
            
            // 计算剩余数据的RSI
            for (size_t i = period_ + 1; i < data.size(); ++i) {
                double change = data[i] - data[i - 1];
                if (change > 0) {
                    avg_gain_ = (avg_gain_ * (period_ - 1) + change) / period_;
                    avg_loss_ = (avg_loss_ * (period_ - 1)) / period_;
                } else {
                    avg_gain_ = (avg_gain_ * (period_ - 1)) / period_;
                    avg_loss_ = (avg_loss_ * (period_ - 1) - change) / period_;
                }
            }
        } else {
            // 继续计算RSI
            for (size_t i = 1; i < data.size(); ++i) {
                double change = data[i] - data[i - 1];
                if (change > 0) {
                    avg_gain_ = (avg_gain_ * (period_ - 1) + change) / period_;
                    avg_loss_ = (avg_loss_ * (period_ - 1)) / period_;
                } else {
                    avg_gain_ = (avg_gain_ * (period_ - 1)) / period_;
                    avg_loss_ = (avg_loss_ * (period_ - 1) - change) / period_;
                }
            }
        }
        
        if (avg_loss_ == 0.0) {
            return 100.0;
        }
        
        double rs = avg_gain_ / avg_loss_;
        return 100.0 - (100.0 / (1.0 + rs));
    }
    
    void reset() {
        initialized_ = false;
        avg_gain_ = 0.0;
        avg_loss_ = 0.0;
    }
    
private:
    size_t period_;
    double avg_gain_ = 0.0;
    double avg_loss_ = 0.0;
    double prev_value_ = 0.0;
    bool initialized_;
};

} // namespace utils
} // namespace quant 