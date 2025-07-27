#include "TestCreateProcessAPI.h"
#include <windows.h>
#include <stdio.h>
int TestCreateProcess() {
    STARTUPINFOW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.cb = sizeof(STARTUPINFO);

    WCHAR cmdLine[] = L"notepad.exe";
    // ��������
    if (CreateProcessW(
        NULL,           // Ӧ�ó�������������������ȡ��
        cmdLine, // ������
        NULL,           // ���̰�ȫ����
        NULL,           // �̰߳�ȫ����
        FALSE,          // ���̳о��
        0,              // Ĭ�ϱ�־
        NULL,           // ʹ�õ�ǰ����
        NULL,           // ʹ�õ�ǰĿ¼
        &si,            // ������Ϣ
        &pi             // ������Ϣ
    )) {
        printf("���±����������ȴ��ر�...\n");

        // �ȴ����̽���
        WaitForSingleObject(pi.hProcess, INFINITE);

        // �رվ��
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        printf("���±��ѹرա�\n");
        return 1;
    }
    else {
        printf("��������ʧ�ܣ�������룺%d\n", GetLastError());
    }
    return 0;
 
}