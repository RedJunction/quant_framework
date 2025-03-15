cmake_minimum_required(VERSION 3.14)

project(quantframework 
    VERSION 0.1.0
    DESCRIPTION "C++ Quantitative Trading Framework"
    LANGUAGES CXX
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 查找线程库
find_package(Threads REQUIRED)

# 添加include目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 设置库的源文件
file(GLOB_RECURSE QUANT_SOURCES 
    "src/data/*.cpp"
    "src/strategy/*.cpp"
    "src/backtest/*.cpp"
    "src/risk/*.cpp"
    "src/execution/*.cpp"
    "src/analysis/*.cpp"
    "src/indicators/*.cpp"
    "src/utils/*.cpp"
)

# 创建库
add_library(quantframework ${QUANT_SOURCES})

# 设置库的链接选项
target_link_libraries(quantframework
    PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
)

# 添加示例目录
add_subdirectory(examples)

# 启用测试
option(BUILD_TESTING "Build the testing tree." ON)
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装目标
include(GNUInstallDirs)
install(TARGETS quantframework
    EXPORT quantframework-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# 安装头文件
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# 创建并安装配置文件
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/quantframework-config-version.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/quantframework-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/quantframework-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quantframework
)

install(EXPORT quantframework-targets
    FILE quantframework-targets.cmake
    NAMESPACE quantframework::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quantframework
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/quantframework-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/quantframework-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/quantframework
) 