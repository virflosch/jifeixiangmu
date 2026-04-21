#include "service.h"
#include "card_service.h"
#include "billing_service.h"
#include "card_file.h"
#include "billing_file.h"
#include "money_file.h"
#include <ctime>
#include <cmath>

// 初始化系统
void initSystem() {
    initCardList();
    initBillingList();
    getCard();       // 从文件加载卡片到内存
    loadBillings();  // 从文件加载计费记录到内存
}

// 退出系统
void exitSystem() {
    // 现代 C++ 退出时虽然 OS 会回收内存，但我们依然可以优雅清理
    releaseCardList();
    releaseBillingList();
}

// 执行上机操作
OpResult doLogon(const std::string& cardNumber, const std::string& password, LogonInfo& logonInfo) {
    Card* card = checkCard(cardNumber, password);
    if (!card) return OpResult::Failed;

    if (card->nStatus != CardStatus::NotInUse) return OpResult::InvalidStatus;
    if (card->fBalance <= 0) return OpResult::NotEnoughMoney;

    // 1. 改内存卡状态
    card->nStatus = CardStatus::InUse;
    card->tLast = std::time(nullptr);

    // 同步卡片到文件 (updateCard内部包含文件操作)
    if (updateCard(*card) != OpResult::Success) return OpResult::Failed;

    // 2. 增加计费流水
    time_t now = std::time(nullptr);
    if (addBilling(cardNumber, now, 0, 0) != OpResult::Success) {
        // 如果计费流水失败，状态回滚
        card->nStatus = CardStatus::NotInUse;
        updateCard(*card);
        return OpResult::Failed;
    }

    // 3. 填充返回信息
    logonInfo.aCardName = cardNumber;
    logonInfo.tLogon = now;
    logonInfo.fBalance = card->fBalance;
    return OpResult::Success;
}

// 执行下机操作
OpResult doSettle(const std::string& cardNumber, const std::string& password, SettleInfo& settleInfo) {
    Card* card = checkCard(cardNumber, password);
    if (!card) return OpResult::Failed;
    if (card->nStatus != CardStatus::InUse) return OpResult::InvalidStatus;

    // 获取结算信息并计算费用
    if (settleBilling(cardNumber, settleInfo) != OpResult::Success) {
        return OpResult::Failed; 
    }

    if (card->fBalance < settleInfo.fAmount) return OpResult::NotEnoughMoney;

    // 更新卡片信息
    card->fBalance -= settleInfo.fAmount;
    card->fTotalUse += settleInfo.fAmount;
    card->nUseCount++;
    card->nStatus = CardStatus::NotInUse;
    card->tLast = settleInfo.tEnd;
    if (updateCard(*card) != OpResult::Success) return OpResult::Failed;

    // 更新计费流水状态为已结算
    Billing billing;
    queryBilling(cardNumber, billing);
    billing.tEnd = settleInfo.tEnd;
    billing.fAmount = settleInfo.fAmount;
    billing.nStatus = BillingStatus::Settled;
    
    if (updateBilling(billing) != OpResult::Success) {
        // 如果计费状态更新失败，理论上需要复杂的回滚机制，这里简单处理
        return OpResult::Failed;
    }

    settleInfo.fBalance = card->fBalance;
    return OpResult::Success;
}

// 充值操作
OpResult doAddMoney(const std::string& cardNumber, const std::string& password, float money, MoneyInfo& moneyInfo) {
    if (money <= 0) return OpResult::Failed;

    Card* card = checkCard(cardNumber, password);
    if (!card) return OpResult::Failed;
    if (card->nStatus == CardStatus::Cancelled || card->nStatus == CardStatus::Invalid) return OpResult::InvalidStatus;

    // 更新卡余额
    card->fBalance += money;
    card->tLast = std::time(nullptr);
    if (updateCard(*card) != OpResult::Success) return OpResult::Failed;

    // 保存资金流水
    Money record;
    record.aCardName = cardNumber;
    record.tTime = std::time(nullptr);
    record.nStatus = MoneyStatus::Recharge;
    record.fMoney = money;
    
    if (saveMoney(record) != OpResult::Success) return OpResult::Failed; // 生产环境中这里也应有回滚逻辑

    moneyInfo.aCardName = cardNumber;
    moneyInfo.fMoney = money;
    moneyInfo.fBalance = card->fBalance;
    return OpResult::Success;
}

// 退费操作
OpResult doRefundMoney(const std::string& cardNumber, const std::string& password, MoneyInfo& moneyInfo) {
    Card* card = checkCard(cardNumber, password);
    if (!card) return OpResult::Failed;
    if (card->nStatus != CardStatus::NotInUse) return OpResult::InvalidStatus;
    if (card->fBalance <= 0) return OpResult::NotEnoughMoney;

    float refundMoney = card->fBalance;
    card->fBalance = 0;
    card->tLast = std::time(nullptr);
    if (updateCard(*card) != OpResult::Success) return OpResult::Failed;

    Money record;
    record.aCardName = cardNumber;
    record.tTime = std::time(nullptr);
    record.nStatus = MoneyStatus::Refund;
    record.fMoney = refundMoney;
    
    if (saveMoney(record) != OpResult::Success) return OpResult::Failed;

    moneyInfo.aCardName = cardNumber;
    moneyInfo.fMoney = refundMoney;
    moneyInfo.fBalance = 0;
    return OpResult::Success;
}

// 注销卡操作
OpResult doAnnulCard(const std::string& cardNumber, const std::string& password, MoneyInfo& moneyInfo) {
    Card* card = checkCard(cardNumber, password);
    if (!card) return OpResult::Failed;
    if (card->nStatus != CardStatus::NotInUse) return OpResult::InvalidStatus;

    float refundMoney = card->fBalance;
    // 使用我们重构过的 cancelCard 接口
    if (cancelCard(cardNumber) != OpResult::Success) return OpResult::Failed;

    // 清零余额的逻辑在服务层单独处理更灵活
    card->fBalance = 0;
    updateCard(*card); 

    moneyInfo.aCardName = cardNumber;
    moneyInfo.fMoney = refundMoney;
    moneyInfo.fBalance = 0;
    return OpResult::Success;
}

// 查询统计 (得益于 STL 容器，这段代码现在无比清爽！)
OpResult doQueryStatistics(StatisticsInfo& info) {
    info = StatisticsInfo(); // 重置为0

    // 统计卡片信息
    for (const auto& card : g_cardList) {
        if (card.nDel != DeleteStatus::NotDeleted) continue;

        info.nCardCount++;
        info.fTotalBalance += card.fBalance;
        info.fTotalUseAmount += card.fTotalUse;
        info.nTotalUseCount += card.nUseCount;

        switch (card.nStatus) {
            case CardStatus::NotInUse: info.nNotInUseCount++; break;
            case CardStatus::InUse: info.nInUseCount++; break;
            case CardStatus::Cancelled: info.nCancelledCount++; break;
            case CardStatus::Invalid: info.nInvalidCount++; break;
        }
    }

    // 统计计费信息
    for (const auto& billing : g_billingList) {
        if (billing.nDel != DeleteStatus::NotDeleted) continue;
        info.nBillingCount++;
        if (billing.nStatus == BillingStatus::Settled) info.nSettledBillingCount++;
        else info.nUnsettledBillingCount++;
    }

    // 统计资金流水
    std::vector<Money> moneyRecords = loadAllMoneyRecords();
    for (const auto& record : moneyRecords) {
        if (record.nDel != DeleteStatus::NotDeleted) continue;
        if (record.nStatus == MoneyStatus::Recharge) {
            info.nRechargeCount++;
            info.fRechargeAmount += record.fMoney;
        } else if (record.nStatus == MoneyStatus::Refund) {
            info.nRefundCount++;
            info.fRefundAmount += record.fMoney;
        }
    }

    return OpResult::Success;
}