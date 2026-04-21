#pragma once
#include <string>
#include <ctime>

// 时间相关函数
time_t getCurrentTime();
std::string formatTime(time_t t);

// 字符串验证函数
bool isCardNumberValid(const std::string& cardNumber);
bool isPasswordValid(const std::string& password);

// 交互辅助
void clearScreen();
void pressEnterToContinue();