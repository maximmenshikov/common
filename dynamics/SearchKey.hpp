#pragma once

#include "regext.h"
#include "..\pacmanclient.h"

namespace Dynamics
{
	namespace SearchKeyAction
	{
		enum SearchKeyAction
		{
			ACTION_NO = 0,
			ACTION_BING = 1,
			ACTION_SEARCH = 2,
			ACTION_RUNAPPLICATION = 3,
			ACTION_URL = 4,
			ACTION_EXE = 5,
			ACTION_STOCK = 6,
			ACTION_KEYEVENT = 7
		};
	}
	class SearchKey
	{
		static DWORD HashString(wchar_t *str, unsigned int key = 0x1505)
		{
			size_t length = 0;
			if (StringCchLength(str, STRSAFE_MAX_CCH, &length) != S_OK)
				return 0;
			if (!length)
				return 0;
			unsigned int hash = key;
			for (unsigned int x = 0;x < length; x++)
			{
				hash=hash + (hash << 5);
				hash=hash + str[x];
			}
			if (hash & 0x80000000)
				hash ^= 0x80000000;
			return hash;
		}

		static void ReadSetting(wchar_t *buf, wchar_t *name)
		{
			buf[0] = L'\0';
			RegistryGetString(HKEY_LOCAL_MACHINE, 
				L"Software\\OEM\\WebSearchOverride", 
				name, buf, 500);
		}

		static void ReadSetting(DWORD *dwResult, wchar_t *name)
		{
			*dwResult = 0;
			RegistryGetDWORD(HKEY_LOCAL_MACHINE, 
				L"Software\\OEM\\WebSearchOverride", 
				name, dwResult);
		}

		static void WriteSetting(DWORD val, wchar_t *name)
		{
			RegistrySetDWORD(HKEY_LOCAL_MACHINE, 
				L"Software\\OEM\\WebSearchOverride", 
				name, val);
		}

		static int xor(int letter, int cipher)
		{
			return letter ^ cipher;
		}

		static void xorwstr(wchar_t *str, int len, int cipher)
		{
			for (int x = 0; x < len; x++)
				str[x] = xor(str[x], cipher);
		}

		static void xorarray(DWORD *arr, int len, int cipher)
		{
			for (int x = 0; x < len; x++)
				arr[x] = xor(arr[x], cipher);
		}

		
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

		static void CopySecurityString(wchar_t *securityEncoded)
		{
			wchar_t buf[8] = {0x986E,0x9858,0x985E,0x9848,0x984F,0x9854,0x9849,0x9844};
			memcpy(securityEncoded, buf, sizeof(buf));
		}

	public:

		static DWORD MakeHash()
		{
			DWORD hash = 0;
			wchar_t buf[500];
			DWORD dwBuf = 0;

			ReadSetting(&dwBuf, L"ActionType");
			hash += dwBuf ^ 0x1019a;
			if (dwBuf == SearchKeyAction::ACTION_RUNAPPLICATION)
			{
				ReadSetting(buf, L"Uri");
				hash += HashString(L"3761c846-6601-4c1b-b8f1-d15707032910");
				hash += HashString(buf);
				hash += HashString(L"d7746628-60e5-4980-a09a-94bc0bcb363d");
			}
			else if (dwBuf == SearchKeyAction::ACTION_URL)
			{
				ReadSetting(buf, L"Url");
				hash += HashString(buf);
				hash += HashString(L"609daaec-919e-4684-af6f-af60f7dc4a4e");
			}
			else if (dwBuf == SearchKeyAction::ACTION_KEYEVENT)
			{
				ReadSetting(&dwBuf, L"KeyCode");
				hash += dwBuf ^ 0xdcfe0aab;
				hash += HashString(L"10d83798-444e-46c4-9524-eb100acb8233");
			}
			else if (dwBuf == SearchKeyAction::ACTION_RUNAPPLICATION)
			{
				ReadSetting(buf, L"ExePath");
				hash += HashString(L"cf746c0b-27a4-474e");
				hash += HashString(buf);
				hash += HashString(L"b7f7-0724ba4ffd7a");
				ReadSetting(buf, L"ExeArgs");
				hash += HashString(buf);
			}
			else
			{
				hash += HashString(L"48544710-28a4-426a-83f4-2f7c60f6c0bc256ccc5b-87cc-45db-bcab-268344e7399d");
			}
			return hash;
		}

		static bool IsValidHash()
		{
			wchar_t securityEncoded[8];
			CopySecurityString(securityEncoded);

			wchar_t securityDecoded[50];
			memset(securityDecoded, 0, sizeof(securityDecoded));
			memcpy(securityDecoded, securityEncoded, sizeof(securityEncoded));
			xorwstr(securityDecoded, sizeof(securityEncoded) / 2, 0xfe1a983d);

			DWORD realHash = MakeHash();
			DWORD statedHash = 0;
			ReadSetting(&statedHash, securityDecoded);
			if (realHash == statedHash)
				return true;
			return false;
		}

		static void WriteHash()
		{
			wchar_t securityEncoded[8];
			CopySecurityString(securityEncoded);

			wchar_t securityDecoded[50];
			memset(securityDecoded, 0, sizeof(securityDecoded));
			memcpy(securityDecoded, securityEncoded, sizeof(securityEncoded));
			xorwstr(securityDecoded, sizeof(securityEncoded) / 2, 0xfe1a983d);

			DWORD realHash = MakeHash();
			WriteSetting(realHash, securityDecoded);
		}

		static bool IsSearchKeySettingsXapPresent()
		{
			GUID guid;
			String2GUID(L"{5a3a52e5-6a6d-4f06-87af-60cb1bbaec68}", &guid);
			IApplicationInfo *info = NULL;
			GetApplicationInfoByProductID(guid, &info);

			if (info)
			{
				delete(info);
				return true;
			}
			return false;
		}
	};

}