#define _CRT_SECURE_NO_WARNINGS

#include "global.h"
#include "billing_service.h"
#include "billing_file.h"
#include <cstring>
#include <cmath>

// 全局计费链表头指针
IpBillingNode billingList = nullptr;

// 初始化计费链表
int initBillingList()
{
    // 创建头结点
    billingList = new BillingNode;
    if (billingList == nullptr)
    {
        return -1; // 内存分配失败
    }
    // 初始化头结点
    memset(&billingList->data, 0, sizeof(Billing));
    billingList->next = nullptr;
    return 0; // 成功
}

// 释放计费链表
void releaseBillingList()
{
    IpBillingNode current = billingList;
    while (current != nullptr)
    {
        IpBillingNode temp = current;
        current = current->next;
        delete temp;
    }
    billingList = nullptr;
}

// 添加计费记录
int addBilling(const char *cardNumber, time_t start, time_t end, float amount)
{
    // 创建新计费记录
    IpBillingNode newNode = new BillingNode;
    if (newNode == nullptr)
    {
        return -1; // 内存分配失败
    }

    // 初始化新结点
    strcpy(newNode->data.aCardName, cardNumber);
    newNode->data.tStart = start;
    newNode->data.tEnd = end;
    newNode->data.fAmount = amount;
    newNode->data.nStatus = BILLING_STATUS_UNSETTLED;
    newNode->data.nDel = NOT_DELETED;
    newNode->next = nullptr;

    // 插入链表尾部
    IpBillingNode current = billingList;
    while (current->next != nullptr)
    {
        current = current->next;
    }
    current->next = newNode;

    // 保存到文件
    if (saveBilling(&newNode->data) != 0)
    {
        return -2; // 保存文件失败
    }

    return 0; // 成功
}

// 查询卡的未结算计费记录
int queryBilling(const char *cardNumber, Billing *billing)
{
    if (billingList == nullptr || billingList->next == nullptr)
    {
        return -1; // 链表为空
    }

    IpBillingNode current = billingList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aCardName, cardNumber) == 0 &&
            current->data.nStatus == BILLING_STATUS_UNSETTLED &&
            current->data.nDel == NOT_DELETED)
        {
            *billing = current->data;
            return 0; // 找到未结算记录
        }
        current = current->next;
    }

    return -1; // 没有找到未结算记录
}

// 从文件加载计费记录
int getBilling()
{
    // 清空链表
    releaseBillingList();
    initBillingList();

    // 从文件加载
    int count = loadBillings();
    return count;
}

// 结算计费记录
int settleBilling(const char *cardNumber, SettleInfo *settleInfo)
{
    // 查找未结算的计费记录
    Billing billing;
    int result = queryBilling(cardNumber, &billing);
    if (result != 0)
    {
        return -1; // 没有找到未结算记录
    }

    // 计算消费金额
    time_t now = time(nullptr);
    double seconds = difftime(now, billing.tStart);
    double minutes = seconds / 60;
    int units = static_cast<int>(ceil(minutes / UNIT));
    float amount = units * CHARGE;

    // 填充结算信息
    strcpy(settleInfo->aCardName, cardNumber);
    settleInfo->tStart = billing.tStart;
    settleInfo->tEnd = now;
    settleInfo->fAmount = amount;

    return 0; // 成功
}

// 更新计费记录
int updateBilling(const Billing *billing)
{
    if (billingList == nullptr || billingList->next == nullptr)
    {
        return -1; // 链表为空
    }

    // 查找并更新链表中的记录
    IpBillingNode current = billingList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aCardName, billing->aCardName) == 0 &&
            current->data.tStart == billing->tStart &&
            current->data.nStatus == BILLING_STATUS_UNSETTLED)
        {
            // 更新链表中的记录
            current->data = *billing;

            // 更新文件中的记录
            if (updateBillingInFile(billing) != 0) {
                return -2; // 更新文件失败
            }

            return 0; // 成功
        }
        current = current->next;
    }

    return -3; // 未找到记录
}
