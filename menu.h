#pragma once
#include "model.h"
#include <string>

// 系统入口
void startBillingSystem();

// UI 渲染函数
void printTitle();
void outputMenu();
void showSelectedOption(int choice);
int getUserChoice();

// 各个菜单功能的交互封装
void handleAddCard();
void handleQueryCard();
void handleQueryStatistics();
void handleAnnulCard();
void handleLogon();
void handleSettle();
void handleAddMoney();
void handleRefundMoney();