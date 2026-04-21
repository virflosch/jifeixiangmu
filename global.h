#pragma once 

namespace Config {
    // 文件路径使用 constexpr 替代宏，更安全
    constexpr const char* CARD_FILE_PATH = "data/card.ams";
    constexpr const char* BILLING_FILE_PATH = "billings.dat";
    constexpr const char* MONEY_FILE_PATH = "data/money.ams";
    constexpr const char* CARD_TEXT_FILE_PATH = "data/card.txt";

    // 计费规则与容量限制
    constexpr int MAX_CARD_COUNT = 1000;
    constexpr int MAX_BILLING_COUNT = 10000;
    constexpr int MAX_MONEY_COUNT = 10000;
    constexpr int UNIT = 15;       // 最小收费单元（分钟）
    constexpr float CHARGE = 0.5f; // 每个计费单元收费（RMB：元）
}

// 卡状态枚举
enum class CardStatus {
    NotInUse = 0, // 未上机
    InUse = 1,    // 正在上机
    Cancelled = 2,// 已注销
    Invalid = 3   // 失效
};

// 消费状态枚举
enum class BillingStatus {
    Unsettled = 0, // 未结算
    Settled = 1    // 已经结算
};

// 充值退费状态枚举
enum class MoneyStatus {
    Recharge = 0, // 充值
    Refund = 1    // 退费
};

// 删除标识枚举
enum class DeleteStatus {
    NotDeleted = 0, // 未删除
    Deleted = 1     // 删除
};

// 业务操作返回值枚举 
enum class OpResult {
    Success = 1,
    Failed = 0,
    InvalidStatus = 2, // 状态异常（如卡正在使用或已注销）
    NotEnoughMoney = 3 // 余额不足
};