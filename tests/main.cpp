#include "kmp_matcher.h"
#include "parallel_matcher.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <ctime>

namespace fs = std::filesystem; 

void Test_GetWitnessArray() {
   ParallelMatcher pm;
   pm.Test_GetWitnessArray();
}

void Test_Duel(){
   ParallelMatcher pm;
   pm.Test_Duel(1,2);
   pm.Test_Duel(3,4);
   pm.Test_Duel(5,6);
   pm.Test_Duel(7,8);
   pm.Test_Duel(9,10);
   pm.Test_Duel(11,12);
   pm.Test_Duel(13,14);
   pm.Test_Duel(15,16);
   pm.Test_Duel(1,4);
   pm.Test_Duel(6,8);
   pm.Test_Duel(9,11);
   pm.Test_Duel(14,16);

}

void Test_MatchNonPeriodic(){
   ParallelMatcher pm;
   pm.Test_MatchNonPeriodic();
}

void Test_ParallelMatch(){
   ParallelMatcher pm;
   pm.Test_ParallelMatch();
}

// 读取文本文件到字符串
bool readTextFile(const std::string& file_path, std::string& content) {
    std::ifstream file(file_path, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Error: 无法打开文本文件 " << file_path << std::endl;
        return false;
    }
    content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    file.close();
    return true;
}

// 读取二进制文件到char数组
bool readBinaryFile(const std::string& file_path, std::string& content) {
    std::ifstream file(file_path, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: 无法打开二进制文件 " << file_path << std::endl;
        return false;
    }
    // 获取文件大小
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    // 读取全部内容
    content.resize(file_size);
    file.read(content.data(), file_size);
    file.close();
    return true;
}

// 场景1：文档检索
void handleDocumentRetrieval(StringMatcher *matcher) {
    const std::string doc_path = std::string(DATA_PATH) + std::string("/document_retrieval/document.txt");
    const std::string target_path = std::string(DATA_PATH) + std::string("/document_retrieval/target.txt");
    // 读取文档内容
    std::string document;
    if (!readTextFile(doc_path, document)) {
        return;
    }
    // 读取模式串列表
    std::vector<std::string> patterns;
    std::ifstream target_file(target_path);
    if (!target_file.is_open()) {
        std::cerr << "Error: 无法打开模式串文件 " << target_path << std::endl;
        return;
    }
    std::string line;
    while (std::getline(target_file, line)) {
        if (!line.empty()) { // 跳过空行
            patterns.push_back(line);
        }
    }
    target_file.close();
    for (const auto& pattern : patterns) {
        std::vector<size_t> positions;
        matcher->match(document, pattern, positions);
    }
}

// 场景2：软件杀毒
void handleSoftwareAntivirus(StringMatcher *matcher) {
    const std::string virus_dir = std::string(DATA_PATH) + std::string("/software_antivirus/virus");
    const std::string scan_dir = std::string(DATA_PATH) + std::string("/software_antivirus/opencv-4.10.0");
    // 加载病毒库（文件名 -> 二进制数据）
    std::map<std::string, std::string> virus_map;
    try {
        for (const auto& entry : fs::directory_iterator(virus_dir)) {
            if (entry.is_regular_file()) {
                std::string virus_name = entry.path().filename().string();
                std::string virus_data;
                if (readBinaryFile(entry.path().string(), virus_data)) {
                    virus_map[virus_name] = virus_data;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: 访问病毒库目录失败 " << e.what() << std::endl;
        return;
    }

    if (virus_map.empty()) {
        std::cerr << "Error: 病毒库为空" << std::endl;
        return;
    }
    // 递归遍历待检测目录
    try {
        for (const auto& entry : fs::recursive_directory_iterator(scan_dir)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                std::string file_data;
                if (!readBinaryFile(file_path, file_data)) {
                    continue; // 跳过无法读取的文件
                }
                // 检测当前文件是否包含病毒
                std::set<std::string> detected_viruses;
                for (const auto& [virus_name, virus_data] : virus_map) {
                    std::vector<size_t> positions;
                    matcher->match(file_data, virus_data, positions);
                    if (!positions.empty()) {
                        detected_viruses.insert(virus_name);
                    }
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: 访问待检测目录失败 " << e.what() << std::endl;
        return;
    }
}

void Test_Time(){
   KMPMatcher kmp;
   ParallelMatcher pm;
   // 执行两个业务场景
   clock_t start, end;
   start = clock();
   handleDocumentRetrieval(&kmp);
   end = clock();
   double scene1, scene2;
   scene1 = ((double) (end - start)) / CLOCKS_PER_SEC;
   std::cout << "KMP场景1用时：" << scene1 << "s.\n";
   start = clock();
   handleSoftwareAntivirus(&kmp);
   end = clock();
   scene2 = ((double) (end - start)) / CLOCKS_PER_SEC;
   std::cout << "KMP场景2用时：" << scene2 << "s.\n";
   start = clock();
   handleDocumentRetrieval(&pm);
   end = clock();
   scene1 = ((double) (end - start)) / CLOCKS_PER_SEC;
   std::cout << "并行场景1用时：" << scene1 << "s.\n";
   start = clock();
   handleSoftwareAntivirus(&pm);
   end = clock();
   scene2 = ((double) (end - start)) / CLOCKS_PER_SEC;
   std::cout << "并行场景2用时：" << scene2 << "s.\n";
}

void test_omp(){
    #pragma omp parallel for if (1)
    for (int i = 0; i < 6; ++i)
    {
        std::cout << "Hello" << ", I am Thread " << omp_get_thread_num() << std::endl;
    }
}

int main() {
    Test_Time();
    // Test_MatchNonPeriodic();
    // Test_ParallelMatch();
    // Test_GetWitnessArray();
    return 0;
}