// cbeaInstaller.cpp: implementation of the CcbeaInstaller class.
//
//////////////////////////////////////////////////////////////////////

#include "CbeaInstaller.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCbeaInstaller::CCbeaInstaller()
{
	m_cCbeaFile = NULL;
}

CCbeaInstaller::CCbeaInstaller(CCbeaFile *file)
{
	m_cCbeaFile = file;
}

CCbeaInstaller::~CCbeaInstaller()
{

}

	BOOL
CCbeaInstaller::Install(CCbeaFile *file)
{
	HRESULT	hr;

	if(file)
	{
		m_cCbeaFile	= file;
	}

	hr = CeRapiInit();

	if(hr >= 0)
	{
		CeRapiUninit();

		return TRUE;
	}

	return FALSE;
}

