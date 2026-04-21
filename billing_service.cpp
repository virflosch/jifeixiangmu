#include "billing_service.h"
#include "billing_file.h" // 文件层下一步重构
#include <cmath>
#include <ctime>

// 全局计费动态数组
std::vector<Billing> g_billingList;

// 初始化计费链表（只需清空）
void initBillingList() {
    g_billingList.clear();
}

// 释放计费链表
void releaseBillingList() {
    g_billingList.clear();
}

// 添加计费记录
OpResult addBilling(const std::string& cardNumber, time_t start, time_t end, float amount) {
    Billing newBilling;
    newBilling.aCardName = cardNumber;
    newBilling.tStart = start;
    newBilling.tEnd = end;
    newBilling.fAmount = amount;
    newBilling.nStatus = BillingStatus::Unsettled;
    newBilling.nDel = DeleteStatus::NotDeleted;

    g_billingList.push_back(newBilling);

    // 调用文件层同步到硬盘
    if (saveBilling(newBilling) != OpResult::Success) { 
        // 写入失败则回滚内存
        g_billingList.pop_back();
        return OpResult::Failed; 
    }

    return OpResult::Success;
}

// 查询卡的未结算计费记录
OpResult queryBilling(const std::string& cardNumber, Billing& billing) {
    for (const auto& record : g_billingList) {
        if (record.aCardName == cardNumber && 
            record.nStatus == BillingStatus::Unsettled && 
            record.nDel == DeleteStatus::NotDeleted) {
            billing = record; // 直接拷贝赋值
            return OpResult::Success;
        }
    }
    return OpResult::Failed; // 没有找到未结算记录
}

// 结算计费记录
OpResult settleBilling(const std::string& cardNumber, SettleInfo& settleInfo) {
    Billing billing;
    if (queryBilling(cardNumber, billing) != OpResult::Success) {
        return OpResult::Failed; // 没有找到未结算记录
    }

    // 计算消费金额
    time_t now = std::time(nullptr);
    double seconds = std::difftime(now, billing.tStart);
    double minutes = seconds / 60.0;
    
    // Config::UNIT 和 Config::CHARGE 是我们在 global.h 里定义的常量
    int units = static_cast<int>(std::ceil(minutes / Config::UNIT));
    float amount = units * Config::CHARGE;

    // 填充结算信息
    settleInfo.aCardName = cardNumber;
    settleInfo.tStart = billing.tStart;
    settleInfo.tEnd = now;
    settleInfo.fAmount = amount;

    return OpResult::Success;
}

// 更新计费记录
OpResult updateBilling(const Billing& billing) {
    for (auto& record : g_billingList) {
        // 通过卡号、上机时间和未结算状态来定位那条唯一的记录
        if (record.aCardName == billing.aCardName &&
            record.tStart == billing.tStart &&
            record.nStatus == BillingStatus::Unsettled) {
            
           Billing oldBilling = record; // 备份旧数据
            record = billing;            // 更新内存数据

            // 全量同步到文件
            if (updateBillingToFile() != OpResult::Success) { 
                record = oldBilling;     // 写入失败则回滚
                return OpResult::Failed; 
            }

            return OpResult::Success;
        }
    }
    return OpResult::Failed; // 未找到记录
}