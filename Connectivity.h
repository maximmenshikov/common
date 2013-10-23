/* 
	Wireless Control class.
	Fully supports Microsoft Bluetooth Stack. Broadcomm stack support limited to HTC devices.
*/
#pragma once
#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#include "wrlspwr.h"
#include "regext.h"

namespace Connectivity
{
	class CWirelessControl
	{
	public:
		CWirelessControl()
		{
		}
		~CWirelessControl()
		{
		}

		/// <summary>
		/// Changes radio device state.
		/// </summary>
		/// <param name="dwDevice">Device ID</param>
		/// <param name="dwState">Desired state</param>
		/// <param name="sync">Synchronous or asynchronous work mode</param>
		/// <returns>S_OK to indicate success, all other values indicate failure</param>
		DWORD SetRadioState(DWORD dwDevice, DWORD dwState, BOOL sync = FALSE)
		{
			if (dwDevice != POWER_BLUETOOTH)
			{
				SetAndPersistRadioState((RADIODEVTYPE)dwDevice, dwState);
				if (dwDevice == POWER_WIFI)
				{
					if (dwState == 0)
					{
						RegistrySetDWORD(HKEY_LOCAL_MACHINE, L"System\\State\\Hardware", L"WiFi", 32);
					}
					else
					{
						DWORD dwState = 0;
						RegistryGetDWORD(HKEY_LOCAL_MACHINE, L"System\\State\\Hardware", L"WiFi", &dwState);
						RegistrySetDWORD(HKEY_LOCAL_MACHINE, L"System\\State\\Hardware", L"WiFi", dwState & 0x10);
					}
				}
				return 0;
			}
			RDD * pDevice = NULL;
			RDD * pTD;
			HRESULT hr;
			DWORD retval = 0;

			hr = GetWirelessDevices(&pDevice, 0);
			if(hr != S_OK) 
			{
				return -1;
			}
			if (pDevice)
			{
				pTD = pDevice;

				// loop through the linked list of devices
				while (pTD)
				{
					if  (pTD->DeviceType == dwDevice)
					{
						if (dwDevice == POWER_BLUETOOTH)
						{
							BthSetMode(dwState);
							hr = S_OK;
							retval = 0;
						}
					}

					pTD = pTD->pNext;

				}
				// Free the list of devices retrieved with    
				// GetWirelessDevices()
				FreeDeviceList(pDevice);
			}
			if(hr == S_OK)
				return retval;

			return -2;
		}

		/// <summary>
		/// Receives information about radio devices state
		/// </summary>
		/// <param name="bWifi">Pointer to DWORD that will receive WiFi state [optional]</param>
		/// <param name="bPhone">Pointer to DWORD that will receive Phone state [optional]</param>
		/// <param name="bBT">Pointer to DWORD that will receive Bluetooth state [optional]</param>
		/// <returns>S_OK to indicate success, all other values indicate failure</param>
		DWORD GetRadioStates(DWORD* bWifi, DWORD* bPhone, DWORD* bBT)
		{
			RDD * pDevice = NULL;
			RDD * pTD;

			HRESULT hr;
			DWORD retval = 0;

			hr = GetWirelessDevices(&pDevice, 0);

			if(hr != S_OK) 
			{
				return -1;
			}

			if (pDevice)
			{
				pTD = pDevice;

				while (pTD)
				{
					switch (pTD->DeviceType)
					{
					case POWER_MANAGED:
						if (bWifi)
							GetCurrentRadioState(POWER_WIFI, bWifi);
						break;
					case POWER_PHONE:
						if (bPhone)
							GetCurrentRadioState(POWER_PHONE, bPhone);
						break;
					case POWER_BLUETOOTH:
						if (bBT)
						{
							DWORD res = 0;
							BthGetMode(&res);
							if (res)
								*bBT = 1;
							else
								*bBT = 0;
						}
						break;
					default:
						break;
					}
					pTD = pTD->pNext; 
				}
				FreeDeviceList(pDevice);
			}

			if(hr == S_OK)
				return retval;

			return -2;
		}

	};
}
#endif