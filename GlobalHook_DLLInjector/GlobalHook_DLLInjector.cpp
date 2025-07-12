#include <windows.h>
#include <iostream>
#include <string>
#include <limits>  // 添加这个头文件以使用numeric_limits

// 定义函数指针类型
typedef BOOL(*SET_HOOK_FUNC)(DWORD);
typedef BOOL(*REMOVE_HOOK_FUNC)();

// 加载DLL并获取函数指针
bool LoadDllFunctions(HMODULE& hDll, SET_HOOK_FUNC& SetHook, REMOVE_HOOK_FUNC& RemoveHook) {
    // 加载DLL
    hDll = LoadLibrary(TEXT("DLL2.dll"));
    if (hDll == NULL) {
        std::cerr << "LoadLibrary failed: " << GetLastError() << std::endl;
        return false;
    }

    // 获取SetHook函数地址
    SetHook = (SET_HOOK_FUNC)GetProcAddress(hDll, "SetHook");
    if (SetHook == NULL) {
        std::cerr << "GetProcAddress(SetHook) failed: " << GetLastError() << std::endl;
        FreeLibrary(hDll);
        return false;
    }

    // 获取RemoveHook函数地址
    RemoveHook = (REMOVE_HOOK_FUNC)GetProcAddress(hDll, "RemoveHook");
    if (RemoveHook == NULL) {
        std::cerr << "GetProcAddress(RemoveHook) failed: " << GetLastError() << std::endl;
        FreeLibrary(hDll);
        return false;
    }

    return true;
}

// 显示菜单
void ShowMenu() {
    std::cout << "\n=== Hook 控制菜单 ===" << std::endl;
    std::cout << "1. 设置钩子(SetHook)" << std::endl;
    std::cout << "2. 移除钩子(RemoveHook)" << std::endl;
    std::cout << "3. 退出程序" << std::endl;
    std::cout << "请输入选择(1-3): ";
}

int main() {
    HMODULE hDll = NULL;
    SET_HOOK_FUNC SetHook = NULL;
    REMOVE_HOOK_FUNC RemoveHook = NULL;

    // 加载DLL和函数
    if (!LoadDllFunctions(hDll, SetHook, RemoveHook)) {
        return 1;
    }

    int choice = 0;
    bool running = true;

    while (running) {
        ShowMenu();
        std::cin >> choice;

        // 清除输入缓冲区
        std::cin.clear();
        // 添加NOMINMAX宏定义或者使用括号
        std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

        switch (choice) {
        case 1: // 设置钩子
            if (SetHook(0)) { // 0表示全局钩子
                std::cout << "钩子设置成功!" << std::endl;
            }
            else {
                DWORD err = GetLastError();
                std::cerr << "钩子设置失败! 错误代码: " << err << std::endl;
            }
            break;

        case 2: // 移除钩子
            if (RemoveHook()) {
                std::cout << "钩子移除成功!" << std::endl;
            }
            else {
                DWORD err = GetLastError();
                std::cerr << "钩子移除失败! 错误代码: " << err << std::endl;
            }
            break;

        case 3: // 退出程序
            running = false;
            std::cout << "程序即将退出..." << std::endl;
            break;

        default:
            std::cerr << "无效输入，请重新选择!" << std::endl;
            break;
        }
    }

    // 卸载DLL
    FreeLibrary(hDll);

    return 0;


}