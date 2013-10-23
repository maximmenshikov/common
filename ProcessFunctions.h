#pragma once
#ifndef PROCESSFUNCTIONS_H
#define PROCESSFUNCTIONS_H

#include "ProcessFunctions_Common.h"

#ifndef ACCTID
typedef DWORD ACCTID;
typedef DWORD* PACCTID;
#endif

extern "C" 
{
	BOOL CeGetProcessAccount(HMODULE hModule, PACCTID account, DWORD cbSize);
	BOOL CeGetOwnerAccount(HANDLE hProcess, PACCTID accountId, DWORD cbSize);
}
				   
#define TCB L"S-1-5-112-0-0-1"
			
#endif
