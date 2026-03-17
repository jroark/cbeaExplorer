/*

	WilRapi.h

	Wil Remote API master include file for Windows CE

	2001,07/29

*/
#ifndef WILRAPI_H
#define WILRAPI_H

#ifndef EXTERN_C
#ifdef __cplusplus
    #define EXTERN_C    extern "C"
#else
    #define EXTERN_C    extern
#endif
#endif
#define IMPORT EXTERN_C __declspec(dllimport)
//#define EXPORT __declspec(dllexport)

/* Start of rapi.h */
//
// The Windows CE WIN32_FIND_DATA structure differs from the
// Windows WIN32_FIND_DATA stucture so we copy the Windows CE
// definition to here so that both sides match.
//
typedef struct _CE_FIND_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
    DWORD    dwOID;
    WCHAR    cFileName[MAX_PATH];
} CE_FIND_DATA, *LPCE_FIND_DATA;
typedef CE_FIND_DATA** LPLPCE_FIND_DATA;
//
// These are flags for CeFindAllFiles
//
#define FAF_ATTRIBUTES      ((DWORD) 0x01)
#define FAF_CREATION_TIME   ((DWORD) 0x02)
#define FAF_LASTACCESS_TIME ((DWORD) 0x04)
#define FAF_LASTWRITE_TIME  ((DWORD) 0x08)
#define FAF_SIZE_HIGH       ((DWORD) 0x10)
#define FAF_SIZE_LOW        ((DWORD) 0x20)
#define FAF_OID             ((DWORD) 0x40)
#define FAF_NAME            ((DWORD) 0x80)
#define FAF_FLAG_COUNT      ((UINT)  8)
#define FAF_ATTRIB_CHILDREN ((DWORD)			0x01000)
#define FAF_ATTRIB_NO_HIDDEN ((DWORD)			0x02000)
#define FAF_FOLDERS_ONLY    ((DWORD)			0x04000)
#define FAF_NO_HIDDEN_SYS_ROMMODULES ((DWORD)	0x08000)
#define FAF_GETTARGET	    ((DWORD)			0x10000)
#define FAD_OID             ((WORD) 0x01)
#define FAD_FLAGS           ((WORD) 0x02)
#define FAD_NAME            ((WORD) 0x04)
#define FAD_TYPE            ((WORD) 0x08)
#define FAD_NUM_RECORDS     ((WORD) 0x10)
#define FAD_NUM_SORT_ORDER  ((WORD) 0x20)
#define FAD_SIZE            ((WORD) 0x40)
#define FAD_LAST_MODIFIED   ((WORD) 0x80)
#define FAD_SORT_SPECS      ((WORD) 0x100)
#define FAD_FLAG_COUNT      ((UINT) 9)
#ifndef FILE_ATTRIBUTE_INROM
#define FILE_ATTRIBUTE_INROM        0x00000040
#endif
#ifndef FILE_ATTRIBUTE_ROMSTATICREF
#define FILE_ATTRIBUTE_ROMSTATICREF 0x00001000
#endif
#ifndef FILE_ATTRIBUTE_ROMMODULE
#define FILE_ATTRIBUTE_ROMMODULE    0x00002000
#endif
//
// The following is not a standard Windows CE File Attribute.
//
#ifndef FILE_ATTRIBUTE_HAS_CHILDREN
#define FILE_ATTRIBUTE_HAS_CHILDREN 0x00010000
#endif
#ifndef FILE_ATTRIBUTE_SHORTCUT
#define FILE_ATTRIBUTE_SHORTCUT		0x00020000
#endif
typedef struct STORE_INFORMATION {
	DWORD dwStoreSize;
	DWORD dwFreeSize;
} STORE_INFORMATION, *LPSTORE_INFORMATION;
typedef struct _RAPIINIT
{
    DWORD cbSize;
    HANDLE heRapiInit;
    HRESULT hrRapiInit;
} RAPIINIT;
/* End of rapi.h */

/* Start of NANDio.h */
//
// Disk info structure for DISK_IOCTL_GETINFO and DISK_IOCTL_SETINFO
//
typedef struct _DISK_INFO {
    DWORD di_total_sectors;
    DWORD di_bytes_per_sect;
    DWORD di_cylinders;
    DWORD di_heads;
    DWORD di_sectors;
    DWORD di_flags;       // see definitions below
} DISK_INFO, * PDISK_INFO;

//
// DISK_INFO.di_flags bit definitions
//
#define DISK_INFO_FLAG_MBR           0x00000001 // device type has/needs an MBR
#define DISK_INFO_FLAG_CHS_UNCERTAIN 0x00000002 // driver generated CHS values are suspect
#define DISK_INFO_FLAG_UNFORMATTED   0x00000004 // low level format (DISK_IOCTL_FORMAT_MEDIA) needed
#define DISK_INFO_FLAG_PAGEABLE      0x00000008 // device can be used for demand paging
/* End of NANDio.h */

/* Wil remote API functions */
IMPORT BOOL    WilRapiChkCeRapi(void);
IMPORT BOOL    WilRapiGetCeRapi(void);
IMPORT BOOL    WilRapiSetCeRapi(BOOL);
IMPORT DWORD   WilRapiVersion(void);
IMPORT DWORD   WilRapiWilVersion(void);
IMPORT HRESULT WilRapiInit(void);
IMPORT HRESULT WilRapiInitEx(RAPIINIT*);
IMPORT HRESULT WilRapiUninit(void);
IMPORT HRESULT WilRapiGetError(void);
IMPORT HRESULT WilRapiFreeBuffer(LPVOID);
IMPORT HRESULT WilRapiInvoke(LPCWSTR,LPCWSTR,DWORD,BYTE *,DWORD *,BYTE **,void *,DWORD);
IMPORT BOOL    WilCheckPassword(LPWSTR);
IMPORT BOOL    WilCloseHandle(HANDLE);
IMPORT BOOL    WilCopyFile(LPCWSTR,LPCWSTR,BOOL);
IMPORT BOOL    WilCreateDirectory(LPCWSTR,LPSECURITY_ATTRIBUTES);
IMPORT HANDLE  WilCreateFile(LPCWSTR,DWORD,DWORD,LPSECURITY_ATTRIBUTES,DWORD,DWORD,HANDLE);
IMPORT BOOL    WilCreateProcess(LPCWSTR,LPCWSTR,LPSECURITY_ATTRIBUTES,LPSECURITY_ATTRIBUTES,BOOL,DWORD,LPVOID,LPWSTR,LPSTARTUPINFO,LPPROCESS_INFORMATION);
IMPORT BOOL    WilDeleteFile(LPCWSTR);
IMPORT BOOL    WilFindAllFiles(LPCWSTR,DWORD,LPDWORD,LPLPCE_FIND_DATA);
IMPORT BOOL    WilFindClose(HANDLE);
IMPORT HANDLE  WilFindFirstFile(LPCWSTR,LPCE_FIND_DATA);
IMPORT BOOL    WilFindNextFile(HANDLE,LPCE_FIND_DATA);
IMPORT DWORD   WilGetFileAttributes(LPCWSTR);
IMPORT DWORD   WilGetFileSize(HANDLE,LPDWORD);
IMPORT BOOL    WilGetFileTime(HANDLE,FILETIME*,FILETIME*,FILETIME*);
IMPORT DWORD   WilGetLastError(void);
IMPORT BOOL    WilGetStoreInformation(LPSTORE_INFORMATION);
IMPORT BOOL    WilMoveFile(LPCWSTR,LPCWSTR);
IMPORT BOOL    WilReadFile(HANDLE,LPVOID,DWORD,LPDWORD,LPOVERLAPPED);
IMPORT BOOL    WilRemoveDirectory(LPCWSTR);
IMPORT BOOL    WilSetEndOfFile(HANDLE);
IMPORT BOOL    WilSetFileAttributes(LPCWSTR,DWORD);
IMPORT DWORD   WilSetFilePointer(HANDLE,LONG,PLONG,DWORD);
IMPORT BOOL    WilSetFileTime(HANDLE,FILETIME*,FILETIME*,FILETIME*);
IMPORT BOOL    WilWriteFile(HANDLE,LPCVOID,DWORD,LPDWORD,LPOVERLAPPED);
IMPORT LONG    WilRegCloseKey(HKEY);
IMPORT LONG    WilRegCreateKeyEx(HKEY,LPCWSTR,DWORD,LPWSTR,DWORD,REGSAM,LPSECURITY_ATTRIBUTES,PHKEY,LPDWORD);
IMPORT LONG    WilRegDeleteKey(HKEY,LPCWSTR);
IMPORT LONG    WilRegDeleteValue(HKEY,LPCWSTR);
IMPORT LONG    WilRegEnumKeyEx(HKEY,DWORD,LPWSTR,LPDWORD,LPDWORD,LPWSTR,LPDWORD,PFILETIME);
IMPORT LONG    WilRegEnumValue(HKEY,DWORD,LPWSTR,LPDWORD,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
IMPORT LONG    WilRegOpenKeyEx(HKEY,LPCWSTR,DWORD,REGSAM,PHKEY);
IMPORT LONG    WilRegQueryInfoKey(HKEY,LPWSTR,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,PFILETIME);
IMPORT LONG    WilRegQueryValueEx(HKEY,LPCWSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
IMPORT LONG    WilRegSetValueEx(HKEY,LPCWSTR,DWORD,DWORD,const BYTE *,DWORD);
IMPORT void    WilCLBSoftReset(void);
IMPORT void    WilCLBRebootInSafeMode(void);
IMPORT void    WilCLBRebootInSafeModeEx(DWORD);
IMPORT BOOL    WilCLBIsSafeMode(void);
IMPORT DWORD   WilCLBGetSafeModeParam(void);
IMPORT void    WilCLBPowerOff(void);
IMPORT BOOL    WilCLBAPODisable(void);
IMPORT BOOL    WilCLBAPOEnable(void);
IMPORT BOOL    WilCLBAPOGet(void);
IMPORT BOOL    WilCLBEnablePowerOff(void);
IMPORT BOOL    WilCLBDisablePowerOff(void);
IMPORT BOOL    WilCLBGetPowerOff(void);
IMPORT BOOL    WilCLBSetOffMaskTime(DWORD,DWORD);
IMPORT BOOL    WilCLBGetOffMaskTime(DWORD *,DWORD *);
IMPORT int     WilGetSystemDisk(LPWSTR,int);
IMPORT int     WilGetInnerUserDisk(LPWSTR,int);
IMPORT BOOL    WilCGDFlushRegistry(void);
IMPORT BOOL    WilCGDFlushAlarm(void);
IMPORT BOOL    WilCoshExecute(LPCWSTR,LPWSTR);
IMPORT UINT    WilCoshSetNopMode(ULONG);
IMPORT BOOL    WilCoshEndAppCategory(int);
IMPORT HANDLE  WilNANDOpen(void);
IMPORT BOOL    WilNANDClose(HANDLE);
IMPORT BOOL    WilNANDGetInfo(HANDLE,PDISK_INFO);
IMPORT BOOL    WilNANDReadSector(HANDLE,DWORD,DWORD,LPBYTE);
IMPORT BOOL    WilNANDWriteSector(HANDLE,DWORD,DWORD,LPBYTE);
IMPORT BOOL    WilNANDGetAccessFlag(HANDLE,BOOL*,BOOL*);
IMPORT BOOL    WilNANDClearAccessFlag(HANDLE);
IMPORT BOOL    WilGetUuid(LPBYTE);
IMPORT BOOL    WilGetSystemTime(LPSYSTEMTIME);
IMPORT BOOL    WilGetLocalTime(LPSYSTEMTIME);
IMPORT BOOL    WilSetSystemTime(LPSYSTEMTIME);
IMPORT BOOL    WilSetLocalTime(LPSYSTEMTIME);
IMPORT HWND    WilFindWindow(LPCWSTR,LPCWSTR);
IMPORT BOOL    WilPostMessage(HWND,UINT,WPARAM,LPARAM);
IMPORT BOOL    WilGetDiskFreeSpaceEx(LPCWSTR,PULARGE_INTEGER,PULARGE_INTEGER,PULARGE_INTEGER);
IMPORT void    WilDisplayDialog(int,BOOL);
IMPORT void    WilDebugLog(BOOL);
IMPORT DWORD   WilSetKicker(DWORD);
IMPORT DWORD   WilSetMonopoly(DWORD);
IMPORT BOOL    WilChkCnct(void);

#define	CeRapiInit				WilRapiInit
#define	CeRapiInitEx			WilRapiInitEx
#define	CeRapiUninit			WilRapiUninit
#define	CeRapiGetError			WilRapiGetError
#define	CeRapiFreeBuffer		WilRapiFreeBuffer
#define	CeRapiInvoke			WilRapiInvoke
#define	CeCheckPassword			WilCheckPassword
#define	CeCloseHandle			WilCloseHandle
#define	CeCopyFile				WilCopyFile
#define	CeCreateDirectory		WilCreateDirectory
#define	CeCreateFile			WilCreateFile
#define	CeCreateProcess			WilCreateProcess
#define	CeDeleteFile			WilDeleteFile
#define	CeFindAllFiles			WilFindAllFiles
#define	CeFindClose				WilFindClose
#define	CeFindFirstFile			WilFindFirstFile
#define	CeFindNextFile			WilFindNextFile
#define	CeGetFileAttributes		WilGetFileAttributes
#define	CeGetFileSize			WilGetFileSize
#define	CeGetFileTime			WilGetFileTime
#define	CeGetLastError			WilGetLastError
#define	CeGetStoreInformation	WilGetStoreInformation
#define	CeMoveFile				WilMoveFile
#define	CeReadFile				WilReadFile
#define	CeRemoveDirectory		WilRemoveDirectory
#define	CeSetEndOfFile			WilSetEndOfFile
#define	CeSetFileAttributes		WilSetFileAttributes
#define	CeSetFilePointer		WilSetFilePointer
#define	CeSetFileTime			WilSetFileTime
#define	CeWriteFile				WilWriteFile
#define	CeRegCloseKey			WilRegCloseKey
#define	CeRegCreateKeyEx		WilRegCreateKeyEx
#define	CeRegDeleteKey			WilRegDeleteKey
#define	CeRegDeleteValue		WilRegDeleteValue
#define	CeRegEnumKeyEx			WilRegEnumKeyEx
#define	CeRegEnumValue			WilRegEnumValue
#define	CeRegOpenKeyEx			WilRegOpenKeyEx
#define	CeRegQueryInfoKey		WilRegQueryInfoKey
#define	CeRegQueryValueEx		WilRegQueryValueEx
#define	CeRegSetValueEx			WilRegSetValueEx
#define	NANDOpen				WilNANDOpen
#define	NANDClose				WilNANDClose
#define	NANDGetInfo				WilNANDGetInfo
#define	NANDReadSector			WilNANDReadSector
#define	NANDWriteSector			WilNANDWriteSector
#define	NANDGetAccessFlag		WilNANDGetAccessFlag
#define	NANDClearAccessFlag		WilNANDClearAccessFlag

#define	RapiFreeBuffer			WilRapiFreeBuffer

#endif // #ifndef WILRAPI_H
