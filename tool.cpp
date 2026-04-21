 #include "tool.h"
#include <iostream>
#include <iomanip>
#include <sstream>

time_t getCurrentTime() {
    return std::time(nullptr);
}

std::string formatTime(time_t t) {
    if (t == 0) return "暂无时间";
    struct tm* localTime = std::localtime(&t);
    std::stringstream timeStream;
    timeStream << std::setfill('0') << std::setw(4) << (localTime->tm_year + 1900) << "-"
               << std::setw(2) << (localTime->tm_mon + 1) << "-"
               << std::setw(2) << localTime->tm_mday << " "
               << std::setw(2) << localTime->tm_hour << ":"
               << std::setw(2) << localTime->tm_min << ":"
               << std::setw(2) << localTime->tm_sec;
    return timeStream.str();
}

bool isCardNumberValid(const std::string& cardNumber) {
    return !cardNumber.empty() && cardNumber.length() <= 18;
}

bool isPasswordValid(const std::string& password) {
    return !password.empty() && password.length() <= 8;
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear"); // 兼容 Linux/Mac
#endif
}

void pressEnterToContinue() {
    std::cout << "\n按回车键继续...";
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    std::cin.get();
}