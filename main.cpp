#include <iostream>
#include "menu.h"
#include "service.h"

// 如果是 Windows 系统，可以保留这部分防止中文乱码
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    // 1. 初始化系统（自动加载数据文件到内存容器中）
    initSystem();

    // 2. 启动用户交互界面主循环
    startBillingSystem();

    // 3. 安全退出并清理资源
    exitSystem();

    return 0;
}