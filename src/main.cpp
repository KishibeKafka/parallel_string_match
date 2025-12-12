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

namespace fs = std::filesystem; // 目录遍历需C++17

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

// 读取二进制文件
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
    const std::string result_path = "../result_document.txt"; // 结果文件输出到项目根目录

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

    std::ofstream result_file(result_path, std::ios::out | std::ios::trunc);
    if (!result_file.is_open()) {
        std::cerr << "Error: 无法创建结果文件 " << result_path << std::endl;
        return;
    }

    // 逐个匹配模式串并输出结果
    for (const auto& pattern : patterns) {
        std::vector<size_t> positions;
        matcher->match(document, pattern, positions);
        // 输出格式：次数 位置1 位置2 ...
        result_file << positions.size();
        for (size_t pos : positions) {
            result_file << " " << pos;
        }
        result_file << std::endl;
    }

    result_file.close();
    std::cout << "场景1完成：结果已保存至 " << result_path << std::endl;
}

// 场景2：软件杀毒
void handleSoftwareAntivirus(StringMatcher *matcher) {
    const std::string virus_dir = std::string(DATA_PATH) + std::string("/software_antivirus/virus");
    const std::string scan_dir = std::string(DATA_PATH) + std::string("/software_antivirus/opencv-4.10.0");
    const std::string result_path = "../result_software.txt"; // 结果文件输出到项目根目录

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

    std::map<std::string, std::set<std::string>> scan_results; // 文件路径 -> 病毒名集合

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

                // 记录检测结果
                if (!detected_viruses.empty()) {
                    scan_results[file_path] = detected_viruses;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Error: 访问待检测目录失败 " << e.what() << std::endl;
        return;
    }

    // 写入杀毒结果
    std::ofstream result_file(result_path, std::ios::out | std::ios::trunc);
    if (!result_file.is_open()) {
        std::cerr << "Error: 无法创建杀毒结果文件 " << result_path << std::endl;
        return;
    }

    for (const auto& [file_path, viruses] : scan_results) {
        result_file << file_path;
        for (const auto& virus : viruses) {
            result_file << " " << virus;
        }
        result_file << std::endl;
    }

    result_file.close();
    std::cout << "场景2完成：结果已保存至 " << result_path << std::endl;
}

int main() {
    ParallelMatcher pm;
    // KMPMatcher pm;
    // 执行两个业务场景
    std::cout << "开始执行两个场景" << '\n';
    clock_t start, end;
    start = clock();
    handleDocumentRetrieval(&pm);
    end = clock();
    double scene1 = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "场景1用时：" << scene1 << "s.\n";
    start = clock();
    handleSoftwareAntivirus(&pm);
    end = clock();
    double scene2 = ((double) (end - start)) / CLOCKS_PER_SEC;
    std::cout << "场景2用时：" << scene2 << "s.\n";
    return 0;
}