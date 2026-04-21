#include "card_file.h"
#include "card_service.h" // 为了访问 g_cardList
#include <fstream>
#include <iostream>

// --- 序列化辅助函数 ---
// 将 std::string 安全地写入二进制文件：先写长度，再写内容
static void writeString(std::ofstream& out, const std::string& str) {
    size_t len = str.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        out.write(str.data(), len);
    }
}

// 将其他基础数据类型安全写入
template<typename T>
static void writeData(std::ofstream& out, const T& data) {
    out.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

// 序列化一整张卡片
static void writeCardData(std::ofstream& out, const Card& card) {
    writeString(out, card.aName);
    writeString(out, card.aPwd);
    writeData(out, card.nStatus);
    writeData(out, card.tStart);
    writeData(out, card.tEnd);
    writeData(out, card.fTotalUse);
    writeData(out, card.tLast);
    writeData(out, card.nUseCount);
    writeData(out, card.fBalance);
    writeData(out, card.nDel);
}

// --- 反序列化辅助函数 ---
// 从二进制文件安全地读取 std::string
static std::string readString(std::ifstream& in) {
    size_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    // 增加安全性校验，防止读到脏数据导致分配超大内存
    if (in && len > 0 && len < 10000) { 
        std::string str(len, '\0');
        in.read(&str[0], len);
        return str;
    }
    return "";
}

// 读取基础数据类型
template<typename T>
static T readData(std::ifstream& in) {
    T data;
    in.read(reinterpret_cast<char*>(&data), sizeof(T));
    return data;
}

// 反序列化一整张卡片
static Card readCardData(std::ifstream& in) {
    Card card;
    card.aName = readString(in);
    card.aPwd = readString(in);
    card.nStatus = readData<CardStatus>(in);
    card.tStart = readData<time_t>(in);
    card.tEnd = readData<time_t>(in);
    card.fTotalUse = readData<float>(in);
    card.tLast = readData<time_t>(in);
    card.nUseCount = readData<int>(in);
    card.fBalance = readData<float>(in);
    card.nDel = readData<DeleteStatus>(in);
    return card;
}

// ---------------- 核心接口实现 ----------------

// 从文件加载所有卡信息
int getCard() {
    g_cardList.clear(); // 清空当前内存
    std::ifstream file(Config::CARD_FILE_PATH, std::ios::binary);
    if (!file) return 0; // 文件不存在说明是第一次启动

    int count = 0;
    while (file.peek() != EOF) {
        Card card = readCardData(file);
        if (file.eof()) break; // 防护：如果读取中途遇到文件尾则终止
        
        if (!card.aName.empty()) {
            g_cardList.push_back(card);
            count++;
        }
    }
    file.close();
    return count;
}

// 追加保存单条卡片
OpResult saveCard(const Card& card) {
    std::ofstream file(Config::CARD_FILE_PATH, std::ios::binary | std::ios::app);
    if (!file) return OpResult::Failed;
    
    writeCardData(file, card);
    file.close();
    return OpResult::Success;
}

// 更新文件（全量覆盖保存）
OpResult updateCardToFile() {
    // ios::trunc 模式会清空原有文件重新写入
    std::ofstream file(Config::CARD_FILE_PATH, std::ios::binary | std::ios::trunc);
    if (!file) return OpResult::Failed;

    for (const auto& card : g_cardList) {
        writeCardData(file, card);
    }
    file.close();
    return OpResult::Success;
}