#ifndef ___PROCESS_FUNCTIONS____
#define ___PROCESS_FUNCTIONS____

typedef struct _CE_PROCESS_INFORMATION {
    WORD      cbSize;
    WORD      wUnknown;
    DWORD     dwFlags;
    LPCWSTR   lpwszAccountName;
    PROCESS_INFORMATION ProcInfo;
}CE_PROCESS_INFORMATION, *PCE_PROCESS_INFORMATION, *LPCE_PROCESS_INFORMATION;

extern "C" BOOL CeCreateProcessEx(LPWSTR lpwszPath, LPWSTR lpwszArguments, CE_PROCESS_INFORMATION *ce_process_information);

#endif
