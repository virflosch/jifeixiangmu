#pragma once
#include "model.h"

// 从文件加载所有卡信息到内存的 g_cardList，返回加载成功的数量
int getCard();

// 追加保存单条新卡片到文件尾部
OpResult saveCard(const Card& card);

// 将内存中所有的卡片全量覆盖写入文件 (用于信息更新时)
OpResult updateCardToFile();