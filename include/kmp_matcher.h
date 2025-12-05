#pragma once
#include "string_matcher.h"
#include <vector>
#include <string>

// KMP算法匹配器（子类）
class KMPMatcher : public StringMatcher {
public:
    // 实现文本匹配接口
    void match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) override;

    // 实现二进制匹配接口
    void match(const std::vector<char>& binary_data, const std::vector<char>& pattern_data, std::vector<size_t>& positions) override;

private:
    // 构建KMP前缀函数（next数组），模板支持文本/二进制
    template <typename T>
    void buildNext(const std::vector<T>& pattern, std::vector<int>& next);

    // 通用匹配逻辑（模板），避免文本/二进制代码重复
    template <typename T>
    void matchGeneric(const std::vector<T>& text, const std::vector<T>& pattern, std::vector<size_t>& positions);
};