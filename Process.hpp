#pragma once
#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <tlhelp32.h>
#pragma comment(lib,"toolhelp.lib")

#include "ProcessFunctions_Common.h"

namespace Diagnostics
{
	class Process
	{
	public:
		static HANDLE Start(wchar_t *processName, wchar_t *arguments, wchar_t *accountName = NULL, BOOL autoCloseHandle = true)
		{
			if (wcslen(processName) > 0)
			{
				CE_PROCESS_INFORMATION cpi;;
				memset(&cpi, 0, sizeof(cpi));
				cpi.cbSize = sizeof(cpi);
				if (accountName && wcsicmp(accountName, L"standard") == 0)
					accountName = NULL;
				cpi.lpwszAccountName = accountName;
				if (CeCreateProcessEx(processName, arguments, &cpi) == TRUE)
				{
					if (cpi.ProcInfo.hThread != INVALID_HANDLE_VALUE)
						CloseHandle(cpi.ProcInfo.hThread);
					if (cpi.ProcInfo.hProcess != INVALID_HANDLE_VALUE)
					{
						if (autoCloseHandle)
							CloseHandle(cpi.ProcInfo.hProcess);
						return cpi.ProcInfo.hProcess;
					}
				}
			}
			return INVALID_HANDLE_VALUE;
		}

		static bool IsRunning(wchar_t *processname)
		{
			HANDLE prss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS,0);
			bool result = false;
			PROCESSENTRY32 pre;
			memset(&pre, 0, sizeof(pre));
			pre.dwSize = sizeof(pre);

			if (Process32First(prss, &pre)) 
			{
				do 
				{
					if (wcsicmp(pre.szExeFile, processname) == 0) 
					{
						result = true;
						break;
					}
				} while (Process32Next(prss, &pre));
			}
			CloseToolhelp32Snapshot(prss);
			return result;
		}

		static bool TerminateIfRunning(wchar_t *processname)
		{
			HANDLE prss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS,0);
			bool result = false;
			PROCESSENTRY32 pre;
			memset(&pre, 0, sizeof(pre));
			pre.dwSize = sizeof(pre);

			if (Process32First(prss, &pre)) 
			{
				do 
				{
					if (wcsicmp(pre.szExeFile, processname) == 0) 
					{
						HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pre.th32ProcessID);
						if (hProcess)
						{
							if (!TerminateProcess(hProcess, 0))
							{
								CloseHandle(hProcess);
								break;
							}
							result = true;
						}
						break;
					}
				} while (Process32Next(prss, &pre));
			}
			CloseToolhelp32Snapshot(prss);
			return result;
		}

		static DWORD GetProcessID(wchar_t *processName)
		{
			if (processName == NULL)
				return 0;

			HANDLE prss = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS,0);

			DWORD result = 0;
			PROCESSENTRY32 pre;
			memset(&pre, 0, sizeof(pre));
			pre.dwSize = sizeof(pre);

			if (Process32First(prss, &pre)) 
			{
				do 
				{
					if (wcsicmp(pre.szExeFile, processName) == 0) 
					{
						result = pre.th32ProcessID;
						break;
					}
				} while (Process32Next(prss, &pre));
			}
			CloseToolhelp32Snapshot(prss);
			return result;
		}


	};
}

#endif
