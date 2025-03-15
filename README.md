# QuantFramework - C++量化交易框架

QuantFramework是一个高性能的C++量化交易框架，专为算法交易、回测和实时交易而设计。该框架提供了一套完整的工具，用于开发、测试和部署交易策略。

## 特点

- **高性能**：使用现代C++17编写，注重性能优化
- **模块化设计**：各组件松耦合，易于扩展和定制
- **完整的回测系统**：支持历史数据回测，包含详细的性能分析
- **技术指标库**：内置常用技术分析指标
- **风险管理**：提供风险控制和资金管理工具
- **数据处理**：支持多种数据源和格式

## 系统要求

- C++17兼容的编译器（GCC 7+, Clang 5+, MSVC 2019+）
- CMake 3.14或更高版本
- 支持的操作系统：Linux, macOS, Windows

## 快速开始

### 构建项目

```bash
mkdir build && cd build
cmake ..
make
```

### 运行示例

```bash
./examples/simple_backtest
```

## 框架结构

QuantFramework由以下主要模块组成：

- **数据模块**：负责数据获取、处理和存储
- **策略模块**：提供策略开发接口和常用策略模板
- **指标模块**：实现各种技术分析指标
- **回测模块**：提供历史数据回测功能
- **执行模块**：处理订单执行和交易管理
- **分析模块**：计算和展示性能指标
- **风险模块**：实现风险控制和资金管理

## 开发自定义策略

以下是创建自定义策略的基本步骤：

1. 继承`Strategy`基类
2. 实现`initialize()`方法进行初始化
3. 实现`on_data()`方法处理市场数据并生成交易信号

```cpp
class MyStrategy : public quant::strategy::Strategy {
public:
    void initialize() override {
        // 初始化策略
    }

    std::optional<quant::strategy::Signal> on_data(const quant::data::MarketData& data) override {
        // 处理数据并生成信号
        if (/* 买入条件 */) {
            quant::strategy::Signal signal;
            signal.type = quant::strategy::SignalType::BUY;
            signal.symbol = data.symbol;
            signal.timestamp = data.timestamp;
            return signal;
        }
        return std::nullopt;
    }
};
```

## 回测策略

使用`BacktestEngine`对策略进行回测：

```cpp
// 创建数据源
auto data_feed = std::make_shared<CSVDataFeed>("data.csv");

// 创建策略
auto strategy = std::make_shared<MyStrategy>();

// 设置回测配置
quant::backtest::BacktestConfig config;
config.initial_capital = 100000.0;
config.commission_rate = 0.001;
config.start_time = /* 开始时间 */;
config.end_time = /* 结束时间 */;

// 创建回测引擎
quant::backtest::BacktestEngine engine(data_feed, strategy, config);

// 运行回测
engine.run();

// 获取回测结果
auto report = engine.get_performance_report();
```

## 性能分析

回测完成后，可以获取详细的性能报告：

```cpp
auto report = engine.get_performance_report();
std::cout << "Total Return: " << report.total_return * 100 << "%\n";
std::cout << "Sharpe Ratio: " << report.sharpe_ratio << "\n";
std::cout << "Max Drawdown: " << report.max_drawdown * 100 << "%\n";
```

## 贡献

欢迎贡献代码、报告问题或提出改进建议。请遵循以下步骤：

1. Fork项目
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 提交更改 (`git commit -m 'Add some amazing feature'`)
4. 推送到分支 (`git push origin feature/amazing-feature`)
5. 创建Pull Request

## 许可证

本项目采用MIT许可证 - 详情请参阅[LICENSE](LICENSE)文件。