#ifndef BILLING_FILE_H
#define BILLING_FILE_H

#include "model.h"
#include "global.h"

// 计费文件操作函数
int saveBilling(const Billing *billing);
int loadBillings();
int getBillingCount();
int updateBillingInFile(const Billing *billing);

#endif // BILLING_FILE_H
