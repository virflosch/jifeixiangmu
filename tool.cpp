#define _CRT_SECURE_NO_WARNINGS

#include "tool.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <ctime>

// 获取当前时间
time_t getCurrentTime()
{
    return time(nullptr);
}

// 格式化时间为字符串
std::string formatTime(time_t t)
{
    struct tm *localTime = localtime(&t);
    std::stringstream timeStream;
    timeStream << std::setfill('0') << std::setw(4) << (localTime->tm_year + 1900) << "-"
               << std::setw(2) << (localTime->tm_mon + 1) << "-"
               << std::setw(2) << localTime->tm_mday << " "
               << std::setw(2) << localTime->tm_hour << ":"
               << std::setw(2) << localTime->tm_min << ":"
               << std::setw(2) << localTime->tm_sec;
    return timeStream.str();
}

// 验证卡号是否有效
bool isCardNumberValid(const char *cardNumber)
{
    if (cardNumber == nullptr)
    {
        return false;
    }
    int length = 0;
    while (cardNumber[length] != '\0')
    {
        length++;
    }
    return length >= 1 && length <= 18;
}

// 验证密码是否有效
bool isPasswordValid(const char *password)
{
    if (password == nullptr)
    {
        return false;
    }
    int length = 0;
    while (password[length] != '\0')
    {
        length++;
    }
    return length >= 1 && length <= 8;
}

// 清屏
void clearScreen()
{
    system("cls");
}

// 按回车键继续
void pressEnterToContinue()
{
    std::cout << "\n按回车键继续...";
    std::cin.ignore();
    std::cin.get();
}