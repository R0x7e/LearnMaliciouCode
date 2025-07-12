#include "pch.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>


// 共享数据段 (用于进程间通信)
#pragma data_seg(".shared")  // 开始定义共享数据段，命名为 ".shared"
HHOOK g_hHook = NULL;    // 在共享数据段中定义全局变量 g_hHook（钩子句柄）
#pragma data_seg()     // 结束共享数据段定义
#pragma comment(linker, "/SECTION:.shared,RWS")   // 告诉链接器：共享段具有读(R)、写(W)、共享(S)权限

// 目标PID
#define TARGET_PID 4960

// 全局保存DLL模块句柄
HMODULE g_hThisDll = NULL;

// DLL入口点
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        // 禁用线程通知 (可选)
        DisableThreadLibraryCalls(hModule);


        g_hThisDll = hModule;
        // 在这里可以执行恶意代码
        //MessageBoxA(NULL, "DLL Injected Successfully!", "Hook DLL", MB_OK);
        break;
    }
    return TRUE;
}

// 写入PID到文件的函数
void WritePidToFile() {
    // 获取当前进程ID
    DWORD pid = GetCurrentProcessId();

    // 构造文件路径 - 使用临时目录避免权限问题
    char path[MAX_PATH];
    GetTempPathA(MAX_PATH, path);
    strcat_s(path, "process_pids.log");

    // 打开文件追加写入
    std::ofstream outFile;
    outFile.open(path, std::ios_base::app);

    if (outFile.is_open()) {
        // 写入PID和时间戳
        SYSTEMTIME st;
        GetLocalTime(&st);
        outFile << "PID: " << pid << " - "
            << st.wYear << "-" << st.wMonth << "-" << st.wDay << " "
            << st.wHour << ":" << st.wMinute << ":" << st.wSecond << "\n";
        outFile.close();
    }
}

// 钩子过程函数
LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    // 获取当前进程ID
    DWORD currentPID = GetCurrentProcessId();
    if (nCode >= 0 and currentPID== TARGET_PID) {
        // 写入当前进程PID到文件
        WritePidToFile();
    }

    // 调用下一个钩子
    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

// 导出函数 - 设置钩子
extern "C" __declspec(dllexport) BOOL SetHook(DWORD threadId) {
    g_hHook = SetWindowsHookEx(WH_GETMESSAGE, HookProc, g_hThisDll, threadId);
    return g_hHook != NULL;
}

// 导出函数 - 移除钩子
extern "C" __declspec(dllexport) BOOL RemoveHook() {
    if (g_hHook) {
        return UnhookWindowsHookEx(g_hHook);
    }
    return FALSE;
}