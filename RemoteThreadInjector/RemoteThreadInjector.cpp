#include <iostream>
#include<windows.h>
#include <string>
#include <tchar.h>
#include <cstdlib>

//注入代码
bool InjectDLL(DWORD proccessID,const char* dllPath) {
    HANDLE hProcess = nullptr;      // 目标进程句柄
    HANDLE hThread = nullptr;       // 远程线程句柄
    LPVOID pRemoteMem = nullptr;    // 目标进程内存指针
    bool success = false;           // 操作结果

    //打开目标进程
    // 需要以下权限：
    // PROCESS_CREATE_THREAD - 创建远程线程
    // PROCESS_VM_OPERATION - 操作进程内存
    // PROCESS_VM_WRITE - 写入进程内存
    // PROCESS_QUERY_INFORMATION - 查询进程信息
    //PROCESS_ALL_ACCESS    完整访问权限
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE,proccessID);
    if (hProcess == nullptr) {
        std::cerr << "[!] Open Process failed：" << GetLastError() << std::endl;
        return FALSE;
    }

    //在目标进程中分配内存
    pRemoteMem = VirtualAllocEx(
        hProcess,                   //目标进程的句柄
        NULL,                     // 让系统决定分配地址
        strlen(dllPath)+1,            //内存大小（包括null终止符）
        MEM_COMMIT | MEM_RESERVE,   // 提交并保留内存
        PAGE_READWRITE              // 内存可读可写
        );
    if (pRemoteMem == nullptr) {
        std::cerr << "[!] VirtualAllocEx failed：" << GetLastError() << std::endl;
        return FALSE;
    }
    
    //将DLL路径写入到目标进程内存
    if (!WriteProcessMemory(
        hProcess,                  // 目标进程句柄
        pRemoteMem,                // 目标内存地址
        dllPath,           // 要写入的数据（dll路径）
        strlen(dllPath)+1,        // 数据大小（包括null终止符）
        nullptr                    // 返回写入字节数（不需要）
    ))
    {
        std::cerr << "[!] WriteProcessMemory failed: " << GetLastError() << std::endl;
        return FALSE;
    }


    //获取LoadLibraryA的函数地址
    LPVOID pLoadLibrary = reinterpret_cast<LPVOID>(
        GetProcAddress(
            GetModuleHandle(L"kernel32.dll"),
            "LoadLibraryW"
        )
        );
    if (pLoadLibrary == nullptr)
    {
        std::cerr << "[!] GetProcAddress failed: " << GetLastError() << std::endl;
        return  FALSE;
    }

    //创建远程线程执行LoadLibraryA，参数是我们写入的DLL路径
    hThread = CreateRemoteThread(
        hProcess,                  // 目标进程句柄
        nullptr,                   // 默认安全属性
        0,                         // 默认堆栈大小
        (LPTHREAD_START_ROUTINE)pLoadLibrary, // 线程函数地址
        pRemoteMem,                // 线程参数（DLL路径）
        0,                         // 创建标志（立即运行）
        nullptr                    // 线程ID（不需要）
    );
    

    if (hThread == nullptr)
    {
        std::cerr << "[!] CreateRemoteThread failed: " << GetLastError() << std::endl;
        return  FALSE;
    }
    // 这里等待INFINITE时间，确保DLL加载完成
    WaitForSingleObject(hThread, INFINITE);

    //检查线程退出码，确保LoadLibrary成功
    //线程退出码就是线程函数（这里是 LoadLibrary）的返回值，LoadLibrary返回值为0表示DLL加载失败
    DWORD exitCode = 0;
    if (!GetExitCodeThread(hThread, &exitCode))
    {
        std::cerr << "[!] GetExitCodeThread failed: " << GetLastError() << std::endl;
        return  FALSE;
    }
    if (exitCode == 0)
    {
        std::cerr << "[!] LoadLibrary failed in remote process: " << GetLastError()<< std::endl;
        return  FALSE;
    }
    return TRUE;

}


int main(int argc,char* argv[])
{
    if (argc != 2) {
        std::cout << "Uasge:"<<argv[0]<<" <PID>" << std::endl;
        return 1;
    }
    DWORD TargetPID = static_cast<DWORD>(std::stoul(argv[1]));


    const char* DLLpath = "Dll3.dll";
    bool result = InjectDLL(TargetPID,DLLpath);
    
    if (result) {
        std::cout << "DLLInject Success!" << std::endl;
    }
    else
    {
        std::cout << "DLLInject Failed!" << std::endl;
    }
    return 0;
}

