#include <windows.h>
#include <tchar.h>
#include <iostream>

int main()
{
    // 创建一个唯一的互斥体名称，可以使用GUID或程序特定名称
    LPCTSTR mutexName = _T("TestMutex");

    // 尝试创建互斥体
    HANDLE hMutex = CreateMutex(NULL, TRUE, mutexName);

    if (hMutex == NULL)
    {
        std::cerr << "创建互斥体失败，错误代码: " << GetLastError() << std::endl;
        return 1;
    }


    // 检查是否已经存在该互斥体（即程序已经在运行）
    if (GetLastError() == ERROR_ALREADY_EXISTS)
        //GetLastError 是 Windows API 中的一个重要函数，用于获取调用线程的最后错误代码值。

    {
        std::cout << "程序已经在运行中，即将退出..." << std::endl;

        // 关闭互斥体句柄
        CloseHandle(hMutex);
        return 0;
    }

    // 程序主逻辑
    std::cout << "程序启动成功，这是唯一运行的实例。" << std::endl;
    std::cout << "按任意键退出..." << std::endl;
    std::cin.get();

    // 释放互斥体
    ReleaseMutex(hMutex);
    CloseHandle(hMutex);

    return 0;
}