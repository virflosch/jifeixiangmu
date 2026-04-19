#define _CRT_SECURE_NO_WARNINGS

#include "global.h"
#include "card_service.h"
#include "card_file.h"
#include "tool.h"
#include <cstring>
// 全局链表头指针
lpCardNode cardList = nullptr;

// 初始化链表
int initCardList()
{
    // 创建头结点
    cardList = new CardNode;
    if (cardList == nullptr)
    {
        return -1; // 内存分配失败
    }
    // 初始化头结点
    memset(&cardList->data, 0, sizeof(Card));
    cardList->next = nullptr;
    return 0; // 成功
}

// 释放链表
void releaseCardList()
{
    lpCardNode current = cardList;
    while (current != nullptr)
    {
        lpCardNode temp = current;
        current = current->next;
        delete temp;
    }
    cardList = nullptr;
}

// 检查卡号是否已存在
int isCardExists(const char *cardNumber)
{
    if (cardList == nullptr)
    {
        return 0;
    }
    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aName, cardNumber) == 0 && current->data.nDel == 0)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

// 新增卡
int addCard(const char *cardNumber, const char *password, float balance)
{
    // 检查卡号是否已存在
    if (isCardExists(cardNumber))
    {
        return -2; // 卡号已存在
    }

    // 检查卡号和密码是否有效
    if (!isCardNumberValid(cardNumber) || !isPasswordValid(password))
    {
        return -3; // 卡号或密码无效
    }

    // 创建新卡
    lpCardNode newNode = new CardNode;
    if (newNode == nullptr)
    {
        return -1; // 内存分配失败
    }

    // 初始化新结点
    strcpy(newNode->data.aName, cardNumber);
    strcpy(newNode->data.aPwd, password);
    newNode->data.nStatus = CARD_STATUS_NOT_IN_USE;
    newNode->data.tStart = getCurrentTime();
    // 设置卡的截止时间为开卡时间后一年
    newNode->data.tEnd = newNode->data.tStart + 365 * 24 * 60 * 60;
    newNode->data.fTotalUse = 0.0;
    newNode->data.tLast = newNode->data.tStart;
    newNode->data.nUseCount = 0;
    newNode->data.fBalance = balance;
    newNode->data.nDel = NOT_DELETED;
    newNode->next = nullptr;

    // 插入链表尾部
    lpCardNode current = cardList;
    while (current->next != nullptr)
    {
        current = current->next;
    }
    current->next = newNode;

    // 保存到文本文件
    if (saveCard(&newNode->data, CARD_FILE_PATH) != 0)
    {
        current->next = nullptr;
        delete newNode;
        return -4; // 保存文件失败
    }

    return 0; // 成功
}
// 精确查询卡
Card *queryCard(const char *cardNumber)
{
    if (cardList == nullptr)
    {
        return nullptr;
    }
    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aName, cardNumber) == 0 && current->data.nDel == 0)
        {
            return &(current->data);
        }
        current = current->next;
    }
    return nullptr;
}

// 模糊查询卡
Card *queryCards(const char *keyword, int *count)
{
    *count = 0;

    if (cardList == nullptr)
    {
        return nullptr;
    }

    // 第一次遍历，统计匹配数量
    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (current->data.nDel == 0 && strstr(current->data.aName, keyword) != nullptr)
        {
            (*count)++;
        }
        current = current->next;
    }

    if (*count == 0)
    {
        return nullptr;
    }

    // 第二次遍历，收集匹配的卡
    Card *result = new Card[*count];
    if (result == nullptr)
    {
        *count = 0;
        return nullptr;
    }

    int index = 0;
    current = cardList->next;
    while (current != nullptr)
    {
        if (current->data.nDel == 0 && strstr(current->data.aName, keyword) != nullptr)
        {
            result[index++] = current->data;
        }
        current = current->next;
    }

    return result;
}

// 注销卡
int cancelCard(const char *cardNumber)
{
    if (cardList == nullptr)
    {
        return -1;
    }
    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aName, cardNumber) == 0 && current->data.nDel == 0)
        {
            // 检查卡状态
            if (current->data.nStatus == CARD_STATUS_IN_USE)
            {
                return -2;
            }

            int oldStatus = current->data.nStatus;

            // 注销卡
            current->data.nStatus = CARD_STATUS_CANCELLED;

            // 保存到文件
            if (updateCard(&current->data, CARD_FILE_PATH) != 0)
            {
                current->data.nStatus = oldStatus;
                return -3;
            }

            return 0;
        }
        current = current->next;
    }

    return -1;
}

// 验证卡号密码
Card *checkCard(const char *cardNumber, const char *password)
{
    if (cardList == nullptr || cardList->next == nullptr)
    {
        return nullptr;
    }

    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aName, cardNumber) == 0 &&
            strcmp(current->data.aPwd, password) == 0 &&
            current->data.nDel == 0)
        {
            return &(current->data);
        }
        current = current->next;
    }

    return nullptr;
}

// 更新卡信息
int updateCard(Card *card)
{
    if (card == nullptr || cardList == nullptr || cardList->next == nullptr)
    {
        return -1;
    }

    lpCardNode current = cardList->next;
    while (current != nullptr)
    {
        if (strcmp(current->data.aName, card->aName) == 0 &&
            current->data.nDel == 0)
        {
            // 更新卡信息
            current->data = *card;
            return 0;
        }
        current = current->next;
    }

    return -1;
}
