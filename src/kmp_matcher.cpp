#include "kmp_matcher.h"
#include <algorithm>

// 构建KMP前缀函数（next数组）
template <typename T>
void KMPMatcher::buildNext(const std::vector<T>& pattern, std::vector<int>& next) {
    size_t pattern_len = pattern.size();
    next.resize(pattern_len);
    next[0] = -1; // 初始值：无匹配前缀
    int j = -1;   // 前缀指针

    for (size_t i = 1; i < pattern_len; ++i) {
        // 回退前缀指针到匹配位置
        while (j >= 0 && pattern[i] != pattern[j + 1]) {
            j = next[j];
        }
        // 前缀匹配成功，指针前进
        if (pattern[i] == pattern[j + 1]) {
            ++j;
        }
        next[i] = j;
    }
}

// 通用KMP匹配逻辑（模板）
template <typename T>
void KMPMatcher::matchGeneric(const std::vector<T>& text, const std::vector<T>& pattern, std::vector<size_t>& positions) {
    positions.clear();
    size_t text_len = text.size();
    size_t pattern_len = pattern.size();

    // 边界条件：模式串为空或文本更短，直接返回
    if (pattern_len == 0 || text_len < pattern_len) {
        return;
    }

    // 构建前缀函数
    std::vector<int> next;
    buildNext(pattern, next);

    int j = -1; // 模式串指针
    for (size_t i = 0; i < text_len; ++i) {
        // 匹配失败时，通过next数组回退模式串指针
        while (j >= 0 && text[i] != pattern[j + 1]) {
            j = next[j];
        }
        // 匹配成功，指针前进
        if (text[i] == pattern[j + 1]) {
            ++j;
        }
        // 完全匹配，记录位置（模式串首字符索引）
        if (static_cast<size_t>(j) == pattern_len - 1) {
            positions.push_back(i - pattern_len + 1);
            j = next[j]; // 继续匹配下一个可能位置
        }
    }
}

// 文本匹配实现（转换为char数组调用通用逻辑）
void KMPMatcher::match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) {
    std::vector<char> text_vec(text.begin(), text.end());
    std::vector<char> pattern_vec(pattern.begin(), pattern.end());
    matchGeneric(text_vec, pattern_vec, positions);
}

// 二进制匹配实现（直接调用通用逻辑）
void KMPMatcher::match(const std::vector<char>& binary_data, const std::vector<char>& pattern_data, std::vector<size_t>& positions) {
    matchGeneric(binary_data, pattern_data, positions);
}

// 显式实例化模板（避免链接错误）
template void KMPMatcher::buildNext<char>(const std::vector<char>&, std::vector<int>&);
template void KMPMatcher::matchGeneric<char>(const std::vector<char>&, const std::vector<char>&, std::vector<size_t>&);