// model.h
#pragma once
#include <string>
#include <ctime>
#include "global.h"

// 卡信息结构体
struct Card {
    std::string aName;      // 卡号
    std::string aPwd;       // 密码
    CardStatus nStatus;     // 卡状态
    time_t tStart;          // 开卡时间
    time_t tEnd;            // 卡的截止时间
    float fTotalUse;        // 累计金额
    time_t tLast;           // 最后使用时间
    int nUseCount;          // 使用次数
    float fBalance;         // 余额
    DeleteStatus nDel;      // 删除标识

    // 默认构造函数，确保所有变量有初始值
    Card() : nStatus(CardStatus::NotInUse), tStart(0), tEnd(0), 
             fTotalUse(0.0f), tLast(0), nUseCount(0), fBalance(0.0f), nDel(DeleteStatus::NotDeleted) {}
};

// 计费信息结构体
struct Billing {
    std::string aCardName;  // 关联卡号
    time_t tStart;          // 上机时间
    time_t tEnd;            // 下机时间
    float fAmount;          // 消费金额
    BillingStatus nStatus;  // 消费状态
    DeleteStatus nDel;      // 删除标识

    Billing() : tStart(0), tEnd(0), fAmount(0.0f), nStatus(BillingStatus::Unsettled), nDel(DeleteStatus::NotDeleted) {}
};

// 下机信息结构体
struct SettleInfo {
    std::string aCardName;
    time_t tStart;
    time_t tEnd;
    float fAmount;
    float fBalance;
};

// 上机信息结构体
struct LogonInfo {
    std::string aCardName;
    time_t tLogon;
    float fBalance;
};

// 充值退费交易结构体
struct Money {
    std::string aCardName;
    time_t tTime;
    MoneyStatus nStatus;
    float fMoney;
    DeleteStatus nDel;

    Money() : tTime(0), nStatus(MoneyStatus::Recharge), fMoney(0.0f), nDel(DeleteStatus::NotDeleted) {}
};

// 充值退费显示结构体
struct MoneyInfo {
    std::string aCardName;
    float fMoney;
    float fBalance;
};

// 统计信息结构体 (C++11 支持直接在声明时赋默认值，非常方便)
struct StatisticsInfo {
    int nCardCount = 0;             
    int nNotInUseCount = 0;         
    int nInUseCount = 0;            
    int nCancelledCount = 0;        
    int nInvalidCount = 0;          
    float fTotalBalance = 0.0f;     
    float fTotalUseAmount = 0.0f;   
    int nTotalUseCount = 0;         
    int nBillingCount = 0;          
    int nUnsettledBillingCount = 0; 
    int nSettledBillingCount = 0;   
    int nRechargeCount = 0;         
    int nRefundCount = 0;           
    float fRechargeAmount = 0.0f;   
    float fRefundAmount = 0.0f;     
};