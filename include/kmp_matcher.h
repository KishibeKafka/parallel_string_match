#pragma once
#include "string_matcher.h"
#include <vector>
#include <string>

// KMP算法匹配器（子类）
class KMPMatcher : public StringMatcher {
public:
    // 实现文本匹配接口
    void match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) override;

private:
    // 构建KMP前缀函数（next数组）
    void buildNext(const std::string& pattern, std::vector<int>& next);
};