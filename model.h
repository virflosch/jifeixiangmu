#ifndef MODEL_H
#define MODEL_H

#include <time.h>

// 卡信息结构体
typedef struct Card
{
    char aName[19];  // 卡号
    char aPwd[9];    // 密码
    int nStatus;     // 卡状态(0-未上机；1-正在上机；2-已注销；3-失效)
    time_t tStart;   // 开卡时间
    time_t tEnd;     // 卡的截止时间
    float fTotalUse; // 累计金额
    time_t tLast;    // 最后使用时间
    int nUseCount;   // 使用次数
    float fBalance;  // 余额
    int nDel;        // 删除标识0-未删除,1-删除
} Card;

// 链表结点结构体
typedef struct CardNode
{
    Card data;             // 卡信息结构体
    struct CardNode *next; // 指向下一个结点
} CardNode, *lpCardNode;

// 计费信息结构体
typedef struct Billing
{
    char aCardName[19]; // 卡号
    time_t tStart;      // 上机时间
    time_t tEnd;        // 下机时间
    float fAmount;      // 消费金额
    int nStatus;        // 消费状态（0-未结算，1-已结算）
    int nDel;           // 删除标识（0-未删除，1-已删除）
} Billing;

// 计费信息链表节点
typedef struct BillingNode
{
    Billing data;             // 计费数据
    struct BillingNode *next; // 下一个节点
} BillingNode, *IpBillingNode;

// 下机信息结构体
typedef struct SettleInfo
{
    char aCardName[19]; // 卡号
    time_t tStart;      // 上机时间
    time_t tEnd;        // 下机时间（当前系统时间）
    float fAmount;      // 消费金额（计算得出）
    float fBalance;     // 余额（上机余额 - 消费金额）
} SettleInfo;

// 上机信息结构体
typedef struct LogonInfo
{
    char aCardName[19]; // 上机卡号
    time_t tLogon;      // 上机时间
    float fBalance;     // 上机时的卡余额
} LogonInfo;

// 充值退费交易结构体
typedef struct Money
{
    char aCardName[19]; // 卡号
    time_t tTime;       // 充值退费的时间
    int nStatus;        // 状态：0-表示充值；1-表示退费
    float fMoney;       // 充值退费金额
    int nDel;           // 删除标识，0-未删除,1-删除
} Money;

// 充值退费显示结构体
typedef struct MoneyInfo
{
    char aCardName[19]; // 卡号
    float fMoney;       // 充值退费金额
    float fBalance;     // 余额
} MoneyInfo;

// 统计信息结构体
typedef struct StatisticsInfo
{
    int nCardCount;             // 卡总数
    int nNotInUseCount;         // 未上机卡数量
    int nInUseCount;            // 正在上机卡数量
    int nCancelledCount;        // 已注销卡数量
    int nInvalidCount;          // 失效卡数量
    float fTotalBalance;        // 卡总余额
    float fTotalUseAmount;      // 累计使用金额
    int nTotalUseCount;         // 累计使用次数
    int nBillingCount;          // 计费记录总数
    int nUnsettledBillingCount; // 未结算计费记录数
    int nSettledBillingCount;   // 已结算计费记录数
    int nRechargeCount;         // 充值记录数
    int nRefundCount;           // 退费记录数
    float fRechargeAmount;      // 充值总金额
    float fRefundAmount;        // 退费总金额
} StatisticsInfo;

#endif // MODEL_H
