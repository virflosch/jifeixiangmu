#pragma once
#include "model.h"
#include <string>

// 通用系统服务
void initSystem();
void exitSystem();

// 核心业务服务 (参数改为 const std::string& 避免拷贝，输出参数改为引用 &)
OpResult doLogon(const std::string& cardNumber, const std::string& password, LogonInfo& logonInfo);
OpResult doSettle(const std::string& cardNumber, const std::string& password, SettleInfo& settleInfo);
OpResult doAddMoney(const std::string& cardNumber, const std::string& password, float money, MoneyInfo& moneyInfo);
OpResult doRefundMoney(const std::string& cardNumber, const std::string& password, MoneyInfo& moneyInfo);
OpResult doAnnulCard(const std::string& cardNumber, const std::string& password, MoneyInfo& moneyInfo);
OpResult doQueryStatistics(StatisticsInfo& statisticsInfo);