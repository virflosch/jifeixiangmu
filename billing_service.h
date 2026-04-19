#ifndef BILLING_SERVICE_H
#define BILLING_SERVICE_H

#include "model.h"
#include "global.h"

// 全局计费链表头指针
extern IpBillingNode billingList;

// 计费管理服务函数
int initBillingList();
void releaseBillingList();
int addBilling(const char *cardNumber, time_t start, time_t end, float amount);
int queryBilling(const char *cardNumber, Billing *billing);
int getBilling();
int settleBilling(const char *cardNumber, SettleInfo *settleInfo);
int updateBilling(const Billing *billing);

#endif // BILLING_SERVICE_H
