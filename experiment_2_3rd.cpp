#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

using namespace std;

// 解析CSV行
vector<string> parseCSVLine(const string& line) {
    vector<string> fields;
    string field;
    bool inQuotes = false;

    for (char c : line) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field += c;
        }
    }
    fields.push_back(field);
    return fields;
}

// 提取IPC代码（假设前两个字符是["，然后取4个字符）
string extractIPC(const string& ipcField) {
    // 直接从索引2开始取4个字符
    if (ipcField.length() >= 6) {
        return ipcField.substr(1, 4);
    }
    return "";
}

int main() {
    string filePath = "E:\\InnovationDataset\\DeepInnovationAI\\DeepPatentAI_1000.csv";

    cout << "输入需要分析的年份（用空格分隔）" << endl;
    cout << "年份：";
    string yearInput;
    getline(cin, yearInput);

    istringstream iss(yearInput);
    vector<int> years;
    int year;
    while (iss >> year) {
        years.push_back(year);
    }

    vector<string> techFields = {"G06K", "F41G", "G10F"};

    map<int, map<string, int>> matrix;

    // 初始化矩阵
    for (int y : years) {
        for (const string& field : techFields) {
            matrix[y][field] = 0;
        }
    }

    // 调试：记录找到的IPC代码
    map<string, int> ipcDebug;
    int totalLines = 0;
    int yearMatches = 0;
    int noveltyMatches = 0;
    int validRecords = 0;

    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件！" << endl;
        return 1;
    }

    string line;
    getline(file, line);  // 跳过表头

    cout << "\n正在处理数据...\n" << endl;

    while (getline(file, line)) {
        totalLines++;

        vector<string> fields = parseCSVLine(line);
        if (fields.size() < 9) continue;

        // 提取年份
        int recordYear = stoi(fields[6]);

        // 检查年份
        if (find(years.begin(), years.end(), recordYear) == years.end()) {
            continue;
        }
        yearMatches++;

        // 提取IPC
        string ipc = extractIPC(fields[3]);

        // 调试：记录所有IPC
        ipcDebug[ipc]++;

        // 提取Novelty
        double novelty = stod(fields[8]);

        if (novelty > 0.8) {
            noveltyMatches++;

            // 检查IPC是否匹配
            if (find(techFields.begin(), techFields.end(), ipc) != techFields.end()) {
                matrix[recordYear][ipc]++;
                validRecords++;
            }
        }

        // 每10000行输出一次进度
        if (totalLines % 10000 == 0) {
            cout << "已处理 " << totalLines << " 行..." << endl;
        }
    }

    file.close();

    cout << "\n========== 调试信息 ==========" << endl;
    cout << "总行数: " << totalLines << endl;
    cout << "匹配年份的记录数: " << yearMatches << endl;
    cout << "Novelty > 0.8 的记录数: " << noveltyMatches << endl;
    cout << "最终有效记录数: " << validRecords << endl;

    cout << "\n找到的IPC代码（前20个）：" << endl;
    int count = 0;
    for (auto& p : ipcDebug) {
        cout << p.first << ": " << p.second << " 次" << endl;
        if (++count >= 20) break;
    }

    // 显示矩阵
    cout << "\n========== 技术热度矩阵 ==========" << endl;
    cout << left << setw(10) << "年份";
    for (const string& field : techFields) {
        cout << setw(10) << field;
    }
    cout << endl;

    for (int y : years) {
        cout << left << setw(10) << y;
        for (const string& field : techFields) {
            cout << setw(10) << matrix[y][field];
        }
        cout << endl;
    }

    // 增长分析
    cout << "\n========== 增长分析 ==========" << endl;
    for (const string& field : techFields) {
        int firstCount = matrix[years.front()][field];
        int lastCount = matrix[years.back()][field];
        int growth = lastCount - firstCount;

        cout << field << ": " << firstCount << " -> " << lastCount
             << " (+" << growth << ")" << endl;
    }

    return 0;
}

