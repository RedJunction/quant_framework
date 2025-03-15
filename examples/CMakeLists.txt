# 添加示例程序
add_executable(simple_ma_strategy simple_ma_strategy.cpp)
target_link_libraries(simple_ma_strategy PRIVATE quantframework)

# 安装示例
install(TARGETS simple_ma_strategy
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)

# 添加简单回测示例
add_executable(simple_backtest simple_backtest.cpp)
target_link_libraries(simple_backtest PRIVATE quantframework)

# 安装示例
install(TARGETS simple_backtest
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}/examples
)

# 复制示例数据文件（如果有）
# install(FILES data/sample_data.csv
#     DESTINATION ${CMAKE_INSTALL_BINDIR}/examples/data
# ) 