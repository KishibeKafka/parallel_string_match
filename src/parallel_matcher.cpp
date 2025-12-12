#include "parallel_matcher.h"

// 并行匹配
void ParallelMatcher::match(const std::string& text, const std::string& pattern, std::vector<size_t>& positions){
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
        std::string npp(pattern.begin(), pattern.begin()+periodic-1); // [1, periodic-1]
        std::vector<int> npwit = GetWitnessArray(npp);
        std::vector<size_t> temp;
        MatchNonPeriodic(text, npp, temp, npwit);
        // 判断剩余部分是否匹配
        #pragma omp parallel for if (temp.size() > 1000)
        for (int i = 0; i < temp.size(); ++i){
            int match = 1;
            for (int j = periodic-1; j < m; ++j){
                if (text[temp[i]+j] != pattern[j]){
                    match = 0;
                    break;
                }
            }
            if (match) {
                #pragma omp critical
                {
                    positions.push_back(temp[i]);
                }
            }
        }
    }
}

std::vector<int> ParallelMatcher::GetWitnessArray(const std::string& pattern){
    int m = pattern.size();
    int size = std::ceil(m / 2.0);
    
    std::vector<int> wit(size, 0);
    // 暴力算法 O(M^2)
    // for (int i = 2; i <= size; ++i){
    //     for (int j = i; j <= pattern.size(); ++j){
    //         if (pattern[j-1] != pattern[j-i]){
    //             wit[i-1] = j-i+1;
    //             break;
    //         }
    //     }
    // }

    // Z算法 O(M)
    std::vector<int> z(size, 0);
    int l = 0, r = 0;
    for (int i = 1; i < size; ++i) {
        if (i <= r && z[i - l] < r - i + 1) {
            z[i] = z[i - l];
        } else {
            z[i] = std::max(0, r - i + 1);
            while (i + z[i] < m && pattern[z[i]] == pattern[i + z[i]]) ++z[i];
        }
        if (i + z[i] - 1 > r) l = i, r = i + z[i] - 1;
    }
    // 将z函数转换为wit
    for (int k = 1; k < size; ++k) {
        if (k + z[k] < m) {
            // 发生了不匹配
            wit[k] = z[k] + 1;
        } else {
            // k + z[k] == m，说明匹配到了模式串末尾，Witness 为 0
            wit[k] = 0;
        }
    }
    return wit;
}

int ParallelMatcher::Duel(int i, int j, const std::string& z, const std::string& y, std::vector<int>& witness){
    // j - i + 1 不能超过wit大小
    int k = witness[j-i];
    return z[j+k-2] != y[k-1] ? i : j;
}

void ParallelMatcher::MatchNonPeriodic(const std::string& text, const std::string& pattern, std::vector<size_t>& positions,  std::vector<int>& wit){
    int n = text.size();
    int m = pattern.size();
    std::vector<int> winners(n-m+1, 0);
    for (int i = 0; i < winners.size(); ++i) winners[i] = i+1; // 胜者初始化
    int d = wit.size();
    // 在d个下标中决出胜者
    #pragma omp parallel for if (n-m > 1000)
    for (int i = 0; i <= (n-m+1)/d; ++i){
        for (int j = 1; j < d; ++j){
            if (i*d+j < n-m+1)
                winners[i*d] = Duel(winners[i*d], winners[i*d+j], text, pattern, wit);
        }
    }
    // 判断是否匹配
    std::vector<size_t> temp;
    int i = 1;
    while (i <= n-m+1){
        temp.push_back(winners[i-1]);
        i += d;
    }
    #pragma omp parallel for if (temp.size() > 1000)
    for (i = 0; i < temp.size(); ++i){
        int match = 1;
        for (int j = 0; j < m; ++j){
            if (text[temp[i]+j-1] != pattern[j]){
                match = 0;
                break;
            }
        }
        if (match) {
            #pragma omp critical
            {
                positions.push_back(temp[i]-1);
            }
        }
    }
}

int ParallelMatcher::GetPeriodicIndex(std::vector<int>& wit){
    for (int i = 2; i <= wit.size(); ++i){
        if (wit[i-1] == 0) return i;
    }
    return 0;
}

void ParallelMatcher::Test_GetWitnessArray(){
    std::cout << "Testing Witness Array.\n";
    
    const std::string p = "abaababa";
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
    const std::string t = "abaababaababaababaababa";
    const std::string p= "abaababa";
    std::vector<int> wit = GetWitnessArray(p);
    int winner = Duel(i, j, t, p, wit);
    std::cout << "Winner of " << i << " and " << j << " is: " << winner << '\n';
}

void ParallelMatcher::Test_MatchNonPeriodic(){
    std::cout << "Testing Match Nonperiodic.\n";
    const std::string t = "abaababaababaababaababa";
    const std::string p = "abaababa";
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
    const std::string t = "abaababaababaababaababa";
    const std::string p = "ababa";
    std::vector<size_t> positions;
    match(t, p, positions);
    for (int i = 0; i < positions.size(); ++i){
        std::cout << positions[i] << ", ";
    }
    std::cout << '\n';
}

