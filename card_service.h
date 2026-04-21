#pragma once

#include <vector>
#include <string>
#include "model.h"
#include "global.h"

// 【重点变化】：用 C++ 的 std::vector 彻底替代原来的手写链表指针 lpCardNode
// 它会自动管理内存，随时可以 push_back 增加数据
extern std::vector<Card> g_cardList;

// 初始化和释放（现在变得非常简单）
void initCardList();
void releaseCardList();

// 业务函数：参数全部改为 const std::string& 避免拷贝，返回值改用安全的 OpResult 枚举
OpResult addCard(const std::string& cardNumber, const std::string& password, float balance);

Card* queryCard(const std::string& cardNumber);

// 【重点变化】：模糊查询直接返回一个装满结果的 vector，彻底告别原代码里的 new Card[] 和繁琐的 count 指针！
std::vector<Card> queryCards(const std::string& keyword);

OpResult cancelCard(const std::string& cardNumber);

Card* checkCard(const std::string& cardNumber, const std::string& password);

// 更新卡信息（传入引用，不传指针）
OpResult updateCard(const Card& card);