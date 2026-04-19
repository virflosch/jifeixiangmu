#ifndef MENU_H
#define MENU_H

#include "model.h"
#include "service.h"
#include <string>
#include <cstring>

// 全局变量声明
extern Card g_cards[1000];
extern int g_cardCount;

// 函数声明
void startBillingSystem();
void printTitle();
void outputMenu();
void showSelectedOption(int choice);
int getUserChoice();
int findCardIndex(const char *cardNumber);
void addCard();
void queryCard();
void queryStatistics();
void annul();
void logon();
void settle();
void addMoney();
void refundMoney();

#endif
