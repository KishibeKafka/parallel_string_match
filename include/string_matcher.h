#pragma once

#include <string>
#include <vector>

// 字符串匹配算法基类（抽象类）
class StringMatcher {
public:
    virtual ~StringMatcher() = default; // 虚析构函数，确保子类析构正确

    // 文本字符串匹配：在text中匹配pattern，返回所有匹配位置（首字符索引）
    virtual void match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) = 0;

    // 二进制数据匹配：在binary_data中匹配pattern_data，返回所有匹配位置
    virtual void match(const std::vector<char>& binary_data, const std::vector<char>& pattern_data, std::vector<size_t>& positions) = 0;
};