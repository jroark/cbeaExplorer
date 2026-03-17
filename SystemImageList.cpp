// SystemImageList.cpp: implementation of the CSystemImageList class.
//
//////////////////////////////////////////////////////////////////////

#include "SystemImageList.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//
// Implementation of the CSystemImageList class
//
CSystemImageList::CSystemImageList()
{
	SHFILEINFO	shInfo;

	m_hilSmall	= ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK, 1, 1024);
	m_hilLarge	= ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 1, 1024);

	ImageList_SetBkColor(m_hilSmall, ILD_TRANSPARENT);
	ImageList_SetBkColor(m_hilLarge, ILD_TRANSPARENT);

	// add unknown icon at index 0
	memset(&shInfo, 0, sizeof(shInfo));
	SHGetFileInfo(_T("TMP"), FILE_ATTRIBUTE_NORMAL, &shInfo, sizeof(shInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
		SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON);

	ImageList_AddIcon(m_hilSmall, shInfo.hIcon);

	memset(&shInfo, 0, sizeof(shInfo));
	SHGetFileInfo(_T("TMP"), FILE_ATTRIBUTE_NORMAL, &shInfo, sizeof(shInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
		SHGFI_TYPENAME | SHGFI_ICON | SHGFI_ICON);

	ImageList_AddIcon(m_hilLarge, shInfo.hIcon);

	// add closed directory icon at index 1
	memset(&shInfo, 0, sizeof(shInfo));
	SHGetFileInfo(_T("TMP"), FILE_ATTRIBUTE_DIRECTORY, &shInfo, sizeof(shInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
		SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON);

	ImageList_AddIcon(m_hilSmall, shInfo.hIcon);

	memset(&shInfo, 0, sizeof(shInfo));
	SHGetFileInfo(_T("TMP"), FILE_ATTRIBUTE_DIRECTORY, &shInfo, sizeof(shInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
		SHGFI_TYPENAME | SHGFI_ICON | SHGFI_ICON);

	ImageList_AddIcon(m_hilLarge, shInfo.hIcon);

	// add open directory icon at index 2
	memset(&shInfo, 0, sizeof(shInfo));
	SHGetFileInfo(_T("TMP"), FILE_ATTRIBUTE_DIRECTORY, &shInfo, sizeof(shInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
		SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON | SHGFI_OPENICON);

	ImageList_AddIcon(m_hilSmall, shInfo.hIcon);

	memset(&shInfo, 0, sizeof(shInfo));
	SHGetFileInfo(_T("TMP"), FILE_ATTRIBUTE_DIRECTORY, &shInfo, sizeof(shInfo), 
		SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
		SHGFI_TYPENAME | SHGFI_ICON | SHGFI_ICON | SHGFI_OPENICON);

	ImageList_AddIcon(m_hilLarge, shInfo.hIcon);
}

CSystemImageList::~CSystemImageList()
{
	std::map<std::string, pSystemFileInfo, ltstr>::iterator	itEnd;

	ImageList_Destroy(m_hilSmall);
	ImageList_Destroy(m_hilLarge);

	m_iconMapIterator	= m_iconMap.begin();
	itEnd				= m_iconMap.end();

	while(m_iconMapIterator != itEnd)
	{
		delete (pSystemFileInfo)(*m_iconMapIterator).second;
		m_iconMapIterator++;
	}
}

pSystemFileInfo	
CSystemImageList::GetSystemFileInfoFromExtension(std::string strExt)
{
	m_iconMapIterator = m_iconMap.find(strExt);

	if(m_iconMapIterator == m_iconMap.end())
	{
		// this ext has not been found yet so find it and add it
		SHFILEINFO	shInfo;
		int			indexS	= -1,
					indexL	= -1;
		memset(&shInfo, 0, sizeof(shInfo));
	
		SHGetFileInfo(strExt.c_str(), FILE_ATTRIBUTE_NORMAL, &shInfo, sizeof(shInfo), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
			SHGFI_TYPENAME | SHGFI_ICON | SHGFI_SMALLICON);

		indexS = ImageList_AddIcon(m_hilSmall, shInfo.hIcon);

		memset(&shInfo, 0, sizeof(shInfo));

		SHGetFileInfo(strExt.c_str(), FILE_ATTRIBUTE_NORMAL, &shInfo, sizeof(shInfo), 
			SHGFI_USEFILEATTRIBUTES | SHGFI_DISPLAYNAME | 
			SHGFI_TYPENAME | SHGFI_ICON);

		indexL = ImageList_AddIcon(m_hilLarge, shInfo.hIcon);

		if((indexL != -1) && (indexS != -1))
		{
			pSystemFileInfo	sysInfo	= new SystemFileInfo();

			ZeroMemory(sysInfo, sizeof(SystemFileInfo));
			_tcscpy(sysInfo->szTypeName, shInfo.szTypeName);

			sysInfo->iIndex		= indexS;
			m_iconMap[strExt]	= sysInfo;

			return sysInfo;
		}
	}
	else
	{
		// we have this ext cached so return the index
		return (pSystemFileInfo)(*m_iconMapIterator).second;
	}

	return NULL;
}

LPCTSTR	
CSystemImageList::GetTypeNameFromExtension(std::string strExt)
{
	pSystemFileInfo	sysInfo	= GetSystemFileInfoFromExtension(strExt);

	if(sysInfo)
		return sysInfo->szTypeName;
	else
		return NULL;
}

int 
CSystemImageList::GetIndexFromExtension(std::string strExt)
{
	pSystemFileInfo	sysInfo	= GetSystemFileInfoFromExtension(strExt);

	if(sysInfo)
		return sysInfo->iIndex;
	else
		return 0;
}
