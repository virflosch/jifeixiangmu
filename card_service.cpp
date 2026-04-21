#include "card_service.h"
#include "card_file.h" // 暂时保留，虽然文件IO层还没重构
#include <iostream>
#include <ctime>

// 定义全局卡片容器
std::vector<Card> g_cardList;

// 初始化（只需清空 vector）
void initCardList() {
    g_cardList.clear();
}

// 释放内存（vector 离开作用域或被 clear 时，会自动释放所有内存，再也不用 while 循环 delete 了！）
void releaseCardList() {
    g_cardList.clear();
}

// 内部辅助函数：检查卡号是否存在
bool isCardExists(const std::string& cardNumber) {
    for (const auto& card : g_cardList) {
        if (card.aName == cardNumber && card.nDel == DeleteStatus::NotDeleted) {
            return true;
        }
    }
    return false;
}

// 新增卡
OpResult addCard(const std::string& cardNumber, const std::string& password, float balance) {
    if (isCardExists(cardNumber)) {
        return OpResult::Failed; // 卡号已存在
    }

    // 简单的长度校验（利用 std::string 的 length 方法，替代原来 tool.cpp 里数 '\0' 的老方法）
    if (cardNumber.empty() || cardNumber.length() > 18 || password.empty() || password.length() > 8) {
        return OpResult::Failed; 
    }

    // 利用我们在 model.h 中写的构造函数，实例化新卡片
    Card newCard;
    newCard.aName = cardNumber;
    newCard.aPwd = password;
    newCard.nStatus = CardStatus::NotInUse;
    newCard.tStart = std::time(nullptr);
    newCard.tEnd = newCard.tStart + 365 * 24 * 60 * 60; // 默认一年有效期
    newCard.fBalance = balance;

    // 先推入内存的动态数组尾部
    g_cardList.push_back(newCard);
    
    // 立刻调用 card_file.cpp 中的函数同步到文件
    if (saveCard(newCard) != OpResult::Success) { 
        // 如果文件保存失败（比如没有磁盘权限），为了保证数据一致性，把刚加进内存的卡弹出来
        g_cardList.pop_back(); 
        return OpResult::Failed; 
    }
    
    return OpResult::Success;
}

// 精确查询卡
Card* queryCard(const std::string& cardNumber) {
    for (auto& card : g_cardList) {
        // 直接使用 == 比较字符串，告别 strcmp!
        if (card.aName == cardNumber && card.nDel == DeleteStatus::NotDeleted) {
            return &card; // 返回对象在 vector 中的地址
        }
    }
    return nullptr;
}

// 模糊查询卡（原代码里需要遍历两次，第一次统计数量，第二次 new 数组并拷贝，现在只需要一次遍历！）
std::vector<Card> queryCards(const std::string& keyword) {
    std::vector<Card> result;
    for (const auto& card : g_cardList) {
        // 使用 std::string 的 find 方法，替代 strstr
        if (card.nDel == DeleteStatus::NotDeleted && card.aName.find(keyword) != std::string::npos) {
            result.push_back(card);
        }
    }
    return result; // 现代 C++ 会自动进行移动语义优化，不用担心返回容器会有性能损失
}

// 注销卡
OpResult cancelCard(const std::string& cardNumber) {
    Card* card = queryCard(cardNumber);
    if (card == nullptr) {
        return OpResult::Failed;
    }
    if (card->nStatus == CardStatus::InUse) {
        return OpResult::InvalidStatus; // 正在上机，不允许注销
    }

    // 记录旧状态，以防文件写入失败需要回滚
    CardStatus oldStatus = card->nStatus;
    time_t oldTime = card->tLast;

    // 修改内存中的状态
    card->nStatus = CardStatus::Cancelled;
    card->tLast = std::time(nullptr);

    // 调用 card_file.cpp 中的函数，将内存数据全量覆盖回文件
    if (updateCardToFile() != OpResult::Success) {
        // 如果写文件失败，把内存状态还原，并返回失败
        card->nStatus = oldStatus;
        card->tLast = oldTime;
        return OpResult::Failed;
    }

    return OpResult::Success;
}

// 验证卡号密码
Card* checkCard(const std::string& cardNumber, const std::string& password) {
    for (auto& card : g_cardList) {
        if (card.aName == cardNumber && card.aPwd == password && card.nDel == DeleteStatus::NotDeleted) {
            return &card;
        }
    }
    return nullptr;
}

// 更新卡信息
OpResult updateCard(const Card& cardData) {
    for (auto& card : g_cardList) {
        if (card.aName == cardData.aName && card.nDel == DeleteStatus::NotDeleted) {
            Card oldCard = card; // 备份旧数据
            card = cardData;     // 更新内存中的数据
            
            // 同步到文件
            if (updateCardToFile() != OpResult::Success) {
                card = oldCard;  // 如果写入失败，回滚内存数据
                return OpResult::Failed;
            }
            return OpResult::Success;
        }
    }
    return OpResult::Failed;
}