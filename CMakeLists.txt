cmake_minimum_required(VERSION 3.10)   # 指定最低 CMake 版本

project(ParallelStringMatch VERSION 1.0)          # 定义项目名称和版本

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 添加源文件
# add_library(MyLib src/mylib.cpp)        # 创建一个库目标 MyLib
add_executable(matcher src/main.cpp)  # 创建一个可执行文件目标 matcher

# 链接库到可执行文件
# target_link_libraries(matcher MyLib)