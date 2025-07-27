#include "TestCreateProcessAPI.h"
#include <windows.h>
#include <stdio.h>
int TestCreateProcess() {
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFO);

    WCHAR cmdLine[] = L"notepad.exe";
    // 创建进程
    if (CreateProcessW(
        NULL,           // 应用程序名（从命令行中提取）
        cmdLine, // 命令行
        NULL,           // 进程安全属性
        NULL,           // 线程安全属性
        FALSE,          // 不继承句柄
        0,              // 默认标志
        NULL,           // 使用当前环境
        NULL,           // 使用当前目录
        &si,            // 启动信息
        &pi             // 进程信息
    )) {
        printf("记事本已启动，等待关闭...\n");

        // 等待进程结束
        WaitForSingleObject(pi.hProcess, INFINITE);

        // 关闭句柄
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        printf("记事本已关闭。\n");
        return 1;
    }
    else {
        printf("创建进程失败，错误代码：%d\n", GetLastError());
    }
    return 0;
 
}