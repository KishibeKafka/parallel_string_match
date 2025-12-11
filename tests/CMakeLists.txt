find_package(OpenMP REQUIRED)

# 创建可执行文件目标
add_executable(test main.cpp)

target_link_libraries(test
PUBLIC
kmp
parallel
OpenMP::OpenMP_CXX
)

target_compile_definitions(test PRIVATE DATA_PATH=\"${DATA_PATH}\")