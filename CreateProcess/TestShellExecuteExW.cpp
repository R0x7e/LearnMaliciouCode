#include "TestShellExecuteExW.h"
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
int TestShellExecuteExW() {
    SHELLEXECUTEINFOW sei = { 0 };
    sei.cbSize = sizeof(SHELLEXECUTEINFOW);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.hwnd = NULL;
    sei.lpVerb = L"open";
    sei.lpFile = L"notepad.exe";
    sei.lpParameters = NULL;
    sei.lpDirectory = NULL;
    sei.nShow = SW_SHOW;

    if (ShellExecuteExW(&sei)) {
        WaitForSingleObject(sei.hProcess, INFINITE);
        CloseHandle(sei.hProcess);
        printf("记事本已打开。\n");
        return 1;
    }
    else {
        printf("执行失败，错误代码：%d\n", GetLastError());
    }

    return 0;
}