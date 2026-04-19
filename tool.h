#ifndef TOOL_H
#define TOOL_H

#include <time.h>
#include <string>

// 时间相关函数
time_t getCurrentTime();
std::string formatTime(time_t t);

// 字符串相关函数
bool isCardNumberValid(const char* cardNumber);
bool isPasswordValid(const char* password);

// 输入输出函数
void clearScreen();
void pressEnterToContinue();

#endif // TOOL_H
