#include "money_file.h"
#include <fstream>

static void writeString(std::ofstream& out, const std::string& str) {
    size_t len = str.length();
    out.write(reinterpret_cast<const char*>(&len), sizeof(len));
    if (len > 0) out.write(str.data(), len);
}

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

template<typename T>
static void writeData(std::ofstream& out, const T& data) {
    out.write(reinterpret_cast<const char*>(&data), sizeof(T));
}

template<typename T>
static T readData(std::ifstream& in) {
    T data;
    in.read(reinterpret_cast<char*>(&data), sizeof(T));
    return data;
}

OpResult saveMoney(const Money& money) {
    std::ofstream file(Config::MONEY_FILE_PATH, std::ios::binary | std::ios::app);
    if (!file) return OpResult::Failed;
    
    writeString(file, money.aCardName);
    writeData(file, money.tTime);
    writeData(file, money.nStatus);
    writeData(file, money.fMoney);
    writeData(file, money.nDel);
    file.close();
    return OpResult::Success;
}

std::vector<Money> loadAllMoneyRecords() {
    std::vector<Money> records;
    std::ifstream file(Config::MONEY_FILE_PATH, std::ios::binary);
    if (!file) return records;

    while (file.peek() != EOF) {
        Money m;
        m.aCardName = readString(file);
        m.tTime = readData<time_t>(file);
        m.nStatus = readData<MoneyStatus>(file);
        m.fMoney = readData<float>(file);
        m.nDel = readData<DeleteStatus>(file);
        if (file.eof()) break;
        if (!m.aCardName.empty()) records.push_back(m);
    }
    return records;
}