#pragma once

#include <string>
#include <vector>

// 字符串匹配算法基类（抽象类）
class StringMatcher {
public:
    virtual ~StringMatcher() = default;

    virtual void match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) = 0;
};