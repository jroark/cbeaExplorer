// CbeaFile.cpp: implementation of the CCbeaFile class.
//
//////////////////////////////////////////////////////////////////////

#include "CbeaFile.h"

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCbeaFile::CCbeaFile()
{
	m_lpszFile		= NULL; 
	m_fpFile		= NULL;
	m_nFileCount	= 0;
	m_arrayOffsets	= NULL;
	m_nTotalSize	= 0;
}

CCbeaFile::CCbeaFile(LPTSTR lpszFile)
{
	m_lpszFile		= NULL; 
	m_fpFile		= NULL;
	m_nFileCount	= 0;
	m_arrayOffsets	= NULL;
	m_nTotalSize	= 0;

	LoadFile(lpszFile);
}

CCbeaFile::~CCbeaFile()
{
	if(m_lpszFile)
		delete (void *)m_lpszFile;

	if(m_fpFile)
		fclose(m_fpFile);

	if(m_arrayOffsets)
		delete m_arrayOffsets;
}

	BOOL 
CCbeaFile::GetFileInfoAt(
	long	lIndex, 
	LPTSTR	lpszFile, 
	int		iLen, 
	long	*lSize,
	long	*lOffset)
{
	unsigned char	n;

	(*lOffset) = m_arrayOffsets[lIndex];

	fseek(m_fpFile, m_arrayOffsets[lIndex], SEEK_SET);

	/* length of filename */
	fread((void *)&n, 1, 1, m_fpFile);

	if(iLen < n + 1)
		return FALSE;

	/* read the filename */
	fread((void *)lpszFile, 1, n + 1, m_fpFile);

	/* filesize */
	fread((void *)lSize, sizeof(long), 1, m_fpFile);

	return TRUE;
}

	BOOL 
CCbeaFile::Extract(
	LPTSTR				lpszPath,
	pExtractCallback	fpCallback,
	void				*pUser)
{
	if(lpszPath)
	{
		TCHAR	szWD[MAX_PATH];

		ZeroMemory(szWD, MAX_PATH * sizeof(TCHAR));

		::GetCurrentDirectory(MAX_PATH, szWD);

		if(::SetCurrentDirectory(lpszPath))
		{
			long			nTotalRead		= 0;
			unsigned int	i				= 0,
							size			= 0;
			unsigned char	n				= '\0',
							szFileName[256],
							*buffer			= NULL;
			FILE			*fp				= NULL;

			for(i = 0; i < m_nFileCount; i++)
			{
				fseek(m_fpFile, m_arrayOffsets[i], SEEK_SET);

				/* length of filename */
				fread((void *)&n, 1, 1, m_fpFile);

				memset(szFileName, 0, 256);
				/* read the filename */
				fread((void *)szFileName, 1, n + 1, m_fpFile);

				/* filesize */
				fread((void *)&size, sizeof(unsigned int), 1, m_fpFile);
				nTotalRead += size;

				if((fp = fopen((const char *)szFileName, "wb")) == NULL)
					return FALSE;

				buffer = (unsigned char *)malloc(size);

				if(buffer == NULL)
				{
					fclose(fp);
					return FALSE;
				}
				
				if((fread((void *)buffer, 1, size, m_fpFile) != size) ||
					(fwrite((void *)buffer, 1, size, fp) != size))
				{
					fclose(fp);
					free(buffer);
					return FALSE;
				}

				fclose(fp);
				free(buffer);

				fp = NULL;
				buffer = NULL;

				if(fpCallback != NULL)
					fpCallback(size, 0, m_nTotalSize, nTotalRead, pUser);
			}

			::SetCurrentDirectory(szWD);

			return TRUE;
		}
	}

	return FALSE;
}

	BOOL
CCbeaFile::LoadFile(LPTSTR lpszFile)
{
	unsigned int	i		= 0,
					offset	= 0,
					tmp		= 0,
					size	= 0;
	char			szFileName[256];

	if(lpszFile == NULL)
		return FALSE;
	
	if(m_lpszFile)
	{
		delete (void *)m_lpszFile;
		m_lpszFile = NULL;
	}

	m_lpszFile = new TCHAR[lstrlen(lpszFile) + 1];
	ZeroMemory((void *)m_lpszFile, lstrlen(lpszFile) + 1);
	_snprintf((char *)m_lpszFile, lstrlen(lpszFile), "%s", lpszFile);
	
	if((m_fpFile = fopen(m_lpszFile, "rb")) == NULL)
		return FALSE;

	m_nTotalSize	= 0;

	/* seek past first two bytes to the file count... */
	fseek(m_fpFile, 2, SEEK_SET);
	fread((void *)&m_nFileCount, sizeof(unsigned int), 1, m_fpFile);

	m_arrayOffsets = new long[m_nFileCount];

	/* seek past the header... who cares */
	fseek(m_fpFile, 256, SEEK_SET);
		
	for(i = 0; i < m_nFileCount; i++)
	{
		/* read offset of next file */
		fread((void *)&offset, sizeof(unsigned int), 1, m_fpFile);

		/* seek to length of filename */
		fseek(m_fpFile, 3, SEEK_CUR);

		m_arrayOffsets[i] = ftell(m_fpFile);

		/* length of filename */
		fread((void *)&tmp, 1, 1, m_fpFile);

		memset(szFileName, 0, 256);
		/* read the filename */
		fread((void *)szFileName, 1, tmp+1, m_fpFile);

		/* filesize */
		fread((void *)&size, sizeof(unsigned int), 1, m_fpFile);

		m_nTotalSize	+= size;

		fseek(m_fpFile, size, SEEK_CUR);
	}

	return TRUE;
}
