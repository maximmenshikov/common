/*
    Header: pacmanclient.h
    Project: ComXapHandler
    State: public
*/
#pragma once
#ifndef PACMANCLIENT_H
#define PACMANCLIENT_H

#define PACMANCLIENT_PUBLIC_HEADER

typedef unsigned long long APPID;

typedef enum 
{
    LIFECYCLE_NOTIFICATION		= 1,
    TOKEN_NOTIFICATION			= 2,
    BNS_NOTIFICATION			= 4,
}PACMANCLIENT_NOTIFICATION_TYPE;


typedef enum
{
    TOKEN_CREATED				= 0,
    TOKEN_DELETED				= 1,
    TOKEN_UPDATED				= 2,

    TOKEN_APPVISIBLE			= 3,
    TOKEN_APPINVISIBLE			= 4,

    TOKEN_PINNED				= 5,
    TOKEN_UNPINNED				= 6
}PACMANCLIENT_TOKEN_NOTIFICATION_TYPE;

// Pacman notification structures.
typedef struct
{
    ULONG									cbSize;  
    PACMANCLIENT_NOTIFICATION_TYPE			notificationType;
    APPID									appId;
    GUID									productId;
}PACMANCLIENT_NOTIFICATION;

typedef struct
{
    PACMANCLIENT_NOTIFICATION				header;
    PACMANCLIENT_TOKEN_NOTIFICATION_TYPE	notificationType;
    DWORD									tokenType;
    ULONG									tokenIdLength;
    WCHAR									tokenId;
}PACMANCLIENT_TOKEN_NOTIFICATION;

typedef enum
{
    APP_INSTALL_STARTED = 0, 
    APP_INSTALL_FINISHED = 1,
    APP_UPDATE_STARTED = 2, 
    APP_UPDATE_FINISHED = 3,
    APP_UNINSTALL_STARTED = 4,
    APP_UNINSTALL_FINISHED = 5,
    APP_UPDATELICENSE_STARTED = 6,
    APP_UPDATELICENSE_FINISHED = 7,
    APP_DOWNLOAD_STARTED = 8,
    APP_DOWNLOAD_INPROGRESS = 9,
    APP_DOWNLOAD_FINISHED = 10, 
    APP_INSTALL_INPROGRESS = 11,
    APP_UPDATE_INPROGRESS = 12,
} PACMANCLIENT_LIFECYCLE_NOTIFICATION_TYPE;

typedef struct
{
    PACMANCLIENT_NOTIFICATION                       header;
    PACMANCLIENT_LIFECYCLE_NOTIFICATION_TYPE        status;
    HRESULT                                         hResult;
    USHORT                                          progress;
} PACMANCLIENT_LIFECYCLE_NOTIFICATION; 

typedef enum
{
    BNS_CREATED = 0,
    BNS_DELETED = 1,
    BNS_UPDATED = 2,
    BNS_LIMITREACHED = 3,
}PACMANCLIENT_BNS_NOTIFICATION_TYPE;

typedef struct
{
    PACMANCLIENT_NOTIFICATION               header;
    PACMANCLIENT_BNS_NOTIFICATION_TYPE      notificationType;
    GUID                                    productID;
    ULONG                                   cNotifications;
}PACMANCLIENT_BNS_NOTIFICATION;

class IApplicationInfo : public IUnknown
{
public:
    virtual APPID       get_AppID();
    virtual GUID*       get_ProductID();
    virtual GUID*       get_InstanceID();
    virtual GUID*       get_OfferID();
    virtual LPCWSTR     get_DefaultTask();
    virtual LPCWSTR     get_AppTitle();
    virtual LPCWSTR     get_AppIconPath();
    virtual BOOL        get_IsNotified();
    virtual DWORD       get_AppInstallType();
    virtual DWORD       get_State();
    virtual BOOL        get_IsRevoked();
    virtual BOOL        get_IsUpdateAvailable();
    virtual FILETIME*   get_InstallDate();
    virtual BOOL        get_IsUninstallable();
    virtual BOOL        get_IsThemable();
    virtual LPCWSTR     get_InstallPath();
    virtual LPCWSTR     get_IsolatedStorageRoot();
    virtual USHORT      get_Rating();
    virtual USHORT      get_Genre();
    virtual LPCWSTR     get_Publisher();
    virtual LPCWSTR     get_Author();
    virtual LPCWSTR     get_Description();
    virtual LPCWSTR     get_Version();
    virtual HRESULT     GetInvocationInfo(LPWSTR* ppszImageUrn, LPWSTR* ppszParameters);  
    virtual LPCWSTR     get_ImagePath();
};


class ITokenInfo : public IUnknown
{
public:
    virtual APPID           get_AppID();
    virtual const GUID&     get_ProductID();
    virtual LPCWSTR         get_TokenID();
    virtual DWORD           get_TemplateType();
    virtual BOOL            get_IsPromoted();
    virtual ULONG           get_Position();
    virtual BOOL            get_IsNotified();
    virtual BOOL            get_IsDefault();
    virtual LPCWSTR         get_TaskID();
    virtual DWORD           get_TokenType();
    virtual BOOL            get_IsThemable();
    virtual LPCWSTR         get_AppIconPath();
    virtual LPCWSTR         get_AppTitle();
    virtual ULONG           get_HubType();
    virtual BOOL            get_IsVisible(DWORD hubType);
    virtual ULONG           get_PropertyCount();
    virtual HRESULT         get_TokenData(PBYTE* ppbTokenData, ULONG* pcbTokenData);
    virtual HRESULT         get_PropertyAtIndex(ULONG ulIndex, ULONG* pulPropID, LPCWSTR* ppszValue);
    virtual HRESULT         GetInvocationInfo(LPWSTR* pszImageUrn, LPWSTR* pszParameters);
};

template <typename Type>
class IDatabaseIterator : public IUnknown
{
public:
    virtual HRESULT get_Next(Type* );
};

typedef IDatabaseIterator<ITokenInfo*> ITokenInfoEnumerator;
typedef IDatabaseIterator<IApplicationInfo*> IApplicationInfoEnumerator;
typedef IDatabaseIterator<LPWSTR> ITokenIDEnumerator;

class IToken : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE set_Property(ULONG propID, LPCWSTR value) =0;
    virtual HRESULT STDMETHODCALLTYPE set_InvocationInfo(LPCWSTR taskName, LPCWSTR taskParameters) = 0;
};

class ITokenManager : public IUnknown
{
public:
    virtual HRESULT GetAppIDFromProductID(GUID productID, APPID* appID) = NULL;
    virtual HRESULT MakeToken(APPID appID, LPCWSTR pszTokenID, DWORD templateType, IToken**) = NULL;
    virtual HRESULT AddToken(IToken*) = NULL;
    virtual HRESULT UpdateToken(IToken*) = NULL;
    virtual HRESULT DeleteToken(APPID appID, LPCWSTR tokenID) = NULL;
    virtual HRESULT GetToken(APPID appID, LPCWSTR tokenID, IToken**) = NULL;
    virtual HRESULT GetTokenIDEnum(APPID appID, ITokenIDEnumerator**) = NULL;
    virtual HRESULT GetTokenIDEnum2(APPID appID, ITokenIDEnumerator**) = NULL;
    virtual HRESULT CreateTokenWithLiveTokenSubscription(APPID appId, LPCWSTR tokenID,
        LPCWSTR remoteImageUrl, LPCWSTR remoteBackImageUrl, void* schedule,
        const LPBYTE tokenXml, DWORD tokenXmlLength) = NULL;
    virtual HRESULT IsTokenPromoted(APPID appID, LPCWSTR tokenID, BOOL* isPromoted) = NULL;
    virtual HRESULT UpdateTokenFromXML(APPID appID, PBYTE tokenXml, ULONG tokenXmlLength) = NULL;
    virtual HRESULT UpdateTokenFromXML(APPID appID, PBYTE tokenXml, ULONG tokenXmlLength, LPCWSTR backgroundImageUrl, LPCWSTR backBackgroundImageUrl) = NULL;
    virtual HRESULT RestoreDefaultTokenData(APPID appID, LPCWSTR tokenID) = NULL;
    virtual HRESULT MakeAppListToken(APPID appID, LPCWSTR appListTokenID, BOOL isThemeable, IToken** appListToken, LPCWSTR primaryTokenID, 
        DWORD primaryTokenTemplateType, IToken** primaryToken) = NULL;
};


class PMNotificationListener : public IUnknown
{
public:
    virtual void HandleTokenNotification(PACMANCLIENT_TOKEN_NOTIFICATION*) = NULL;
    virtual void HandleLifeCycleNotification(PACMANCLIENT_LIFECYCLE_NOTIFICATION*) = NULL;
    virtual void HandleBnsNotification(PACMANCLIENT_BNS_NOTIFICATION*) = NULL;
};

// System APIs
extern "C" 
{
    HRESULT	GetApplicationInfoByProductID(GUID, IApplicationInfo **);
    HRESULT GetApplicationInfo(APPID appID, IApplicationInfo**);

    HRESULT	GetAllApplications(IApplicationInfoEnumerator **);
    HRESULT	GetAllVisibleApplications(IApplicationInfoEnumerator **);
    HRESULT GetApplicationsOfHubType(int hubType, IApplicationInfoEnumerator **);

    BOOL	SetEventData(HANDLE hEvent, DWORD dwData);
    DWORD	GetEventData(HANDLE hEvent); 

    HRESULT PMBeginInstall(void *package);
    HRESULT PMBeginDeployPackage(void *package);

	HRESULT PMBeginUpdate(void *package);
    HRESULT PMBeginUpdateDeployedPackage(void *package);

    HRESULT PMBeginUninstall(GUID guid);

    HRESULT PMRegisterForNotification(PMNotificationListener *, int n1, int n2);
    HRESULT PMUnregisterFromNotification(PMNotificationListener *);

    HRESULT PMUpdateAppIconPath(GUID guid, wchar_t *path);
    HRESULT PMTerminateApplicationProcesses(GUID guid);

    HRESULT PMSetApplicationVisibility(const GUID& productID, BOOL fIsVisible);
    HRESULT GetTokenManager(ITokenManager** ppTokenManager);
    HRESULT SetTokenPromoted(APPID appID, LPCWSTR pszTokenID,  BOOL fPromoted);
    HRESULT SetTokenPosition(APPID appID, LPCWSTR pszTokenID,  ULONG ulPosition);
    HRESULT SetTokenNotifiedState(APPID appID, LPCWSTR pszTokenID, BOOL fNotified);
    HRESULT PMPinTokenToStart(APPID appID, LPCWSTR pszTokenId);
    HRESULT GetAllPromotedTokens(ITokenInfoEnumerator**);
    HRESULT GetAllAppListTokens(ITokenInfoEnumerator**);
    HRESULT GetTokenInfo(APPID appID, LPCWSTR pszTokenID, ITokenInfo**);
    HRESULT GetDefaultTokenInfo(APPID appID, ITokenInfo**);

}

namespace Shell
{
    class Applications
    {
    public:
        static void String2GUID(LPOLESTR str, GUID *guid)
        {
            if (str && guid)
            {
                wchar_t *targetStr = str;

                wchar_t strGuid[50];
                if (str[0] == L'{')
                {
                    targetStr = str;
                }
                else
                {
                    memset(strGuid, 0, sizeof(strGuid));
                    strGuid[0] = L'{';
                    wcscat_s(strGuid, 50, str);
                    wcscat_s(strGuid, 50, L"}");
                    targetStr = strGuid;
                }
                CLSIDFromString(targetStr, guid);
            }
        }
    };

}
#endif