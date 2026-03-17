// CbeaFile.h: interface for the CCbeaFile class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CBEAFILE_H_
#define _CBEAFILE_H_

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

typedef	BOOL (*pExtractCallback)(long nFileLength, long nFileRead, long nTotalLength, long nTotalRead, void *pUser);

class CCbeaFile  
{
public:
	CCbeaFile();
	CCbeaFile(LPTSTR lpszFile);
	virtual ~CCbeaFile();

public:
	BOOL			Extract(LPTSTR lpszPath, pExtractCallback fpCallback, void *pUser);
	BOOL			GetFileInfoAt(long lIndex, LPTSTR lpszFile, int iLen, long *lSize, long *lOffset);
	unsigned int	GetFileCount() const { return m_nFileCount; }
	LPCTSTR			GetCBEAName() const { return m_lpszFile; }
	long			GetTotalSize() const { return m_nTotalSize; }
	BOOL			LoadFile(LPTSTR lpszFile);

private:
	LPCTSTR			m_lpszFile;
	FILE			*m_fpFile;
	unsigned int	m_nFileCount;
	long			m_nTotalSize;
	long			*m_arrayOffsets;

};

#endif // _CBEAFILE_H_
