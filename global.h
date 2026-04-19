#ifndef GLOBAL_H
#define GLOBAL_H

// 卡状态定义
#define CARD_STATUS_NOT_IN_USE 0 // 未上机
#define CARD_STATUS_IN_USE 1     // 正在上机
#define CARD_STATUS_CANCELLED 2  // 已注销
#define CARD_STATUS_INVALID 3    // 失效

// 消费状态定义
#define BILLING_STATUS_UNSETTLED 0 // 未结算
#define BILLING_STATUS_SETTLED 1   // 已经结算

// 充值退费状态定义
#define MONEY_STATUS_RECHARGE 0 // 充值
#define MONEY_STATUS_REFUND 1   // 退费

// 删除标识定义
#define NOT_DELETED 0 // 未删除
#define DELETED 1     // 删除

// 文件路径
#define CARD_FILE_PATH "data/card.ams"
#define BILLING_FILE_PATH "billings.dat"
#define MONEY_FILE_PATH "data/money.ams"
#define CARD_TEXT_FILE_PATH "data/card.txt"

// 最大卡数量
#define MAX_CARD_COUNT 1000

// 最大计费记录数量
#define MAX_BILLING_COUNT 10000

// 最大充值退费记录数量
#define MAX_MONEY_COUNT 10000

// 上机相关常量
#define FALSE 0                         // 上机失败
#define TRUE 1                          // 上机成功
#define UNUSE 2                         // 卡不能使用（正在使用或已注销）
#define ENOUGHMONEY 3                   // 余额不足
#define BILLINGPATH "data\\billing.ams" // 计费信息保存路径

// 计费规则
#define UNIT 15    // 最小收费单元（分钟）
#define CHARGE 0.5 // 每个计费单元收费（RMB：元）

#endif // GLOBAL_H
