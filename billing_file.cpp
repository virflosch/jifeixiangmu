#define _CRT_SECURE_NO_WARNINGS

#include "billing_file.h"
#include "billing_service.h"
#include <fstream>
#include <cstring>

// 保存单个计费记录到文件（追加）
int saveBilling(const Billing* billing) {
    std::ofstream outFile(BILLINGPATH, std::ios::binary | std::ios::app);
    if (!outFile) {
        return -1;
    }
    outFile.write(reinterpret_cast<const char*>(billing), sizeof(Billing));
    outFile.close();
    return 0;
}

// 从文件加载所有计费记录到链表
int loadBillings() {
    std::ifstream inFile(BILLINGPATH, std::ios::binary);
    if (!inFile) {
        return 0;
    }

    inFile.seekg(0, std::ios::end);
    int count = inFile.tellg() / sizeof(Billing);
    inFile.seekg(0, std::ios::beg);

    for (int i = 0; i < count; i++) {
        Billing billing;
        inFile.read(reinterpret_cast<char*>(&billing), sizeof(Billing));

        // 添加到链表
        IpBillingNode newNode = new BillingNode;
        newNode->data = billing;
        newNode->next = nullptr;

        // 插入链表尾部
        IpBillingNode current = billingList;
        while (current->next != nullptr) {
            current = current->next;
        }
        current->next = newNode;
    }

    inFile.close();
    return count;
}

// 获取文件中计费记录的数量
int getBillingCount() {
    std::ifstream inFile(BILLINGPATH, std::ios::binary);
    if (!inFile) {
        return 0;
    }

    inFile.seekg(0, std::ios::end);
    int count = inFile.tellg() / sizeof(Billing);
    inFile.close();
    return count;
}

// 更新指定的计费记录到文件
int updateBillingInFile(const Billing* billing) {
    std::fstream file(BILLINGPATH, std::ios::binary | std::ios::in | std::ios::out);
    if (!file) {
        return -1;
    }

    Billing temp;
    int index = 0;
    bool found = false;

    while (file.read(reinterpret_cast<char*>(&temp), sizeof(Billing))) {
        if (strcmp(temp.aCardName, billing->aCardName) == 0 && 
            temp.tStart == billing->tStart && 
            temp.nStatus == BILLING_STATUS_UNSETTLED) {
            // 定位到记录位置
            file.seekp(index * sizeof(Billing), std::ios::beg);
            file.write(reinterpret_cast<const char*>(billing), sizeof(Billing));
            found = true;
            break;
        }
        index++;
    }

    file.close();
    return found ? 0 : -1;
}