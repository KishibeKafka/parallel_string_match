#include "parallel_matcher.h"

// 文本匹配
void ParallelMatcher::match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions){
    std::vector<char> text_vec(text.begin(), text.end());
    std::vector<char> pattern_vec(pattern.begin(), pattern.end());
    ParallelMatch(text_vec, pattern_vec, positions);
}

// 二进制匹配
void ParallelMatcher::match(const std::vector<char>& binary_data, const std::vector<char>& pattern_data, std::vector<size_t>& positions){
    ParallelMatch(binary_data, pattern_data, positions);
}

template <typename T>
std::vector<int> ParallelMatcher::GetWitnessArray(const std::vector<T>& pattern){
    int size = std::ceil(pattern.size() / 2.0);
    std::vector<int> wit(size, 0);
    #pragma omp parallel for
    for (int i = 2; i <= size; ++i){
        for (int j = i; j <= pattern.size(); ++j){
            if (pattern[j-1] != pattern[j-i]){
                wit[i-1] = j-i+1;
                break;
            }
        }
    }
    return wit;
}

template <typename T>
int ParallelMatcher::Duel(int i, int j, const std::vector<T>& z, const std::vector<T>& y, std::vector<int>& witness){
    // j - i + 1 不能超过wit大小
    int k = witness[j-i];
    return z[j+k-2] != y[k-1] ? i : j;
}

template <typename T>
void ParallelMatcher::ParallelMatch(const std::vector<T>& text, const std::vector<T>& pattern, std::vector<size_t>& positions){
    positions.clear();
    size_t n = text.size();
    size_t m = pattern.size();
    if (m == 0 || n < m) {
        return;
    }

    std::vector<int> wit = GetWitnessArray(pattern);
    int periodic = GetPeriodicIndex(wit);
    if (!periodic){
        // 非周期串
        MatchNonPeriodic(text, pattern, positions, wit);
    } else{
        // 周期串
        std::vector<T> npp(pattern.begin(), pattern.begin()+periodic-1); // [1, periodic-1]
        std::vector<int> npwit = GetWitnessArray(npp);
        std::vector<size_t> temp;
        MatchNonPeriodic(text, npp, temp, npwit);
        // 判断剩余部分是否匹配
        #pragma omp parallel for
        for (int i = 0; i < temp.size(); ++i){
            int match = 1;
            for (int j = periodic-1; j < m; ++j){
                if (text[temp[i]+j] != pattern[j]){
                    match = 0;
                    break;
                }
            }
            if (match) positions.push_back(temp[i]);
        }
    }
}

template <typename T>
void ParallelMatcher::MatchNonPeriodic(const std::vector<T>& text, const std::vector<T>& pattern, std::vector<size_t>& positions, std::vector<int>& wit){
    int n = text.size();
    int m = pattern.size();
    int range = wit.size() < n-m+1 ? wit.size() : n-m+1;
    std::vector<int> winners(n-m+1, 0);
    std::vector<int> jump(n-m+1, 1); //最终胜者跳转
    int d = 1;
    // 在d个下标中决出胜者
    while(d <= range){
        #pragma omp parallel for
        for (int i = 0; i < (n-m+1)/d; ++i){
            if (d == 1){
                winners[i] = i+1; //胜者初始化
            } else{
                // 两组中的胜者对决
                // 结果存到首元素
                winners[i*d] = Duel(winners[i*d], winners[i*d+d/2], text, pattern, wit);
                jump[i*d] = d;
            }
        }
        d <<= 1;
    } 
    // 判断是否匹配
    std::vector<size_t> temp;
    int i = 1;
    while (i <= n-m+1){
        temp.push_back(winners[i-1]);
        i += jump[i-1];
    }
    #pragma omp parallel for
    for (i = 0; i < temp.size(); ++i){
        int match = 1;
        for (int j = 0; j < m; ++j){
            if (text[temp[i]+j-1] != pattern[j]){
                match = 0;
                break;
            }
        }
        if (match) positions.push_back(temp[i]-1);
    }
}

int ParallelMatcher::GetPeriodicIndex(std::vector<int>& wit){
    for (int i = wit.size(); i >= 2; --i){
        if (wit[i-1] == 0) return i;
    }
    return 0;
}


void ParallelMatcher::Test_GetWitnessArray(){
    std::cout << "Testing Witness Array.\n";
    
    const std::string pattern = "abababa";
    std::vector<char> p(pattern.begin(), pattern.end());
    std::vector<int> wit = GetWitnessArray(p);
    std::cout << "size: " << wit.size() << '\n';
    std::cout << "Witness Array result: ";
    for (int val : wit) {
        std::cout << val << " ";
    }
    std::cout << '\n';
}

void ParallelMatcher::Test_Duel(int i, int j){
    std::cout << "Testing Duel.\n";
    const std::string text = "abaababaababaababaababa";
    const std::string pattern = "abaababa";
    std::vector<char> t(text.begin(), text.end());
    std::vector<char> p(pattern.begin(), pattern.end());
    std::vector<int> wit = GetWitnessArray(p);
    int winner = Duel(i, j, t, p, wit);
    std::cout << "Winner of " << i << " and " << j << " is: " << winner << '\n';
}

void ParallelMatcher::Test_MatchNonPeriodic(){
    std::cout << "Testing Match Nonperiodic.\n";
    const std::string text = "abaababaababaababaababa";
    const std::string pattern = "abaababa";
    std::vector<char> t(text.begin(), text.end());
    std::vector<char> p(pattern.begin(), pattern.end());
    std::vector<int> wit = GetWitnessArray(p);
    std::vector<size_t> positions;
    MatchNonPeriodic(t, p, positions, wit);
    for (int i = 0; i < positions.size(); ++i){
        std::cout << positions[i] << ", ";
    }
    std::cout << '\n';
}

void ParallelMatcher::Test_ParallelMatch(){
    std::cout << "Testing Parallel Match.\n";                       
    const std::string text = "abaababaababaababaababa";
    const std::string pattern = "ababa";
    std::vector<char> t(text.begin(), text.end());     
    std::vector<char> p(pattern.begin(), pattern.end());
    std::vector<size_t> positions;
    ParallelMatch(t, p, positions);
    for (int i = 0; i < positions.size(); ++i){
        std::cout << positions[i] << ", ";
    }
    std::cout << '\n';
}

