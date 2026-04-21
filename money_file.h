#pragma once
#include "model.h"
#include <vector>

// 保存单条资金流水到文件
OpResult saveMoney(const Money& money);

// 读取所有的资金流水（专门用于统计模块）
std::vector<Money> loadAllMoneyRecords();