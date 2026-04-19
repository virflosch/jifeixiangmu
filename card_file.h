#ifndef CARD_FILE_H
#define CARD_FILE_H

#include "model.h"
#include "global.h"

// 卡文件操作函数
int saveCards(Card cards[], int count);
int loadCards(Card cards[]);
int findCardIndex(Card cards[], int count, const char *cardNumber);

// 文本文件操作函数
int saveCard(const Card *pCard, const char *pPath);
int getCardCount(const char *pPath);
int readCard(Card *pCard, const char *pPath);
int getCard();
int updateCard(const Card *card, const char *pPath);
Card parseCard(char *pBuf);
time_t stringToTime(char *pTime);

#endif // CARD_FILE_H
