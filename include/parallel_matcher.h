#pragma once
#include "string_matcher.h"
#include <vector>
#include <string>
#include <math.h>
#include <omp.h>
#include <iostream>
#include <map>

class ParallelMatcher : public StringMatcher{
public:
    void match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions) override;

private:
    std::map<std::string, std::vector<int>> wit_map;
    std::vector<int> GetWitnessArray(const std::string& pattern);
    int Duel(int i, int j, const std::string& z, const std::string& y, std::vector<int>& witness);
    void MatchNonPeriodic(const std::string& text, const std::string& pattern, std::vector<size_t>& positions,  std::vector<int>& wit);
    void MatchPeriodic(const std::string& text, const std::string& pattern, std::vector<size_t>& positions,  std::vector<int>& wit, int periodic);
    int GetPeriodicIndex(std::vector<int>& wit);

public:
    void Test_GetWitnessArray();
    void Test_Duel(int i, int j);
    void Test_MatchNonPeriodic();
    void Test_ParallelMatch();
};