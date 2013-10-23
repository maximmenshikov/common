#pragma once

/* Types */
typedef HANDLE SHELL_PAGEMANAGER;
typedef HANDLE SHELL_PAGEMANAGER_CALLBACK;
typedef HANDLE SHELL_PAGE_CALLBACK;
typedef HANDLE SHELL_PAGE;
typedef HANDLE SHELL_SYSTEM_TRAY;
typedef HANDLE SHELL_APPLICATION_BAR;
typedef DWORD TASKID;

/* Enumerations */
namespace Shl
{

	namespace Direction
	{
		typedef enum
		{
			Forward,
			Back,
			Invalid,
		}Direction;
	};

	namespace Directions
	{
		typedef enum
		{
			Forward        = 1,
			Back           = 2,
			ForwardAndBack = Forward | Back
		}Directions;
	};

	namespace NavigationType
	{
		typedef enum
		{
			To,
			Away
		}NavigationType;
	};

	namespace NavigationTypes
	{
		typedef enum
		{
			To             = 1,
			Away           = 2,
			ToAndAway      = 3
		}NavigationTypes;
	};

	namespace AnimationType
	{
		enum AnimationType
		{
			None,
			Default,
		};
	};

	namespace CallbackThreading
	{
		enum CallbackThreading
		{
			SameThread = 0,
			AnyThread = 1,
		};
	};

	namespace Obscurity
	{
		enum Obscurity
		{
			Obscured,
			PartiallyObscured,
			NotObscured,
		};
	};    

	namespace Orientation
	{
		enum Orientation
		{
			Angle0 = 1,
			Angle90 = 2,
			Angle180 = 4 ,
			Angle270 = 8,
		};
	}

	namespace KeyboardPosition 
	{
		enum KeyboardPosition
		{
			Closed,
			Open,
		};
	}

	namespace AnimationIds
	{
		enum AnimationId
		{
			Invalid = 0,
			HideStart = 1,
			ShowStart = 2,
			First = 3
		};
	}
}

using namespace Shl;
typedef void (*OnNavigateTo)(SHELL_PAGE hPage, Direction::Direction direction, SHELL_PAGE hPagePrevious);
typedef void (*OnNavigateAway)(SHELL_PAGE hPage, Direction::Direction direction, SHELL_PAGE hPageNext); 
typedef void (*OnRemove)(SHELL_PAGE hPage);
typedef void (*OnVisibleRegionChange)(SHELL_PAGE hPage, RECT rcVisibleRegion, DWORD dwDuration);
typedef void (*OnSipDeployChange)(SHELL_PAGE hPage, bool deployed);
typedef BOOL (*OnBackRequest)(SHELL_PAGE hPage);
typedef void (*OnUiOrientationChange)(Orientation::Orientation orientation, bool animate);
typedef void (*OnObscurityChange)(bool obscured);
typedef void (*OnLockStateChange)(bool locked);
typedef void (*OnShowRequest)(Direction::Direction direction, AnimationIds::AnimationId idAnimation, AnimationType::AnimationType animationType);
typedef void (*OnHideRequest)(Direction::Direction direction, AnimationIds::AnimationId idAnimation, SHELL_PAGE hPageNext);

typedef struct
{
	OnNavigateTo          onNavigateTo;
	OnNavigateAway        onNavigateAway;
	OnRemove              onRemove;
	OnVisibleRegionChange onVisibleRegionChange;
	OnSipDeployChange     onSipDeployChange;
	OnBackRequest         onBackRequest;
	OnUiOrientationChange onUiOrientationChange;
	OnObscurityChange     onObscurityChange;
	OnLockStateChange     onLockStateChange;
	OnShowRequest         onShowRequest;
	OnHideRequest         onHideRequest;
}PageCallbacks;


typedef void (*OnInvokeReturning)(SHELL_PAGEMANAGER_CALLBACK hPMC, TASKID qwTaskId, DWORD cbData);
typedef void (*OnCancelRequested)(SHELL_PAGEMANAGER_CALLBACK hPM);
typedef void (*OnPageStackReactivated)(SHELL_PAGEMANAGER_CALLBACK hPMC, DWORD cbParameters);
typedef void (*OnMemoryNotification)(SHELL_PAGEMANAGER_CALLBACK hPM);
typedef void (*OnResumePageRequest)(SHELL_PAGEMANAGER_CALLBACK hPM, SHELL_PAGE pPage);

typedef struct
{
	OnInvokeReturning onInvokeReturning;
	OnCancelRequested onCancelRequested;
	OnPageStackReactivated onPageStackReactivated;
	OnMemoryNotification onMemoryNotification;
	OnResumePageRequest onResumePageRequest;
}PageManagerCallbacks;

extern "C"
{
	// Page Manager-related Frame functions.
	HRESULT SHPMCreatePageManager(TASKID dwTaskId, SHELL_PAGEMANAGER* phPageManager);

	HRESULT SHPMCreatePageManagerEx(TASKID dwTaskId, 
		PageManagerCallbacks* pCallback,
		SHELL_PAGEMANAGER* phPageManager,
		SHELL_PAGEMANAGER_CALLBACK* phPageManagerCallback);

	HRESULT SHPMCreatePage(SHELL_PAGEMANAGER hPageManager, PageCallbacks *pCallback,  CallbackThreading::CallbackThreading callbackThreading, HWND hWnd, LPCWSTR pszName, SHELL_PAGE *phPage,
		SHELL_PAGE_CALLBACK *phPageCallback);

	HRESULT SHPMReleasePageManager(SHELL_PAGEMANAGER hPageManager);

	HRESULT SHPMNavigateBack(SHELL_PAGEMANAGER hPageManager);
	HRESULT SHPMNavigateBackToTargetPage(SHELL_PAGEMANAGER hPageManager, SHELL_PAGE hPage);
	HRESULT SHPMNavigateTo(SHELL_PAGEMANAGER hPageManager, SHELL_PAGE hPage);
	HRESULT SHPMNavigateToExternalPage(SHELL_PAGEMANAGER hPageManager, LPCWSTR pszPageUri,
		BOOL fIsInvocation, BYTE* pPageArgs, DWORD cbPageArgs);

	HRESULT SHPMRemovePage(SHELL_PAGEMANAGER hPageManager, SHELL_PAGE hPage);
	HRESULT SHPMResumePage(SHELL_PAGEMANAGER hPageManager, PageCallbacks *pCallbacks, CallbackThreading::CallbackThreading callbackThreading,
		HWND hWnd, SHELL_PAGE hPage, SHELL_PAGE_CALLBACK *phPageCallback);

	HRESULT SHPMSetPropertyBlob(SHELL_PAGEMANAGER hPageManager, PCWSTR pszPropertyName, const BYTE* pbData, size_t cbData);
	HRESULT SHPMGetPropertyBlob(SHELL_PAGEMANAGER hPageManager, PCWSTR pszPropertyName, LPBYTE pbData, size_t cbData, size_t* pcbActual);

	HRESULT SHPMSetPauseSupported(SHELL_PAGEMANAGER hPageManager, BOOL pauseSupported);
	HRESULT SHPMSetPauseOnLock(SHELL_PAGEMANAGER hPageManager, BOOL pauseOnLock);
	HRESULT SHPMGetPauseSupported(SHELL_PAGEMANAGER hPageManager, BOOL * pauseSupported);
	HRESULT SHPMSetTerminateOnCancel(TASKID dwTaskId, BOOL terminateOnCancel);
	HRESULT SHPMGetTerminateOnCancel(TASKID dwTaskId, BOOL* terminateOnCancel);
	HRESULT SHPMSetProgressIndicatorIsVisible(SHELL_PAGEMANAGER hPageManager, BOOL isVisible);
	HRESULT SHPMSetProgressIndicatorIsIndeterminate(SHELL_PAGEMANAGER hPageManager, BOOL isIndeterminate);
	HRESULT SHPMSetProgressIndicatorText(SHELL_PAGEMANAGER hPageManager, LPCWSTR pszText);
	HRESULT SHPMSetProgressIndicatorValue(SHELL_PAGEMANAGER hPageManager, float value);
	HRESULT SHPMSetOrientationQuery(SHELL_PAGEMANAGER hPageManager, BOOL fQuery);


	// Page-related Frame functions.
	HRESULT SHReleasePage(SHELL_PAGE hPage);

	HRESULT SHGetApplicationBar(SHELL_PAGE hPage, SHELL_APPLICATION_BAR* phAppbar) ;
	HRESULT SHSetApplicationBar(SHELL_PAGE hPage, SHELL_APPLICATION_BAR hAppbar);

	HRESULT SHSetPropertyBlob(SHELL_PAGE hPage, PCWSTR pszPropertyName, const BYTE* pbData, size_t cbData);
	HRESULT SHSetPropertyDword(SHELL_PAGE hPage, PCWSTR pszPropertyName, DWORD dwPropertyValue);
	HRESULT SHSetPropertyString(SHELL_PAGE hPage, PCWSTR pszPropertyName, PCWSTR pszPropertyValue);
	HRESULT SHGetPropertyBlob(SHELL_PAGE hPage, PCWSTR pszPropertyName, LPBYTE pbData, size_t cbData, size_t* pcbActual);
	HRESULT SHGetPropertyDword(SHELL_PAGE hPage, PCWSTR pszPropertyName, DWORD* pdwPropertyValue);
	HRESULT SHGetPropertyString(SHELL_PAGE hPage, PCWSTR pszPropertyName, PWSTR pszPropertyValue, size_t cchPropertyValue);

	HRESULT SHGetPreviousPage(SHELL_PAGE hPage, SHELL_PAGE* phPage, bool allowDehydrated, LPWSTR pszPageName, UINT cchPageName);
	HRESULT SHGetNextPage(SHELL_PAGE hPage, SHELL_PAGE* phPage);

	HRESULT SHSetSupportedOrientations(SHELL_PAGE hPage, DWORD orientation);

	HRESULT SHGetPageOrientation(SHELL_PAGE hPage, DWORD& po);

	HRESULT SHGetVisibleRegion(SHELL_PAGE hPage, RECT &rcVisibleRegion);

	HRESULT SHSetSystemTrayVisible(SHELL_PAGE hPage, BOOL visible);
	HRESULT SHSetOverrideAnimation(SHELL_PAGE hPage, NavigationTypes::NavigationTypes types, Directions::Directions directions, BOOL override);

	HRESULT SHShowDone(SHELL_PAGE hPage, AnimationIds::AnimationId idAnimation);
	HRESULT SHHideDone(SHELL_PAGE hPage, AnimationIds::AnimationId idAnimation);
}

namespace Shl
{
	class Frame
	{
	public:
		static HRESULT Initialize(void);
		static HRESULT Uninitialize(void);
		static HRESULT VerifyInitialized(void);    
	};

};

class CFrameInitializer
{
public:
	CFrameInitializer()
	{
		if (Shl::Frame::VerifyInitialized() != S_OK)
			Shl::Frame::Initialize();
	}

	~CFrameInitializer()
	{
		if (Shl::Frame::VerifyInitialized() == S_OK)
			Shl::Frame::Uninitialize();
	}
};

/* We don't want to make user worry about initialization */
static CFrameInitializer _FrameInitializer;
