#ifndef SERVICE_H
#define SERVICE_H

#include "model.h"

// 通用服务函数
void initSystem();
void exitSystem();

// 上机下机服务函数
int doLogon(const char *cardNumber, const char *password, LogonInfo *logonInfo);
int doSettle(const char *cardNumber, const char *password, SettleInfo *settleInfo);
int doAddMoney(const char *cardNumber, const char *password, float money, MoneyInfo *moneyInfo);
int doRefundMoney(const char *cardNumber, const char *password, MoneyInfo *moneyInfo);
int doAnnulCard(const char *cardNumber, const char *password, MoneyInfo *moneyInfo);
int doQueryStatistics(StatisticsInfo *statisticsInfo);
float getAmount(time_t start, time_t end);

#endif // SERVICE_H
