find_package(OpenMP REQUIRED)

add_library(kmp kmp_matcher.cpp)
add_library(parallel parallel_matcher.cpp)


# 创建可执行文件目标
add_executable(matcher main.cpp)

target_link_libraries(matcher
PUBLIC
kmp
parallel
OpenMP::OpenMP_CXX
)

target_compile_definitions(matcher PRIVATE DATA_PATH=\"${DATA_PATH}\")