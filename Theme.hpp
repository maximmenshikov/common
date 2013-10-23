#pragma once

namespace Shell
{
	class Theme
	{
	public:

		static DWORD GetAccentColor()
		{
			DWORD dwTempColor = 0;
			wchar_t accentNum[50], themeNum[50];
			if (RegistryGetString(HKEY_LOCAL_MACHINE, L"ControlPanel\\Themes", L"CurrentAccent", accentNum, 50) == S_OK)
			{
				if (RegistryGetString(HKEY_LOCAL_MACHINE, L"ControlPanel\\Themes", L"CurrentTheme", themeNum, 50) == S_OK)
				{
					wchar_t regKey[500];
					swprintf(regKey, L"ControlPanel\\Themes\\%ls\\Accents", themeNum);
					if (RegistryGetDWORD(HKEY_LOCAL_MACHINE, regKey, accentNum, &dwTempColor) == S_OK)
					{
						dwTempColor = dwTempColor & 0xFFFFFF;
						dwTempColor = ((dwTempColor >> 16) & 0xFF) | (((dwTempColor >> 8) & 0xFF) << 8) | ((dwTempColor & 0xFF) << 16);
					}
				}
			}
			return dwTempColor;
		}
	};
}