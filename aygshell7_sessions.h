#pragma once

#ifndef __AYGSHELL7_H__
#define __AYGSHELL7_H__

extern "C" 
{
    HRESULT SHLaunchSessionByUri(LPCWSTR szTaskUri, HANDLE* lphSession);
    HRESULT SHLaunchTask(LPCWSTR pszUri, LPCWSTR pszCmdLineArguments, HANDLE* lphSession);
    HRESULT SHLaunchSession(ULONGLONG ulAppId, LPCWSTR pszTokenId, HANDLE* lphSession);
    HRESULT LaunchCpl(LPCTSTR pszCpl);
    HRESULT SHLaunchTask(LPCWSTR pszUri, LPCWSTR pszArguments, HANDLE* lphSession);
}

#ifndef TASKID
typedef DWORD TASKID;
#endif

typedef struct _PAGEINFO
{
    TASKID taskId;
    TASKID sessionId;
    DWORD dwProcessId;
    DWORD hPageHandle;
    HWND hwnd;
    DWORD orientations;
    wchar_t szTaskUri[1024];
    wchar_t szPageId[256];
} PAGEINFO;

extern "C" HRESULT SHGetForegroundPageInfo(PAGEINFO* pPageInfo);

#endif
