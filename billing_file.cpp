#include "billing_file.h"
#include "billing_service.h" // 为了访问 g_billingList
#include <fstream>

// --- 序列化辅助函数 ---
// 安全写入 std::string
static void writeString(std::ofstream& out, const std::string& str) {
    size_t len = str.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) {
        out.write(str.data(), len);
    }
}

// 安全写入基础数据类型
template<typename T>
static void writeData(std::ofstream& out, const T& data) {
    out.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

// 序列化单条计费记录
static void writeBillingData(std::ofstream& out, const Billing& billing) {
    writeString(out, billing.aCardName);
    writeData(out, billing.tStart);
    writeData(out, billing.tEnd);
    writeData(out, billing.fAmount);
    writeData(out, billing.nStatus);
    writeData(out, billing.nDel);
}

// --- 反序列化辅助函数 ---
// 安全读取 std::string
static std::string readString(std::ifstream& in) {
    size_t len = 0;
    in.read(reinterpret_cast<char*>(&len), sizeof(len));
    if (in && len > 0 && len < 10000) { 
        std::string str(len, '\0');
        in.read(&str[0], len);
        return str;
    }
    return "";
}

// 安全读取基础数据类型
template<typename T>
static T readData(std::ifstream& in) {
    T data;
    in.read(reinterpret_cast<char*>(&data), sizeof(T));
    return data;
}

// 反序列化单条计费记录
static Billing readBillingData(std::ifstream& in) {
    Billing billing;
    billing.aCardName = readString(in);
    billing.tStart = readData<time_t>(in);
    billing.tEnd = readData<time_t>(in);
    billing.fAmount = readData<float>(in);
    billing.nStatus = readData<BillingStatus>(in);
    billing.nDel = readData<DeleteStatus>(in);
    return billing;
}

// ---------------- 核心接口实现 ----------------

// 加载所有计费记录
int loadBillings() {
    g_billingList.clear(); // 清空当前内存
    std::ifstream file(Config::BILLING_FILE_PATH, std::ios::binary);
    if (!file) return 0;

    int count = 0;
    while (file.peek() != EOF) {
        Billing billing = readBillingData(file);
        if (file.eof()) break; 
        
        if (!billing.aCardName.empty()) {
            g_billingList.push_back(billing);
            count++;
        }
    }
    file.close();
    return count;
}

// 追加单条计费记录
OpResult saveBilling(const Billing& billing) {
    std::ofstream file(Config::BILLING_FILE_PATH, std::ios::binary | std::ios::app);
    if (!file) return OpResult::Failed;
    
    writeBillingData(file, billing);
    file.close();
    return OpResult::Success;
}

// 全量覆写更新文件
OpResult updateBillingToFile() {
    std::ofstream file(Config::BILLING_FILE_PATH, std::ios::binary | std::ios::trunc);
    if (!file) return OpResult::Failed;

    for (const auto& billing : g_billingList) {
        writeBillingData(file, billing);
    }
    file.close();
    return OpResult::Success;
}