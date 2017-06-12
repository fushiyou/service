/*
�������������������
1.������� MY_NAME �����޸ĳ��Լ���
2.������� MY_DESC �����޸ĳ��Լ���
3.Ȼ��ʵ��һ�� void (*PRUN_FUNCTION)(bool bRunning) �����ĺ���
4.�� ����ָ�� ����ȫ�ֱ��� g_runFunc
5.����Service_Run ��main�е������������� OK ��
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
