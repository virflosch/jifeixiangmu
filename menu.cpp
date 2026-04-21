#include "menu.h"
#include "tool.h"
#include "service.h"
#include "card_service.h"
#include <iostream>
#include <limits>

using namespace std;

void printTitle() {
    cout << "=================================" << endl;
    cout << "      现代 C++ 计费管理系统 v2.0    " << endl;
    cout << "=================================" << endl;
}

void outputMenu() {
    cout << "1. 新增卡     2. 查询卡     3. 上机" << endl;
    cout << "4. 下机       5. 充值       6. 退费" << endl;
    cout << "7. 查询统计   8. 注销卡     0. 退出" << endl;
    cout << "---------------------------------" << endl;
}

int getUserChoice() {
    int choice;
    while (true) {
        cout << "请选择功能 (0-8): ";
        if (cin >> choice && choice >= 0 && choice <= 8) {
            return choice;
        }
        cout << "输入无效，请重新输入!\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// 新增卡
void handleAddCard() {
    string cardNumber, password;
    float balance;

    cout << "请输入卡号 (最长18位): ";
    cin >> cardNumber;
    cout << "请输入密码 (最长8位): ";
    cin >> password;
    cout << "请输入开卡金额: ";
    cin >> balance;

    OpResult result = addCard(cardNumber, password, balance);
    if (result == OpResult::Success) cout << "新增卡成功!" << endl;
    else cout << "新增卡失败 (卡号已存在或输入非法)!" << endl;
}

// 辅助打印卡片状态
string getStatusStr(CardStatus status) {
    switch(status) {
        case CardStatus::NotInUse: return "未上机";
        case CardStatus::InUse: return "正在上机";
        case CardStatus::Cancelled: return "已注销";
        case CardStatus::Invalid: return "失效";
        default: return "未知";
    }
}

// 查询卡 (模糊查询现在变得非常简单)
void handleQueryCard() {
    int type;
    cout << "1. 精确查询  2. 模糊查询  3. 所有卡\n请选择: ";
    cin >> type;

    if (type == 1) {
        string cardNumber;
        cout << "请输入完整卡号: ";
        cin >> cardNumber;
        Card* card = queryCard(cardNumber);
        if (card) {
            cout << "\n卡号: " << card->aName << " | 状态: " << getStatusStr(card->nStatus)
                 << " | 余额: " << card->fBalance << "元\n";
        } else cout << "卡片不存在！\n";
    } 
    else if (type == 2 || type == 3) {
        string keyword = "";
        if (type == 2) {
            cout << "请输入关键字: ";
            cin >> keyword;
        }
        
        // 直接获取载满数据的 vector，不再需要自己去 new 和 delete！
        vector<Card> results = queryCards(keyword); 
        if (results.empty()) {
            cout << "没有找到匹配的卡片。\n";
        } else {
            cout << "\n找到 " << results.size() << " 张卡片:\n";
            for (const auto& c : results) {
                cout << "卡号: " << c.aName << " | 状态: " << getStatusStr(c.nStatus) 
                     << " | 余额: " << c.fBalance << "元\n";
            }
        }
    }
}

// 上机
void handleLogon() {
    string cardNumber, password;
    LogonInfo info;
    cout << "卡号: "; cin >> cardNumber;
    cout << "密码: "; cin >> password;

    OpResult result = doLogon(cardNumber, password, info);
    if (result == OpResult::Success) {
        cout << "\n上机成功！\n卡号: " << info.aCardName 
             << " | 上机时间: " << formatTime(info.tLogon) << " | 余额: " << info.fBalance << endl;
    } else if (result == OpResult::InvalidStatus) {
        cout << "该卡状态异常，无法上机！\n";
    } else if (result == OpResult::NotEnoughMoney) {
        cout << "余额不足！\n";
    } else {
        cout << "上机失败 (卡号或密码错误)！\n";
    }
}

// 下机
void handleSettle() {
    string cardNumber, password;
    SettleInfo info;
    cout << "卡号: "; cin >> cardNumber;
    cout << "密码: "; cin >> password;

    OpResult result = doSettle(cardNumber, password, info);
    if (result == OpResult::Success) {
        cout << "\n下机成功！\n消费: " << info.fAmount << "元 | 剩余余额: " << info.fBalance << "元\n";
    } else {
        cout << "下机失败 (卡未上机或密码错误)！\n";
    }
}

// 充值
void handleAddMoney() {
    string cardNumber, password;
    float money;
    MoneyInfo info;
    cout << "卡号: "; cin >> cardNumber;
    cout << "密码: "; cin >> password;
    cout << "充值金额: "; cin >> money;

    if (doAddMoney(cardNumber, password, money, info) == OpResult::Success) {
        cout << "充值成功！当前余额: " << info.fBalance << "元\n";
    } else cout << "充值失败！\n";
}

// 退费
void handleRefundMoney() {
    string cardNumber, password;
    MoneyInfo info;
    cout << "卡号: "; cin >> cardNumber;
    cout << "密码: "; cin >> password;

    if (doRefundMoney(cardNumber, password, info) == OpResult::Success) {
        cout << "退费成功！退款金额: " << info.fMoney << "元\n";
    } else cout << "退费失败！\n";
}

// 注销
void handleAnnulCard() {
    string cardNumber, password;
    MoneyInfo info;
    cout << "卡号: "; cin >> cardNumber;
    cout << "密码: "; cin >> password;

    if (doAnnulCard(cardNumber, password, info) == OpResult::Success) {
        cout << "注销成功！退还余额: " << info.fMoney << "元\n";
    } else cout << "注销失败！\n";
}

// 统计
void handleQueryStatistics() {
    StatisticsInfo info;
    if (doQueryStatistics(info) == OpResult::Success) {
        cout << "\n====== 营业统计 ======\n"
             << "总卡数: " << info.nCardCount << " | 正在上机: " << info.nInUseCount << "\n"
             << "总充值: " << info.fRechargeAmount << "元 | 总消费: " << info.fTotalUseAmount << "元\n"
             << "======================\n";
    }
}

// 主循环
void startBillingSystem() {
    while (true) {
        printTitle();
        outputMenu();
        int choice = getUserChoice();

        if (choice == 0) {
            cout << "感谢使用，再见！" << endl;
            break;
        }

        switch (choice) {
            case 1: handleAddCard(); break;
            case 2: handleQueryCard(); break;
            case 3: handleLogon(); break;
            case 4: handleSettle(); break;
            case 5: handleAddMoney(); break;
            case 6: handleRefundMoney(); break;
            case 7: handleQueryStatistics(); break;
            case 8: handleAnnulCard(); break;
        }
        pressEnterToContinue();
        clearScreen();
    }
}