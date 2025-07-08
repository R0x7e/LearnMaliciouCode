#include <iostream>
#include "ManualDelayLoad.h"



/*
 * 链接器指令：指定要延迟加载的DLL
 *
 * /DELAYLOAD:Dll1.dll 表示Dll1.dll将采用延迟加载方式
 * 这个DLL不会在程序启动时加载，而是在首次调用其函数时加载
 */
//#pragma comment(linker, "/DELAYLOAD:Dll1.dll")
 /*
  * 链接器指令：链接延迟加载支持库
  *
  * delayimp.lib 提供了延迟加载的运行时支持
  * 包含__delayLoadHelper2等实现延迟加载机制的辅助函数
  */
//#pragma comment(lib, "Dll1.lib")


// 正确声明延迟加载的函数
/*
 * 声明要从DLL中延迟加载的函数
 *
 * extern "C" - 使用C语言链接约定，防止C++的名称修饰(name mangling)
 * __declspec(dllimport) - 告诉编译器这个函数将从DLL中导入
 * void Runcalc() - 函数原型，必须与DLL中的定义完全一致
 */
extern "C" __declspec(dllimport) void Runcalc();


int main()
{
    //ManualDelay();
    Runcalc();  // 调用DLL函数
    std::cout << "Hello World!\n";
}

