#pragma once
#include <windows.h>
#include <iostream>

void ManualDelay() {
    // 1. ����DLL
    HMODULE hDll = LoadLibraryA("Dll1.dll");
    if (!hDll) {
        std::cerr << "�޷�����DLL1.dll������: " << GetLastError() << std::endl;
        return;
    }

    // 2. ��ȡ������ַ
    auto pRuncalc = (void(*)())GetProcAddress(hDll, "Runcalc");
    if (!pRuncalc) {
        std::cerr << "�Ҳ���Runcalc����" << std::endl;
        FreeLibrary(hDll);
        return;
    }

    // 3. ���ú���
    pRuncalc();

    // 4. �ͷ�DLL
    FreeLibrary(hDll);
}

//int main() {
//    ManualDelayLoad();
//    return 0;
//}