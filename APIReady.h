#pragma once
#ifndef APIREADY_H
#define APIREADY_H

extern "C" DWORD WaitForAPIReady(
  DWORD dwAPISlotIndex,
  DWORD dwTimeout
);

#define SH_WIN32        0
#define SH_CURTHREAD    1
#define SH_CURPROC      2
#define SH_CURTOKEN     3
#define HT_EVENT        4 // Event handle type
#define HT_MUTEX        5 // Mutex handle type
#define HT_APISET       6 // kernel API set handle type
#define HT_FILE         7 // open file handle type
#define HT_FIND         8 // FindFirst handle type
#define HT_DBFILE       9 // open database handle type
#define HT_DBFIND       10 // database find handle type
#define HT_SOCKET       11 // WinSock open socket handle type
#define HT_CRITSEC      12 // Critical section
#define HT_SEMAPHORE    13 // Semaphore handle type
#define HT_FSMAP        14 // mapped files
#define HT_WNETENUM     15 // Net Resource Enumeration
#define HT_AFSVOLUME    16 // file system volume handle type
#define SH_GDI          80
#define SH_WMGR         81
#define SH_WNET         82 // WNet APIs for network redirector
#define SH_COMM         83 // Communications not COM
#define SH_FILESYS_APIS 84 // File system APIS
#define SH_SHELL        85
#define SH_DEVMGR_APIS  86 // File system device manager
#define SH_TAPI         87
#define SH_CPROG        88 // Handle to the specified API set
#define SH_SERVICES     90
#define SH_DDRAW        91
#define SH_D3DM         92


#endif
