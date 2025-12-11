find_package(OpenMP REQUIRED)

# 创建可执行文件目标
add_executable(test main.cpp)

target_link_libraries(test
PUBLIC
parallel
OpenMP::OpenMP_CXX
)