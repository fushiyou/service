/*
看这里，看这里，看这里。。。
1.把下面的 MY_NAME 名字修改成自己的
2.把下面的 MY_DESC 名字修改成自己的
3.然后实现一个 void (*PRUN_FUNCTION)(bool bRunning) 这样的函数
4.把 函数指针 赋给全局变量 g_runFunc
5.调用Service_Run 把main中的两个参数传入 OK 啦
*/
#pragma once
#pragma comment(lib, "advapi32.lib")

#include <tchar.h>

#define MY_NAME "ChangePassword"
#define MY_DESC "Change Windows password"
typedef void (*PRUN_FUNCTION)(bool *pbRunning);

#ifdef UNICODE
typedef wchar_t YCHAR;
#define _YTEXT(x) __T(x)
#define Ystrcpy(desbuf, dessize, srcbuf) wcscpy_s(desbuf, dessize, srcbuf)
#define Ystrlen(str) wcslen(str)
#define Ysprintf(buf,len,format,...)  swprintf_s(buf,len,format,##__VA_ARGS__)
#else
typedef char YCHAR;
#define _YTEXT(x) (x)
#define Ystrcpy(desbuf, srcbuf) strcpy(desbuf, srcbuf)
#define Ystrlen(str) strlen(str)
#define Ysprintf(buf,len,format,...)  sprintf_s(buf,len,format,##__VA_ARGS__);
#endif // !UNICODE

#define SERVICE_NAME  _YTEXT(MY_NAME)
#define SERVICE_DESC  _YTEXT(MY_DESC)

#define AUTOMOUNT_ACCEPTED_CONTROLS\
    (SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_SESSIONCHANGE)

extern "C"
    int Service_Run(int argc, YCHAR* argv[]);

void SetRunFun(PRUN_FUNCTION pfun);

extern bool g_Running;
