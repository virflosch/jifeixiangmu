#include <iostream>
#include <windows.h>
#include "menu.h"
#include "service.h"

using namespace std;

int main()
{
    // 设置控制台输出为UTF-8编码
    SetConsoleOutputCP(CP_UTF8);

    // 初始化系统
    initSystem();

    // 启动计费管理系统
    startBillingSystem();

    // 退出系统
    exitSystem();

    return 0;
}
