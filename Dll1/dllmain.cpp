// 包含Windows API头文件，提供必要的函数和类型定义
#include <windows.h>
#include "pch.h"  // 这是Visual Studio生成的预编译头文件

// 添加导出函数（必须声明为C风格）
extern "C" __declspec(dllexport) void Runcalc() {
    WinExec("calc.exe", SW_SHOW);
}

/*
 * DLL主入口点函数 - 操作系统在DLL加载、卸载等情况下自动调用
 *
 * @param hModule 当前DLL模块的句柄，代表这个DLL在内存中的基地址
 * @param ul_reason_for_call 调用原因标志，指示为什么调用这个函数
 *       可能的值:
 *       - DLL_PROCESS_ATTACH:  当DLL被首次加载到进程地址空间时
 *       - DLL_PROCESS_DETACH:  当DLL从进程地址空间卸载时
 *       - DLL_THREAD_ATTACH:   当进程创建新线程时(线程将使用这个DLL)
 *       - DLL_THREAD_DETACH:   当线程退出时(线程曾使用过这个DLL)
 * @param lpReserved 保留参数，用于特殊情况
 *
 * @return BOOL 返回TRUE表示成功，FALSE表示失败(仅DLL_PROCESS_ATTACH时有效)
 *        如果返回FALSE，系统会终止进程并卸载DLL
 */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    // 根据调用原因执行不同的操作
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:  // DLL被加载到进程时
        /*
         * 使用WinExec函数启动Windows计算器
         * WinExec是Windows API，用于执行外部程序
         *
         * @param "calc.exe" 要执行的程序名
         *       系统会在PATH环境变量指定的目录中查找这个程序
         *       对于calc.exe，系统会直接找到Windows系统目录下的计算器
         *
         * @param SW_SHOW 显示窗口的标志
         *       其他可能的值:
         *       - SW_HIDE: 隐藏窗口
         *       - SW_MAXIMIZE: 最大化窗口
         *       - SW_MINIMIZE: 最小化窗口
         *
         * @return 如果函数成功，返回值大于31
         *         如果函数失败，返回值是错误代码
         *
         * 注意: WinExec是较老的API，现代程序建议使用CreateProcess或ShellExecute
         * 但这里使用WinExec因为它简单直观
         */
        //WinExec("notepad.exe", SW_SHOW);

        // 不需要处理其他情况，直接break
        break;

    case DLL_THREAD_ATTACH:   // 新线程创建时
        // 本例中不需要处理线程附加事件
        WinExec("mspaint.exe", SW_SHOW);
        break;

    case DLL_THREAD_DETACH:   // 线程退出时
        // 本例中不需要处理线程分离事件
        break;

    case DLL_PROCESS_DETACH:  // DLL被卸载时
        // 本例中不需要处理进程分离事件
        break;
    }

    // 返回TRUE表示DLL初始化成功
    return TRUE;
}

/*
 * 补充技术说明:
 * 1. 这个DLL被加载到任何进程时都会立即弹出计算器
 * 2. 在实际应用中，应该考虑更安全的程序启动方式(CreateProcess)
 * 3. 这种技术可以被用于合法用途(如插件系统)，也可能被恶意软件滥用
 * 4. 现代安全软件会监控此类行为，特别是从DLL中启动外部程序
 * 5. 在生产环境中，应该添加错误检查和更完善的启动逻辑
 */