#pragma once
#ifndef ALARMS_HPP
#define ALARMS_HPP

#include "regext.h"

typedef struct
{
    BYTE        wFlags;
    WORD		wAlarmTime;
    BYTE        bActiveDays;
    DWORD       dwNotifyFlags;
    HANDLE      rghNotify[7];
    TCHAR       szAlarmText[1000];
    TCHAR       szSoundName[1000]; 
    FILETIME    ftOnetimeAlarm;
    DWORD		dwAlarmID;
} ALARM_INFO;

#define PUN_LED     1
#define PUN_VIBRATE 2
#define PUN_DIALOG  4
#define PUN_SOUND   8 
#define PUN_REPEAT  16

namespace Shell
{
    /* Since alarms must be read/written in special manner, let's emulate ms behaviour */
    class CAlarm
    {
    public:
        // Reads alarms from the registry.  
        HRESULT ReadAlarm(HKEY hkeyParent, int index)
        {
            _isValid = FALSE;
            ALARM_INFO *p = &_alarm;
            memset(p, 0, sizeof(ALARM_INFO));
            HRESULT hResult = S_OK;

            DWORD count;
            HKEY  key = NULL;
            DWORD dwDisposition;

            wchar_t keyName[50];
            wsprintf(keyName, L"%d", index);
            if (RegCreateKeyEx(hkeyParent, keyName, 0, L"Prefs", REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS, NULL, &key, &dwDisposition) == S_OK)
            {
                /* AlarmFlags */
                count = sizeof(p->wFlags);
                RegQueryValueEx(key, L"AlarmFlags", 0, NULL, (LPBYTE) &(p->wFlags), &count);

                /* AlarmTime */
                count = sizeof(p->wAlarmTime);
                RegQueryValueEx(key, L"AlarmTime", 0, NULL, (LPBYTE) &(p->wAlarmTime), &count);

                /* AlarmHandle */
                //count = sizeof(p->rghNotify);
                //RegQueryValueEx(key, L"AlarmHandle", 0, NULL, (LPBYTE) &(p->rghNotify), &count);

                /* AlarmNotFlags */
                count = sizeof(p->dwNotifyFlags);
                if (RegQueryValueEx(key, L"AlarmNotFlags", 0, NULL, (LPBYTE) &(p->dwNotifyFlags), &count) != ERROR_SUCCESS)
                {
                    p->dwNotifyFlags = PUN_LED | PUN_VIBRATE | PUN_REPEAT | PUN_DIALOG | PUN_SOUND;
                }

                /* AlarmDays */
                count = sizeof(p->bActiveDays);
                if (RegQueryValueEx(key, L"AlarmDays", 0, NULL, (LPBYTE) &(p->bActiveDays), &count) != ERROR_SUCCESS)
                {
                    p->bActiveDays = 0x7F;
                }

                /* AlarmOnetime */
                count = sizeof(p->ftOnetimeAlarm);
                if (RegQueryValueEx(key, L"AlarmOnetime", 0, NULL, (LPBYTE) &(p->ftOnetimeAlarm), &count) != ERROR_SUCCESS)
                {
                    p->ftOnetimeAlarm.dwLowDateTime = 0;
                    p->ftOnetimeAlarm.dwHighDateTime = 0;
                }

                /* AlarmText */
                hResult = RegistryGetString(key, NULL, L"AlarmText", p->szAlarmText, sizeof(p->szAlarmText) / sizeof(wchar_t));
                if (hResult != S_OK)
                {
                    hResult = E_FAIL;
                    goto out;
                }

                /* AlarmID */
                count = sizeof(p->dwAlarmID);
                if (RegQueryValueEx(key, L"AlarmID", 0, NULL, (LPBYTE) &(p->dwAlarmID), &count) != S_OK || count != sizeof(DWORD) ||
                    p->dwAlarmID == (DWORD)INVALID_HANDLE_VALUE)
                {
                    hResult = E_FAIL;
                    goto out;
                }

                /* AlarmSnd */
                RegistryGetString(key, NULL, L"AlarmSnd", p->szSoundName, sizeof(p->szSoundName) / sizeof(wchar_t));
            }
            else
            {
                hResult = E_FAIL;
            }

out:
            if (key)
                RegCloseKey(key);

            if (hResult == S_OK)
                _isValid = TRUE;

            return hResult;
        }

        // Writes alarm to registry. 
        HRESULT WriteAlarm(HKEY hkeyParent, int index)
        {
            ALARM_INFO *p = &_alarm;
            HRESULT hResult = S_OK;

            DWORD count;
            HKEY  key = NULL;
            DWORD dwDisposition;

            wchar_t keyName[50];
            swprintf(keyName, L"%d", index);
            if (RegCreateKeyEx(hkeyParent, keyName, 0, L"Prefs", REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS, NULL, &key, &dwDisposition) == S_OK)
            {
                HANDLE hMutex = CreateMutex(NULL, FALSE, L"AlarmUpdateMutex");
                if (WaitForSingleObject(hMutex, 5000) != WAIT_OBJECT_0)
                {
                    hResult = E_FAIL;
                    goto out;
                }

                /* AlarmFlags */
                count = sizeof(p->wFlags);
                RegSetValueEx(key, L"AlarmFlags", 0, REG_DWORD, (LPBYTE) &(p->wFlags), count);

                /* AlarmTime */
                count = sizeof(p->wAlarmTime);
                RegSetValueEx(key, L"AlarmTime", 0, REG_DWORD, (LPBYTE) &(p->wAlarmTime), count);


                /* AlarmHandle */
                //count = sizeof(p->rghNotify);
                //RegSetValueEx(key, L"AlarmHandle", 0, REG_BINARY, (LPBYTE) &(p->rghNotify), count);

                /* AlarmNotFlags */
                count = sizeof(p->dwNotifyFlags);
                if (RegSetValueEx(key, L"AlarmNotFlags", 0, REG_DWORD, (LPBYTE) &(p->dwNotifyFlags), count) != ERROR_SUCCESS)
                {
                    p->dwNotifyFlags = PUN_LED | PUN_VIBRATE | PUN_REPEAT | PUN_DIALOG | PUN_SOUND;
                }

                /* AlarmDays */
                count = sizeof(p->bActiveDays);
                if (RegSetValueEx(key, L"AlarmDays", 0, REG_DWORD, (LPBYTE) &(p->bActiveDays), count) != ERROR_SUCCESS)
                {
                    p->bActiveDays = 0x7f;
                }

                /* AlarmOnetime */
                count = sizeof(p->ftOnetimeAlarm);
                if (RegSetValueEx(key, L"AlarmOnetime", 0, REG_BINARY, (LPBYTE) &(p->ftOnetimeAlarm), count) != ERROR_SUCCESS)
                {
                    p->ftOnetimeAlarm.dwLowDateTime = 0;
                    p->ftOnetimeAlarm.dwHighDateTime = 0;
                }

                /* AlarmText */
                hResult = RegistrySetString(key, NULL, L"AlarmText", p->szAlarmText);
                if (hResult != S_OK)
                {
                    ReleaseMutex(hMutex);
                    CloseHandle(hMutex);
                    hResult = E_FAIL;
                    goto out;
                }

                /* AlarmID */
                count = sizeof(p->dwAlarmID);
                RegSetValueEx(key, L"AlarmID", 0, REG_DWORD, (LPBYTE) &(p->dwAlarmID), count);

                /* AlarmSnd */
                RegistrySetString(key, NULL, L"AlarmSnd", p->szSoundName);

                ReleaseMutex(hMutex);
                CloseHandle(hMutex);
            }
            else
            {
                hResult = E_FAIL;
            }

out:
            if (key)
                RegCloseKey(key);

            return hResult;
        }

        // Checks if alarm is valid.
        BOOL IsValid()
        {
            return _isValid;
        }

        // Force validation state change.
        void MakeValid(BOOL isValid)
        {
            _isValid = isValid;
        }

        // Removes existing notifications for current alarm and creates new.
        void CreateNotifications(int index)
        {
            wchar_t args[50];
            swprintf(args, L"-reset -alarm %d", index);
            CreateProcess(L"clocknot.exe", args, NULL, NULL, FALSE, 0, NULL, NULL, NULL, NULL);
        }

        // Gets current alarm info. Mainly for accessing private alarm data.
        ALARM_INFO GetAlarmInfo()
        {
            ALARM_INFO alarmInfo;
            memcpy(&alarmInfo, &_alarm, sizeof(ALARM_INFO));
            return alarmInfo;
        }

        // Sets current alarm info. Mainly for changing private alarm data.
        void SetAlarmInfo(ALARM_INFO alarmInfo)
        {
            memcpy(&_alarm, &alarmInfo, sizeof(ALARM_INFO));
        }

    private:
        ALARM_INFO _alarm; 
        BOOL _isValid;
    };


}

#endif
