#define _CRT_SECURE_NO_WARNINGS

#include "menu.h"
#include "tool.h"
#include "card_service.h"
#include "card_file.h"
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
#include <limits>

using namespace std;

// 系统标题
void printTitle()
{
    cout << "=================================" << endl;
    cout << "      欢迎使用计费管理系统 v1.0       " << endl;
    cout << "=================================" << endl;
    cout << "===============菜单===============" << endl;
}

// 输出菜单选项
void outputMenu()
{
    cout << "1. 新增卡" << endl;
    cout << "2. 查询卡" << endl;
    cout << "3. 上机" << endl;
    cout << "4. 下机" << endl;
    cout << "5. 充值" << endl;
    cout << "6. 退费" << endl;
    cout << "7. 查询统计" << endl;
    cout << "8. 注销卡" << endl;
    cout << "0. 退出" << endl;
    cout << "-------------------------------" << endl;
}

// 根据选择显示对应的菜单名称
void showSelectedOption(int choice)
{
    string optionName = "";
    switch (choice)
    {
    case 1:
        optionName = "新增卡";
        break;
    case 2:
        optionName = "查询卡";
        break;
    case 3:
        optionName = "上机";
        break;
    case 4:
        optionName = "下机";
        break;
    case 5:
        optionName = "充值";
        break;
    case 6:
        optionName = "退费";
        break;
    case 7:
        optionName = "查询统计";
        break;
    case 8:
        optionName = "注销卡";
        break;
    case 0:
        optionName = "退出";
        break;
    default:
        optionName = "未知选择";
        break;
    }
    cout << ">> 您选择的菜单功能是: [" << optionName << "]" << endl;
}

// 获取用户选择并验证
int getUserChoice()
{
    int choice;
    while (true)
    {
        cout << "请选择菜单选项 (0-8): ";
        if (cin >> choice)
        {
            if (choice >= 0 && choice <= 8)
            {
                return choice;
            }
            else
            {
                cout << "选择的菜单选项范围错误，请重新输入!\n"
                     << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
        }
        else
        {
            cout << "输入无效，请输入数字!\n"
                 << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// 模糊查询卡
void fuzzyQueryCard()
{
    char keyword[19];
    cout << "请输入卡号关键字: ";
    cin.ignore();
    cin.getline(keyword, 19);

    int count = 0;
    Card *result = queryCards(keyword, &count);

    cout << "=================================" << endl;
    cout << "模糊查询结果: " << endl;
    cout << "=================================" << endl;

    if (count == 0)
    {
        cout << "没有找到匹配的卡!" << endl;
    }
    else
    {
        for (int i = 0; i < count; i++)
        {
            cout << "卡号: " << result[i].aName << "\t";
            cout << "状态: ";
            switch (result[i].nStatus)
            {
            case 0:
                cout << "未上机" << "\t";
                break;
            case 1:
                cout << "正在上机" << "\t";
                break;
            case 2:
                cout << "已注销" << "\t";
                break;
            case 3:
                cout << "失效" << "\t";
                break;
            }
            cout << "余额: " << result[i].fBalance << " 元" << endl;
            cout << "-------------------------------" << endl;
        }
        cout << "共找到 " << count << " 张匹配的卡" << endl;
        delete[] result; // 释放内存
    }
    cout << "=================================" << endl;
}

// 查看所有卡
void listAllCards()
{
    cout << "=================================" << endl;
    cout << "所有已注册的卡: " << endl;
    cout << "=================================" << endl;

    if (cardList == nullptr || cardList->next == nullptr)
    {
        cout << "没有已注册的卡!" << endl;
    }
    else
    {
        lpCardNode current = cardList->next;
        int count = 0;
        while (current != nullptr)
        {
            if (current->data.nDel == 0)
            {
                cout << "卡号: " << current->data.aName << "\t";
                cout << "状态: ";
                switch (current->data.nStatus)
                {
                case 0:
                    cout << "未上机" << "\t";
                    break;
                case 1:
                    cout << "正在上机" << "\t";
                    break;
                case 2:
                    cout << "已注销" << "\t";
                    break;
                case 3:
                    cout << "失效" << "\t";
                    break;
                default:
                    cout << "未知" << "\t";
                    break;
                }
                cout << "余额: " << current->data.fBalance << " 元" << endl;
                cout << "-------------------------------" << endl;
                count++;
            }
            current = current->next;
        }
        cout << "共找到 " << count << " 张已注册的卡" << endl;
    }
    cout << "=================================" << endl;
}

// 新增卡
void addCard()
{
    char cardNumber[19];
    char password[9];
    float balance;

    cout << "请输入卡号(提示: 卡号长度不超过18位):  ";
    cin.ignore();
    cin.getline(cardNumber, 19);
    cout << "请输入密码(提示: 密码长度不超过8位):  ";
    cin.getline(password, 9);
    cout << "请输入开卡金额: ";
    cin >> balance;

    int result = addCard(cardNumber, password, balance);
    if (result == 0)
    {
        cout << "新增卡成功!" << endl;
    }
    else if (result == -1)
    {
        cout << "内存分配失败!" << endl;
    }
    else if (result == -2)
    {
        cout << "卡号已存在!" << endl;
    }
    else if (result == -3)
    {
        cout << "卡号或密码无效!" << endl;
    }
    else if (result == -4)
    {
        cout << "保存文件失败!" << endl;
    }
    else
    {
        cout << "新增卡失败!" << endl;
    }
}

// 查询卡
void queryCard()
{
    int queryType;
    cout << "请选择查询方式: " << endl;
    cout << "1. 精确查询(输入完整卡号)" << endl;
    cout << "2. 模糊查询(输入卡号关键字)" << endl;
    cout << "3. 查看所有已注册的卡" << endl;
    cout << "请选择: ";
    cin >> queryType;

    if (queryType == 1)
    {
        char cardNumber[19];
        cout << "请输入卡号: " << endl;
        cin.ignore();
        cin.getline(cardNumber, 19);

        Card *card = queryCard(cardNumber);
        if (card == nullptr)
        {
            cout << "卡不存在!" << endl;
            return;
        }

        cout << "=================================" << endl;
        cout << "卡号: " << card->aName << "\t";
        cout << "卡状态: ";
        switch (card->nStatus)
        {
        case 0:
            cout << "未上机" << "\t";
            break;
        case 1:
            cout << "正在上机" << "\t";
            break;
        case 2:
            cout << "已注销" << "\t";
            break;
        case 3:
            cout << "失效" << "\t";
            break;
        }
        cout << "余额: " << card->fBalance << " 元" << "\t";
        cout << "累计使用金额: " << card->fTotalUse << " 元" << "\t";
        cout << "使用次数: " << card->nUseCount << "   ";
        cout << "最后使用时间: " << formatTime(card->tLast) << endl;
        cout << "=================================" << endl;
    }
    else if (queryType == 2)
    {
        fuzzyQueryCard();
    }
    else if (queryType == 3)
    {
        listAllCards();
    }
    else
    {
        cout << "选择错误!" << endl;
    }
}

// 查询统计
void queryStatistics()
{
    StatisticsInfo statisticsInfo;
    int result = doQueryStatistics(&statisticsInfo);
    if (result != TRUE)
    {
        cout << "查询统计失败!" << endl;
        return;
    }

    cout << "=================================" << endl;
    cout << "统计信息如下" << endl;
    cout << "=================================" << endl;
    cout << "卡总数: " << statisticsInfo.nCardCount << endl;
    cout << "未上机卡数量: " << statisticsInfo.nNotInUseCount << endl;
    cout << "正在上机卡数量: " << statisticsInfo.nInUseCount << endl;
    cout << "已注销卡数量: " << statisticsInfo.nCancelledCount << endl;
    cout << "失效卡数量: " << statisticsInfo.nInvalidCount << endl;
    cout << "卡总余额: " << statisticsInfo.fTotalBalance << " 元" << endl;
    cout << "累计使用金额: " << statisticsInfo.fTotalUseAmount << " 元" << endl;
    cout << "累计使用次数: " << statisticsInfo.nTotalUseCount << endl;
    cout << "计费记录总数: " << statisticsInfo.nBillingCount << endl;
    cout << "未结算计费记录数: " << statisticsInfo.nUnsettledBillingCount << endl;
    cout << "已结算计费记录数: " << statisticsInfo.nSettledBillingCount << endl;
    cout << "充值记录数: " << statisticsInfo.nRechargeCount << endl;
    cout << "退费记录数: " << statisticsInfo.nRefundCount << endl;
    cout << "充值总金额: " << statisticsInfo.fRechargeAmount << " 元" << endl;
    cout << "退费总金额: " << statisticsInfo.fRefundAmount << " 元" << endl;
    cout << "=================================" << endl;
}

// 注销卡
void annul()
{
    char cardNumber[19];
    char password[9];
    MoneyInfo moneyInfo;

    cout << "请输入要注销的卡号: " << endl;
    cin.ignore();
    cin.getline(cardNumber, 19);
    cout << "请输入密码: " << endl;
    cin.getline(password, 9);

    int result = doAnnulCard(cardNumber, password, &moneyInfo);
    if (result == TRUE)
    {
        cout << "----------注销信息如下----------" << endl;
        cout << "卡号      退款金额" << endl;
        cout << moneyInfo.aCardName << "      " << moneyInfo.fMoney << endl;
    }
    else if (result == UNUSE)
    {
        cout << "注销失败：该卡正在使用或者已注销!" << endl;
    }
    else
    {
        cout << "注销卡失败!" << endl;
    }
}

// 上机操作
void logon()
{
    char cardNumber[19];
    char password[9];
    LogonInfo logonInfo;

    cout << "请输入卡号: " << endl;
    cin.ignore();
    cin.getline(cardNumber, 19);
    cout << "请输入密码: " << endl;
    cin.getline(password, 9);

    int result = doLogon(cardNumber, password, &logonInfo);
    if (result == TRUE)
    {
        cout << "----------上机信息如下----------" << endl;
        cout << "卡号      余额      上机时间" << endl;
        cout << logonInfo.aCardName << "      " << logonInfo.fBalance << "      " << formatTime(logonInfo.tLogon) << endl;
    }
    else if (result == FALSE)
    {
        cout << "上机失败!" << endl;
    }
    else if (result == UNUSE)
    {
        cout << "该卡正在使用或者已注销!" << endl;
    }
    else if (result == ENOUGHMONEY)
    {
        cout << "卡余额不足!" << endl;
    }
    else
    {
        cout << "上机失败!" << endl;
    }
}

// 下机操作
void settle()
{
    char cardNumber[19];
    char password[9];
    SettleInfo settleInfo;

    cout << "请输入卡号: " << endl;
    cin.ignore();
    cin.getline(cardNumber, 19);
    cout << "请输入密码: " << endl;
    cin.getline(password, 9);

    int result = doSettle(cardNumber, password, &settleInfo);
    if (result == TRUE)
    {
        cout << "----------下机信息如下----------" << endl;
        cout << "卡号      消费      余额      上机时间                下机时间" << endl;
        cout << settleInfo.aCardName << "      " << settleInfo.fAmount << "       " << settleInfo.fBalance << "      " << formatTime(settleInfo.tStart) << "      " << formatTime(settleInfo.tEnd) << endl;
    }
    else if (result == FALSE)
    {
        cout << "下机失败!" << endl;
    }
    else if (result == UNUSE)
    {
        cout << "该卡未上机!" << endl;
    }
    else if (result == ENOUGHMONEY)
    {
        cout << "卡余额不足!" << endl;
    }
    else
    {
        cout << "下机失败!" << endl;
    }
}

// 充值操作
void addMoney()
{
    char cardNumber[19];
    char password[9];
    float money;
    MoneyInfo moneyInfo;

    cout << "请输入卡号: " << endl;
    cin.ignore();
    cin.getline(cardNumber, 19);
    cout << "请输入密码: " << endl;
    cin.getline(password, 9);
    cout << "请输入充值金额: ";
    cin >> money;

    int result = doAddMoney(cardNumber, password, money, &moneyInfo);
    if (result == TRUE)
    {
        cout << "----------充值信息如下----------" << endl;
        cout << "卡号      充值金额      余额" << endl;
        cout << moneyInfo.aCardName << "        " << moneyInfo.fMoney << "         " << moneyInfo.fBalance << endl;
    }
    else if (result == UNUSE)
    {
        cout << "该卡状态不允许充值!" << endl;
    }
    else
    {
        cout << "充值失败!" << endl;
    }
}

// 退费操作
void refundMoney()
{
    char cardNumber[19];
    char password[9];
    MoneyInfo moneyInfo;

    cout << "请输入卡号: " << endl;
    cin.ignore();
    cin.getline(cardNumber, 19);
    cout << "请输入密码: " << endl;
    cin.getline(password, 9);

    int result = doRefundMoney(cardNumber, password, &moneyInfo);
    if (result == TRUE)
    {
        cout << "----------退费信息如下----------" << endl;
        cout << "卡号      退费金额      余额" << endl;
        cout << moneyInfo.aCardName << "      " << moneyInfo.fMoney << "      " << moneyInfo.fBalance << endl;
    }
    else if (result == ENOUGHMONEY)
    {
        cout << "卡余额不足!" << endl;
    }
    else if (result == UNUSE)
    {
        cout << "该卡状态不允许退费!" << endl;
    }
    else
    {
        cout << "退费失败!" << endl;
    }
}

// 系统主循环
void startBillingSystem()
{
    int choice = -1;
    while (true)
    {
        printTitle();
        outputMenu();
        choice = getUserChoice();

        // 验证选择范围
        if (choice < 0 || choice > 8)
        {
            cout << "选择的菜单选项范围错误\n"
                 << endl;
            continue;
        }

        showSelectedOption(choice);

        // 选择为 0 时退出循环
        if (choice == 0)
        {
            cout << ">> 系统即将退出，感谢使用!" << endl;
            break;
        }

        // 根据选择执行相应功能
        switch (choice)
        {
        case 1:
            addCard();
            break;
        case 2:
            queryCard();
            break;
        case 3:
            logon();
            break;
        case 4:
            settle();
            break;
        case 5:
            addMoney();
            break;
        case 6:
            refundMoney();
            break;
        case 7:
            queryStatistics();
            break;
        case 8:
            annul();
            break;
        default:
            cout << "该功能暂未实现!" << endl;
            break;
        }

        // 暂停一下让用户看到结果，然后再显示菜单
        cout << "\n按回车键继续..." << endl;
        cin.ignore();
        cin.get();
        system("cls"); // 清屏
    }
}
