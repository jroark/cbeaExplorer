// SystemImageList.h: interface for the CSystemImageList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SYSTEMIMAGELIST_H_
#define _SYSTEMIMAGELIST_H_

#pragma warning(disable:4786)

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>

#include <string>
#include <map>

typedef struct _SystemFileInfo
{
	int		iIndex;
	TCHAR	szTypeName[80];
} SystemFileInfo, *pSystemFileInfo;

class CSystemImageList
{
public:
	CSystemImageList();
	virtual ~CSystemImageList();

	int		GetIndexFromExtension(std::string strExt);
	LPCTSTR	GetTypeNameFromExtension(std::string strExt);

	operator HIMAGELIST() const { return m_hilSmall; }

	HIMAGELIST	GetHandle(BOOL bLarge) { return bLarge ? m_hilLarge : m_hilSmall; }

private:
	pSystemFileInfo	GetSystemFileInfoFromExtension(std::string strExt);

private:

	struct ltstr
	{
		bool operator()(std::string _Left, std::string _Right) const
		{
			// extensions are case insensitive
			return (_tcsicmp(_Left.c_str(), _Right.c_str()) < 0);
		}
	};

	std::map<std::string, pSystemFileInfo, ltstr>				m_iconMap;
	std::map<std::string, pSystemFileInfo, ltstr>::iterator		m_iconMapIterator;

	HIMAGELIST	m_hilSmall;
	HIMAGELIST	m_hilLarge;
};

#endif // _SYSTEMIMAGELIST_H_
