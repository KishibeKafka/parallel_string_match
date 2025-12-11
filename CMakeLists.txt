cmake_minimum_required(VERSION 3.10)   # 指定最低 CMake 版本

project(ParallelStringMatch VERSION 1.0)          # 定义项目名称和版本

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DATA_PATH "/root/ParallelComputing/data")

# 添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)