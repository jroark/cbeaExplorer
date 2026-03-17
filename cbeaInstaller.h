// cbeaInstaller.h: interface for the CcbeaInstaller class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CBEAINSTALLER_H_
#define _CBEAINSTALLER_H_

#include "CbeaFile.h"

#include <wilrapi.h>

class CCbeaInstaller  
{
public:
	CCbeaInstaller();
	CCbeaInstaller(CCbeaFile *file);
	virtual ~CCbeaInstaller();

public:
	BOOL	Install(CCbeaFile *file);

private:
	CCbeaFile	*m_cCbeaFile;

};

#endif // _CBEAINSTALLER_H_
