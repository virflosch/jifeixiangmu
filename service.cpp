#include "service.h"
#include "card_service.h"
#include "billing_service.h"
#include "card_file.h"
#include "money_file.h"
#include "global.h"
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>

// 初始化系统
void initSystem()
{
    // 初始化卡链表
    initCardList();
    // 初始化计费链表
    initBillingList();
    // 从文件加载卡信息
    getCard();
    // 从文件加载计费信息
    getBilling();
}

// 退出系统
void exitSystem()
{
    // 释放卡链表
    releaseCardList();
    // 释放计费链表
    releaseBillingList();
}

// 计算消费金额
float getAmount(time_t start, time_t end)
{
    double seconds = difftime(end, start);
    double minutes = seconds / 60;
    int units = static_cast<int>(ceil(minutes / UNIT));
    return units * CHARGE;
}

// 执行上机操作
int doLogon(const char *cardNumber, const char *password, LogonInfo *logonInfo)
{
    // 验证卡号密码
    Card *card = checkCard(cardNumber, password);
    if (card == nullptr)
    {
        return FALSE; // 上机失败
    }

    // 检查卡状态
    if (card->nStatus == CARD_STATUS_IN_USE || card->nStatus == CARD_STATUS_CANCELLED || card->nStatus == CARD_STATUS_INVALID)
    {
        return UNUSE; // 卡不能使用
    }

    // 检查余额
    if (card->fBalance <= 0)
    {
        return ENOUGHMONEY; // 余额不足
    }

    // 更新卡状态为上机中
    card->nStatus = CARD_STATUS_IN_USE;
    card->tLast = time(nullptr);

    // 更新卡信息到文件
    if (updateCard(card, CARD_FILE_PATH) != 0)
    {
        return FALSE; // 更新失败
    }

    // 添加计费记录
    time_t now = time(nullptr);
    if (addBilling(cardNumber, now, 0, 0) != 0)
    {
        // 恢复卡状态
        card->nStatus = CARD_STATUS_NOT_IN_USE;
        updateCard(card, CARD_FILE_PATH);
        return FALSE; // 添加计费记录失败
    }

    // 填充上机信息
    strcpy(logonInfo->aCardName, cardNumber);
    logonInfo->tLogon = now;
    logonInfo->fBalance = card->fBalance;

    return TRUE; // 上机成功
}

// 执行下机操作
int doSettle(const char *cardNumber, const char *password, SettleInfo *settleInfo)
{
    // 验证卡号密码
    Card *card = checkCard(cardNumber, password);
    if (card == nullptr)
    {
        return FALSE; // 下机失败
    }

    // 检查卡状态
    if (card->nStatus != CARD_STATUS_IN_USE)
    {
        return UNUSE; // 卡未上机
    }

    // 查找未结算的计费记录
    Billing billing;
    if (queryBilling(cardNumber, &billing) != 0)
    {
        return FALSE; // 没有找到未结算记录
    }

    // 计算消费金额
    time_t now = time(nullptr);
    float amount = getAmount(billing.tStart, now);

    // 检查余额是否充足
    if (card->fBalance < amount)
    {
        return ENOUGHMONEY; // 余额不足
    }

    // 更新卡信息
    card->fBalance -= amount;
    card->fTotalUse += amount;
    card->nUseCount++;
    card->nStatus = CARD_STATUS_NOT_IN_USE;
    card->tLast = now;

    // 更新卡信息到文件
    if (updateCard(card, CARD_FILE_PATH) != 0)
    {
        return FALSE; // 更新失败
    }

    // 更新计费记录
    billing.tEnd = now;
    billing.fAmount = amount;
    billing.nStatus = BILLING_STATUS_SETTLED;

    if (updateBilling(&billing) != 0)
    {
        // 恢复卡信息
        card->fBalance += amount;
        card->fTotalUse -= amount;
        card->nUseCount--;
        card->nStatus = CARD_STATUS_IN_USE;
        updateCard(card, CARD_FILE_PATH);
        return FALSE; // 更新计费记录失败
    }

    // 填充下机信息
    strcpy(settleInfo->aCardName, cardNumber);
    settleInfo->tStart = billing.tStart;
    settleInfo->tEnd = now;
    settleInfo->fAmount = amount;
    settleInfo->fBalance = card->fBalance;

    return TRUE; // 下机成功
}

// 执行充值操作
int doAddMoney(const char *cardNumber, const char *password, float money, MoneyInfo *moneyInfo)
{
    if (moneyInfo == nullptr || money <= 0)
    {
        return FALSE;
    }

    Card *card = checkCard(cardNumber, password);
    if (card == nullptr)
    {
        return FALSE;
    }

    time_t now = time(nullptr);
    if (card->nStatus == CARD_STATUS_CANCELLED || card->nStatus == CARD_STATUS_INVALID || card->tEnd < now)
    {
        return UNUSE;
    }

    if (card->nStatus != CARD_STATUS_NOT_IN_USE && card->nStatus != CARD_STATUS_IN_USE)
    {
        return UNUSE;
    }

    Card oldCard = *card;
    card->fBalance += money;
    card->tLast = now;

    if (updateCard(card, CARD_FILE_PATH) != 0)
    {
        *card = oldCard;
        return FALSE;
    }

    Money record;
    memset(&record, 0, sizeof(Money));
    strcpy(record.aCardName, cardNumber);
    record.tTime = now;
    record.nStatus = MONEY_STATUS_RECHARGE;
    record.fMoney = money;
    record.nDel = NOT_DELETED;

    if (saveMoney(&record, MONEY_FILE_PATH) != 0)
    {
        *card = oldCard;
        updateCard(card, CARD_FILE_PATH);
        return FALSE;
    }

    strcpy(moneyInfo->aCardName, cardNumber);
    moneyInfo->fMoney = money;
    moneyInfo->fBalance = card->fBalance;
    return TRUE;
}

// 执行退费操作
int doRefundMoney(const char *cardNumber, const char *password, MoneyInfo *moneyInfo)
{
    if (moneyInfo == nullptr)
    {
        return FALSE;
    }

    Card *card = checkCard(cardNumber, password);
    if (card == nullptr)
    {
        return FALSE;
    }

    time_t now = time(nullptr);
    if (card->nStatus == CARD_STATUS_CANCELLED || card->nStatus == CARD_STATUS_INVALID || card->tEnd < now)
    {
        return UNUSE;
    }

    if (card->nStatus != CARD_STATUS_NOT_IN_USE)
    {
        return UNUSE;
    }

    if (card->fBalance <= 0)
    {
        return ENOUGHMONEY;
    }

    Card oldCard = *card;
    float refundMoney = card->fBalance;
    card->fBalance = 0;
    card->tLast = now;

    if (updateCard(card, CARD_FILE_PATH) != 0)
    {
        *card = oldCard;
        return FALSE;
    }

    Money record;
    memset(&record, 0, sizeof(Money));
    strcpy(record.aCardName, cardNumber);
    record.tTime = now;
    record.nStatus = MONEY_STATUS_REFUND;
    record.fMoney = refundMoney;
    record.nDel = NOT_DELETED;

    if (saveMoney(&record, MONEY_FILE_PATH) != 0)
    {
        *card = oldCard;
        updateCard(card, CARD_FILE_PATH);
        return FALSE;
    }

    strcpy(moneyInfo->aCardName, cardNumber);
    moneyInfo->fMoney = refundMoney;
    moneyInfo->fBalance = 0;
    return TRUE;
}

// 执行注销卡操作
int doAnnulCard(const char *cardNumber, const char *password, MoneyInfo *moneyInfo)
{
    if (moneyInfo == nullptr)
    {
        return FALSE;
    }

    Card *card = checkCard(cardNumber, password);
    if (card == nullptr)
    {
        return FALSE;
    }

    if (card->nStatus != CARD_STATUS_NOT_IN_USE)
    {
        return UNUSE;
    }

    Card oldCard = *card;
    float refundMoney = card->fBalance;
    time_t now = time(nullptr);

    card->nStatus = CARD_STATUS_CANCELLED;
    card->fBalance = 0;
    card->tLast = now;

    if (updateCard(card, CARD_FILE_PATH) != 0)
    {
        *card = oldCard;
        return FALSE;
    }

    strcpy(moneyInfo->aCardName, cardNumber);
    moneyInfo->fMoney = refundMoney;
    moneyInfo->fBalance = 0;
    return TRUE;
}

// 执行查询统计
int doQueryStatistics(StatisticsInfo *statisticsInfo)
{
    if (statisticsInfo == nullptr)
    {
        return FALSE;
    }

    memset(statisticsInfo, 0, sizeof(StatisticsInfo));

    if (cardList != nullptr)
    {
        lpCardNode currentCard = cardList->next;
        while (currentCard != nullptr)
        {
            if (currentCard->data.nDel == NOT_DELETED)
            {
                statisticsInfo->nCardCount++;
                statisticsInfo->fTotalBalance += currentCard->data.fBalance;
                statisticsInfo->fTotalUseAmount += currentCard->data.fTotalUse;
                statisticsInfo->nTotalUseCount += currentCard->data.nUseCount;

                switch (currentCard->data.nStatus)
                {
                case CARD_STATUS_NOT_IN_USE:
                    statisticsInfo->nNotInUseCount++;
                    break;
                case CARD_STATUS_IN_USE:
                    statisticsInfo->nInUseCount++;
                    break;
                case CARD_STATUS_CANCELLED:
                    statisticsInfo->nCancelledCount++;
                    break;
                case CARD_STATUS_INVALID:
                    statisticsInfo->nInvalidCount++;
                    break;
                default:
                    break;
                }
            }

            currentCard = currentCard->next;
        }
    }

    if (billingList != nullptr)
    {
        IpBillingNode currentBilling = billingList->next;
        while (currentBilling != nullptr)
        {
            if (currentBilling->data.nDel == NOT_DELETED)
            {
                statisticsInfo->nBillingCount++;
                if (currentBilling->data.nStatus == BILLING_STATUS_SETTLED)
                {
                    statisticsInfo->nSettledBillingCount++;
                }
                else
                {
                    statisticsInfo->nUnsettledBillingCount++;
                }
            }

            currentBilling = currentBilling->next;
        }
    }

    std::ifstream moneyFile(MONEY_FILE_PATH, std::ios::binary);
    if (moneyFile)
    {
        Money moneyRecord;
        while (moneyFile.read(reinterpret_cast<char *>(&moneyRecord), sizeof(Money)))
        {
            if (moneyRecord.nDel != NOT_DELETED)
            {
                continue;
            }

            if (moneyRecord.nStatus == MONEY_STATUS_RECHARGE)
            {
                statisticsInfo->nRechargeCount++;
                statisticsInfo->fRechargeAmount += moneyRecord.fMoney;
            }
            else if (moneyRecord.nStatus == MONEY_STATUS_REFUND)
            {
                statisticsInfo->nRefundCount++;
                statisticsInfo->fRefundAmount += moneyRecord.fMoney;
            }
        }

        moneyFile.close();
    }

    return TRUE;
}
