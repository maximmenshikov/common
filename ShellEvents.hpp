#pragma once
#include <windows.h>

typedef enum
{
    ForegroundProcessChange = 1,
    ForegroundStateChange
}ShellEvent;

namespace Shell
{
	class IShellEventCallback
	{
	public:
		virtual void OnForegroundProcessChange(DWORD processID)
		{
		}

		virtual void OnForegroundStateChange(BOOL foreground)
		{
		}

		virtual void OnDebugSessionStart(const GUID& productID)
		{
		}

		virtual void OnDebugSessionEnd(const GUID& productID)
		{
		}
	};
	
}

extern "C" HRESULT SHCreateShellEventListener(DWORD events, bool unk, Shell::IShellEventCallback *shellEventCallback, IUnknown **listener);

