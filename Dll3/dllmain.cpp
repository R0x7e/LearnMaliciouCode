﻿#include <windows.h>
#include "pch.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
            MessageBox(NULL,L"DLL加载成功",L"success", MB_OK);
        break;
    }
    return TRUE;
}


