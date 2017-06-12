#include "Service.h"
#include <windows.h>
#include <stdio.h>

SERVICE_STATUS_HANDLE g_status_handle = NULL;
SERVICE_STATUS g_service_status;
bool g_Running = false;
PRUN_FUNCTION g_runFunc = NULL;

DWORD WINAPI control_handler(DWORD control, DWORD event_type, LPVOID event_data,
                             LPVOID context)
{
    DWORD ret = NO_ERROR;

    switch (control)
    {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN: 
        {
            g_service_status.dwCurrentState = SERVICE_STOP_PENDING;
            SetServiceStatus(g_status_handle, &g_service_status);
            g_Running = false;
            break;
        }
    case SERVICE_CONTROL_INTERROGATE:
        SetServiceStatus(g_status_handle, &g_service_status);
        break;
    default:
        ret = ERROR_CALL_NOT_IMPLEMENTED;
    }
    return ret;
}

void WINAPI work(DWORD argc, TCHAR * argv[])
{
#ifndef _DEBUG
    //设置服务
    SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
    g_service_status.dwServiceType = SERVICE_WIN32;
    g_service_status.dwCurrentState = SERVICE_STOPPED;
    g_service_status.dwControlsAccepted = 0;
    g_service_status.dwWin32ExitCode = NO_ERROR;
    g_service_status.dwServiceSpecificExitCode = NO_ERROR;
    g_service_status.dwCheckPoint = 0;
    g_service_status.dwWaitHint = 0;

    g_status_handle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, 
        control_handler,
        NULL);

    g_service_status.dwCurrentState = SERVICE_START_PENDING;
    SetServiceStatus(g_status_handle, &g_service_status);

    // service running
    g_service_status.dwControlsAccepted |= AUTOMOUNT_ACCEPTED_CONTROLS;
    g_service_status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(g_status_handle, &g_service_status);
#endif


    g_Running = true;

    //==================================================================
    //功能实现

    if ( g_runFunc != NULL )
    {
        g_runFunc(&g_Running);
    }

#ifndef _DEBUG
    // service was stopped
    g_service_status.dwCurrentState = SERVICE_STOP_PENDING;
    SetServiceStatus(g_status_handle, &g_service_status);

    // service is stopped
    g_service_status.dwControlsAccepted &= ~AUTOMOUNT_ACCEPTED_CONTROLS;
    g_service_status.dwCurrentState = SERVICE_STOPPED;

    SetServiceStatus(g_status_handle, &g_service_status);
#endif //_DEBUG
    return ;
}

int start()
{
#ifndef _DEBUG
    SERVICE_TABLE_ENTRY service_table[] = {{SERVICE_NAME, work}, {0, 0}};
    return !!StartServiceCtrlDispatcher(service_table);
#else
    work(0, NULL);
    return 0;
#endif
}

bool install()
{
    bool ret = false;

    SC_HANDLE service_control_manager = OpenSCManager(0, 0, SC_MANAGER_CREATE_SERVICE);
    if (!service_control_manager) {
        printf("OpenSCManager failed\n");
        return false;
    }
    TCHAR path[_MAX_PATH + 1];
    if (!GetModuleFileName(0, path, sizeof(path) / sizeof(path[0]))) {
        printf("GetModuleFileName failed\n");
        CloseServiceHandle(service_control_manager);
        return false;
    }
    //FIXME: SERVICE_INTERACTIVE_PROCESS needed for xp only
    SC_HANDLE service = CreateService(service_control_manager, SERVICE_NAME,
        SERVICE_NAME, SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
        SERVICE_AUTO_START, SERVICE_ERROR_IGNORE, path,
        _YTEXT(""), 0, 0, 0, 0);
    if (service) {
        SERVICE_DESCRIPTION descr;
        descr.lpDescription = SERVICE_DESC;
        if (!ChangeServiceConfig2(service, SERVICE_CONFIG_DESCRIPTION, &descr)) {
            printf("ChangeServiceConfig2 failed\n");
        }
        CloseServiceHandle(service);
        printf("Service installed successfully\n");
        ret = true;
    } else if (GetLastError() == ERROR_SERVICE_EXISTS) {
        printf("Service already exists\n");
        ret = true;
    } else {
        printf("Service not installed successfully, error %ld\n", GetLastError());
    }
    CloseServiceHandle(service_control_manager);
    return ret;
}

bool uninstall()
{
    bool ret = false;

    SC_HANDLE service_control_manager = OpenSCManager(0, 0, SC_MANAGER_CONNECT);
    if (!service_control_manager) {
        printf("OpenSCManager failed\n");
        return false;
    }
    SC_HANDLE service = OpenService(service_control_manager, SERVICE_NAME,
        SERVICE_QUERY_STATUS | DELETE);
    if (!service) {
        printf("OpenService failed\n");
        CloseServiceHandle(service_control_manager);
        return false;
    }
    SERVICE_STATUS status;
    if (!QueryServiceStatus(service, &status)) {
        printf("QueryServiceStatus failed\n");
    } else if (status.dwCurrentState != SERVICE_STOPPED) {
        printf("Service is still running\n");
    } else if (DeleteService(service)) {
        printf("Service removed successfully\n");
        ret = true;
    } else {
        switch (GetLastError()) {
        case ERROR_ACCESS_DENIED:
            printf("Access denied while trying to remove service\n");
            break;
        case ERROR_INVALID_HANDLE:
            printf("Handle invalid while trying to remove service\n");
            break;
        case ERROR_SERVICE_MARKED_FOR_DELETE:
            printf("Service already marked for deletion\n");
            break;
        }
    }
    CloseServiceHandle(service);
    CloseServiceHandle(service_control_manager);
    return ret;
}

extern "C"
int Service_Run(int argc, YCHAR* argv[])
{
    if (argc > 1) 
    {
        if (lstrcmpi(argv[1], _YTEXT("install")) == 0)
        {
            install();
        }
        else if (lstrcmpi(argv[1], _YTEXT("uninstall")) == 0)
        {
            uninstall();
        }
        else if ((lstrcmpi(argv[1], _YTEXT("debug")) == 0))
        {
            work(0, NULL);
        }
        else
        {
            printf("Use: %s install / uninstall\n", SERVICE_NAME);
        }
    }
    else 
    {
        start();
    }
    return 0;
}

void SetRunFun(PRUN_FUNCTION pfun)
{
    g_runFunc = pfun;
}
