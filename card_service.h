#ifndef CARD_SERVICE_H
#define CARD_SERVICE_H

#include "model.h"
#include "global.h"
#include "tool.h"

// 全局链表头指针
extern lpCardNode cardList;

// 卡管理服务函数
int initCardList();
void releaseCardList();
int addCard(const char *cardNumber, const char *password, float balance);
Card *queryCard(const char *cardNumber);
Card *queryCards(const char *keyword, int *count);
int cancelCard(const char *cardNumber);
Card *checkCard(const char *cardNumber, const char *password);
int updateCard(Card *card);

#endif // CARD_SERVICE_H
