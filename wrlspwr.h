/**************************************************************/
//
//  radiopwr.h: structures and functions for managing radio
//  devices
//
//
/**************************************************************/
#pragma once

// Types of radio device
typedef enum _RADIODEVTYPE
{
    POWER_MANAGED = 1,
    POWER_PHONE,
    POWER_BLUETOOTH,
	POWER_WIFI
} RADIODEVTYPE;

/*
// Types of radio device
typedef enum _RADIODEVTYPE
{
  RADIODEVICES_MANAGED = 1,
  RADIODEVICES_PHONE,
  RADIODEVICES_BLUETOOTH,
} RADIODEVTYPE;
*/
// whether to save before or after changing state
// whether to save before or after changing state
typedef enum _SAVEACTION
{
    POWER_DONT_SAVE = 0,
    POWER_PRE_SAVE,
    POWER_POST_SAVE,
} SAVEACTION;
// Flags for GetWirelessDevices 
#define WIRELESS_GET_DESIRED    1       // Read the stored states from the registry whilst enumerating devices
#define WIRELESS_NO_POWER_MAN   2       // DON'T Look for powermanagaed devices
#define WIRELESS_NO_PHONE       4   	// DON'T check the phone device


// Details of radio devices
struct RDD 
{
  RDD() : pszDeviceName(NULL), pNext(NULL), pszDisplayName(NULL) {}
  ~RDD() { LocalFree(pszDeviceName); LocalFree(pszDisplayName); }
  LPTSTR   pszDeviceName;  // Device name for registry setting.
  LPTSTR   pszDisplayName; // Name to show the world
  DWORD    dwState;        // ON/off/[Discoverable for BT]
  DWORD    dwDesired;      // desired state - used for setting registry etc.
  RADIODEVTYPE    DeviceType;         // Managed, phone, BT etc.
  RDD * pNext;    // Next device in list
}; //radio device details



#define THREAD_TIMEOUT  30000 // 30 seconds 

#define WRLS_TERMINATE_EVENT TEXT("WRLS_TERMINATE_EVENT")

// Flags to pass to the notify callback 
#define WLSCB_EMPTY 1       // The message queue is now empty. This may accompany one or none of the following flags
#define WLSCB_ADD   2       // device has been added
#define WLSCB_REMOVE 4       // device has been removed

typedef HRESULT (*PFN_WRLSPOWERSTATUSCHANGE )(RDD *pRD, DWORD dwFlags, LPARAM lParam);

struct WRLSNOTIFY
{
    PFN_WRLSPOWERSTATUSCHANGE pfnCallback;
    BOOL    bGetDesired;
    LPARAM  lParam;
};

enum BTH_RADIO_MODE {
  BTH_POWER_OFF,
  BTH_CONNECTABLE,
  BTH_DISCOVERABLE
};

// Exported functions
extern "C"
{
// retrieves the actual state of the phone radio
BOOL GetPhoneState(BOOL *pbState);  

// checks a list of devices to see if any are on
BOOL AnyDevicesOn(RDD *pRootDevice);

// gets the desired state of a device from the registry
HRESULT GetRadioStoredState(RDD*  pDev, DWORD *dwOn);

// updates the desired state of a device in the registry
HRESULT UpdateRadioStoredState(RDD*  pDev, DWORD dwState);

// sets a wireless device's state, optionally saving the desired state before or after the change
HRESULT ChangeRadioState(RDD* pDev, DWORD dwState, SAVEACTION sa);

// sets a wireless device's state, which persists across reboots
// only POWER_WIFI and POWER_PHONE are supported by this API.
HRESULT SetAndPersistRadioState(RADIODEVTYPE eRadioType, DWORD dwState);

// gets the current device's state
// only POWER_WIFI and POWER_PHONE are supported by this API.
HRESULT GetCurrentRadioState(RADIODEVTYPE eRadioType, DWORD* pdwState);

// attempts to repair the device 
HRESULT RepairRadio(RDD* pDev);

// retrieves the overall 'wireless on/off' state from the registry
HRESULT GetWirelessState(BOOL *bState);

// returns a linked list of the wireless devices in the system. Optionally populates the desired states.
HRESULT GetWirelessDevices(RDD **pDevices, DWORD dwFlags);

// Switches the overall wireless state to 'on' or 'off', changing the devices accordingly. When switching on, 
// devices are set to their desired state stored in the registry. 
HRESULT ChangeWirelessState(BOOL bOn);

// checks if any devices in a list are on.
BOOL AnyDevicesOn(RDD *pRootDevice);

// frees a list of radio devices
void FreeDeviceList(RDD *pRoot);

// Gets collective name for a list of wireless devices
HRESULT GetNameForWirelessDevices(RDD *pDevices, LPTSTR *pName);

// Power down any wireless that should be powered down before device power off.
HRESULT SetPowerDownWirelessState();


int BthSetMode(
      DWORD
      dwMode
);

int BthGetMode(
      DWORD*
      pdwMode
);

}