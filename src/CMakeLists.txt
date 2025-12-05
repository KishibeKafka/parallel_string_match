add_library(kmp kmp_matcher.cpp)

# 创建可执行文件目标
add_executable(matcher main.cpp)

target_link_libraries(matcher kmp)