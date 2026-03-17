/* cbeaExplorer - a Casio cbea archive explorer editor...
 * author: John Roark <jroark@cs.usfca.edu>
 */
#include "CbeaExplorerWin.h"

#include <commctrl.h>
#include <ole2.h>

	INT WINAPI 
WinMain(
	HINSTANCE	hInst, 
	HINSTANCE	hPrevInst, 
	char		*cmdParam, 
	int			cmdShow)
{
	INITCOMMONCONTROLSEX	iccx;
	CCbeaExplorerWin		*mainWin	= NULL;

	OleInitialize(NULL);

	ZeroMemory(&iccx, sizeof(iccx));
	iccx.dwSize = sizeof(iccx);
	iccx.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES | ICC_COOL_CLASSES;
	if(!InitCommonControlsEx(&iccx))
		return -1;

	mainWin	= new CCbeaExplorerWin(hInst, cmdParam);

	mainWin->UpdateWindow();
	mainWin->ShowWindow(cmdShow);

	mainWin->MessageLoop();

	OleUninitialize();

	return 0;
}