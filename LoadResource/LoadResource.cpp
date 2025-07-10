#include <windows.h>
#include "resource.h"
#include <cstdio>
#include <iostream>


typedef BOOL(*RUN_CALC_FUNC)(); // 定义函数指针类型


BOOL LoadDllFromResource(int resourceId, LPCWSTR resourceType) {
    // 1. 查找资源
    HRSRC hResource = FindResource(NULL, MAKEINTRESOURCE(resourceId), resourceType);
    if (!hResource) {
        DWORD err = GetLastError();
        std::cout << "FindResource failed with error: " << err << std::endl;

        return 1;
    }

    // 2. 获取资源大小并加载
    DWORD dwSize = SizeofResource(NULL, hResource);  //获取大小
    HGLOBAL hGlobal = LoadResource(NULL, hResource);   //加载
    if (!hGlobal || !dwSize) {
        return 2;
    }

    // 3. 锁定资源并返回资源指针
    LPVOID pResourceData = LockResource(hGlobal);
    if (!pResourceData) {
        return 3;
    }

    //4.保存资源到指定文件
    FILE* fp = NULL;   //声名文件指针
    fopen_s(&fp, "DLL1.dll", "wb+");  //以二进制写入模式创建
    if (fp == NULL) {
        return 4;
    }
    fwrite(pResourceData,sizeof(char),dwSize,fp);  // 将资源数据写入文件
    fclose(fp);     // 确保数据写入磁盘并释放资源
    
    //5.加载DLL文件
    HMODULE hdll= LoadLibrary(L"DLL1.dll");
    if (!hdll) {
        //DeleteFile(L"DLL1.dll"); // 清理临时文件
        return 5;
    }
    //6.获取函数指针
    RUN_CALC_FUNC Runcalc=(RUN_CALC_FUNC)GetProcAddress(hdll, "Runcalc");
    if (!Runcalc) {
        return 6;
    }
    //7.调用函数
    Runcalc();
    FreeLibrary(hdll);

    return 0;
}

// 使用示例
int main() {
    BOOL result=LoadDllFromResource(IDR_DLL1, L"DLL");
    if (!result) {
        std::cout << "DLL loaded successfully!" << std::endl;
        //MessageBox(NULL, L"DLL loaded successfully!", L"Success", MB_OK);
    }
    else {
        std::cout << "Failed to load DLL! Error Code:"<< result << std::endl;
        //MessageBox(NULL, L"Failed to load DLL!"+result, L"Error", MB_OK);
    }
    return 0;
}