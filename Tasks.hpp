#pragma once

#include "PageManager.hpp"
#include "CPage.hpp"
#include "aygshell7_sessions.h"

static void _PageManager_OnNavigateTo(SHELL_PAGE hPage, Direction::Direction direction, SHELL_PAGE hPagePrevious)
{
}

static void _PageManager_OnNavigateAway(SHELL_PAGE hPage, Direction::Direction direction, SHELL_PAGE hPageNext)
{
}

static void _PageManager_OnRemove(SHELL_PAGE hPage)
{
}

static void _PageManager_OnVisibleRegionChange(SHELL_PAGE hPage, RECT rcVisibleRegion, DWORD dwDuration)
{
}

static void _PageManager_OnSipDeployChange(SHELL_PAGE hPage, bool deployed)
{

}

static BOOL _PageManager_OnBackRequest(SHELL_PAGE hPage)
{
	return TRUE;
}

static void _PageManager_OnUiOrientationChange(Orientation::Orientation orientation, bool animate)
{
}

static void _PageManager_OnObscurityChange(bool obscured)
{
}

static void _PageManager_OnLockStateChange(bool locked)
{
}

static void _PageManager_OnShowRequest(Direction::Direction direction, AnimationIds::AnimationId idAnimation, AnimationType::AnimationType animationType)
{
}

static void _PageManager_OnHideRequest(Direction::Direction direction, AnimationIds::AnimationId idAnimation, SHELL_PAGE hPageNext)
{
}

static PageCallbacks _pageCallbacks = {
    _PageManager_OnNavigateTo,
    _PageManager_OnNavigateAway,
    _PageManager_OnRemove,
    _PageManager_OnVisibleRegionChange,
    _PageManager_OnSipDeployChange,
    _PageManager_OnBackRequest,
    _PageManager_OnUiOrientationChange,
    _PageManager_OnObscurityChange,
    _PageManager_OnLockStateChange,
    _PageManager_OnShowRequest,
    _PageManager_OnHideRequest
};


static void _PageManager_OnInvokeReturning(SHELL_PAGEMANAGER_CALLBACK hPMC, TASKID qwTaskId, DWORD cbData)
{
}

static void _PageManager_OnCancelRequested(SHELL_PAGEMANAGER_CALLBACK hPM)
{
}

static void _PageManager_OnPageStackReactivated(SHELL_PAGEMANAGER_CALLBACK hPMC, DWORD cbParameters)
{
}

static void _PageManager_OnMemoryNotification(SHELL_PAGEMANAGER_CALLBACK hPM)
{
}

static void _PageManager_OnResumePageRequest(SHELL_PAGEMANAGER_CALLBACK hPM, SHELL_PAGE pPage)
{
}

static PageManagerCallbacks _pageManagerCallbacks = {
    _PageManager_OnInvokeReturning,
    _PageManager_OnCancelRequested,
    _PageManager_OnPageStackReactivated,
    _PageManager_OnMemoryNotification,
    _PageManager_OnResumePageRequest
};


class CRunExePage : public CPage 
{
	wchar_t _pageName[128];
	bool _pageCreated;
public:
	CRunExePage(wchar_t *name)
	{
		wcscpy(_pageName, name);
	}

	virtual ~CRunExePage()
	{
		if (_page != NULL)
		{
			Release();
		}
	}
	virtual void Create()
	{
		// we don't want to create new window, no reason for it. Let's use someone else's window.
		HWND hWnd = FindWindow(L"RenderWindow", NULL);
		if (hWnd == NULL)
		{
			hWnd = FindWindow(L"Tray", NULL);
		}

		SHELL_PAGEMANAGER_CALLBACK pmCallback = NULL;
		SHPMCreatePageManagerEx(0, &_pageManagerCallbacks, &_pageManager, &pmCallback);

		SHELL_PAGE_CALLBACK pageCallback = NULL;
		SHPMCreatePage(_pageManager, &_pageCallbacks, Shl::CallbackThreading::AnyThread, hWnd, _pageName, &_page, &pageCallback);
		SHSetOverrideAnimation(_page, NavigationTypes::ToAndAway, Directions::ForwardAndBack, TRUE);
		SHPMNavigateTo(_pageManager, _page);
		SHSetSupportedOrientations(_page, Orientation::Angle0);
		SHSetSystemTrayVisible(_page, FALSE);
		SHPMSetPauseSupported(_pageManager, FALSE);
		SHPMSetPauseOnLock(_pageManager, FALSE);
	}

	virtual void Release()
	{
		SHPMNavigateBack(_pageManager);
		SHReleasePage(_page);
		SHPMReleasePageManager(_pageManager);
		_page = NULL;
		_pageManager = NULL;
	}

};

namespace Shell
{
	class Tasks
	{
	public:
		static bool IsForegroundPage(wchar_t *pageUri)
		{
			PAGEINFO pageinf;
			if (wcsicmp(pageinf.szTaskUri, pageUri) == 0)
				return true;
			return false;
		}

		static void StartSession(wchar_t *uri, bool standaloneExe = false)
		{
			if (wcslen(uri) > 0)
			{
				if (IsForegroundPage(uri) == false)
				{
					CRunExePage *page = NULL;
					if (standaloneExe)
					{
						page = new CRunExePage(L"ShellTasksStartSessionPage");
						if (page)
							page->Create();
					}
					HANDLE handle;
					SHLaunchSessionByUri(uri, &handle);
					CloseHandle(handle);
					if (standaloneExe && page)
					{
						page->Release();
						delete[] page;
					}
				}
			}
		}
	};
}
