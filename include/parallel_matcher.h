#pragma once
#include "string_matcher.h"
#include <vector>
#include <string>
#include <math.h>
#include <omp.h>
#include <iostream>

class ParallelMatcher : public StringMatcher{
public:
    void match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) override;
    void match(const std::vector<char>& binary_data, const std::vector<char>& pattern_data, std::vector<size_t>& positions) override;
private:
    template <typename T>
    std::vector<int> GetWitnessArray(const std::vector<T>& pattern);
    template <typename T>
    int Duel(int i, int j, const std::vector<T>& z, const std::vector<T>& y, std::vector<int>& witness);
    template <typename T>
    void ParallelMatch(const std::vector<T>& text, const std::vector<T>& pattern, std::vector<size_t>& positions);
    template <typename T>
    void MatchNonPeriodic(const std::vector<T>& text, const std::vector<T>& pattern, std::vector<size_t>& positions,  std::vector<int>& wit);

public:
    void Test_GetWitnessArray();
    void Test_Duel(int i, int j);
    void Test_MatchNonPeriodic();
};