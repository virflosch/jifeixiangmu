#pragma once

#include <vector>
#include <string>
#include "model.h"
#include "global.h"

// 使用 std::vector 替代手写链表
extern std::vector<Billing> g_billingList;

// 初始化和释放
void initBillingList();
void releaseBillingList();

// 添加计费记录
OpResult addBilling(const std::string& cardNumber, time_t start, time_t end, float amount);

// 查询卡的未结算计费记录（使用引用传递结果）
OpResult queryBilling(const std::string& cardNumber, Billing& billing);

// 结算计费记录
OpResult settleBilling(const std::string& cardNumber, SettleInfo& settleInfo);

// 更新计费记录
OpResult updateBilling(const Billing& billing);