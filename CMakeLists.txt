# 指定 CMake 最低版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称
project(BillingSystem)

# 设置 C++ 标准为 C++11（因为我们使用了基于范围的 for 循环等现代特性）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 确保所有包含头文件的目录被找到（当前目录）
include_directories(.)

# 收集所有的源文件
set(SOURCE_FILES
    main.cpp
    menu.cpp
    service.cpp
    billing_service.cpp
    card_service.cpp
    billing_file.cpp
    card_file.cpp
    money_file.cpp
    tool.cpp
)

# 生成可执行文件
add_executable(BillingSystem ${SOURCE_FILES})

# 为了防止 Windows 控制台中文乱码，加上编译选项
if(WIN32)
    target_compile_options(BillingSystem PRIVATE "-fexec-charset=UTF-8") 
endif()