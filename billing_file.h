#pragma once
#include "model.h"

// 从文件加载所有计费记录到内存的 g_billingList
int loadBillings();

// 追加保存单条新计费记录到文件尾部
OpResult saveBilling(const Billing& billing);

// 将内存中所有的计费记录全量覆盖写入文件
OpResult updateBillingToFile();