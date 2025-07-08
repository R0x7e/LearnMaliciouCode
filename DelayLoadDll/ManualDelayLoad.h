#pragma once
#include <windows.h>
#include <iostream>

void ManualDelay() {
    // 1. 加载DLL
    HMODULE hDll = LoadLibraryA("Dll1.dll");
    if (!hDll) {
        std::cerr << "无法加载DLL1.dll，错误: " << GetLastError() << std::endl;
        return;
    }

    // 2. 获取函数地址
    auto pRuncalc = (void(*)())GetProcAddress(hDll, "Runcalc");
    if (!pRuncalc) {
        std::cerr << "找不到Runcalc函数" << std::endl;
        FreeLibrary(hDll);
        return;
    }

    // 3. 调用函数
    pRuncalc();

    // 4. 释放DLL
    FreeLibrary(hDll);
}

//int main() {
//    ManualDelayLoad();
//    return 0;
//}