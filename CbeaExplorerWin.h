// CbeaExplorerWin.h: interface for the CCbeaExplorerWin class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CBEAEXPLORERWIN_H_
#define _CBEAEXPLORERWIN_H_

#include "Window.h"
#include "CbeaFile.h"
#include "CbeaInstaller.h"
#include "resource.h"

#include <commctrl.h>
#include <shlwapi.h>

#include "SystemImageList.h"

#define NUM_TBBUTTONS		12

#define	IDC_STATUSBAR	0x3333
#define	IDC_REBAR		0x3334
#define	IDC_TOOLBAR		0x3335
#define	IDC_LISTVIEW	0x3336

class CCbeaExplorerWin : public CWindow  
{
public:
	CCbeaExplorerWin(HINSTANCE hInst, char *cmdParam);
	virtual ~CCbeaExplorerWin();

private:
	LRESULT OnClose(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnSize(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam);
	LRESULT	OnTimer(HWND hWnd, WPARAM wParam, LPARAM lParam);

	DECLARE_MSG_HANDLER(CCbeaExplorerWin);

private:
	void	CreateToolbar();
	void	CreateListview();
	BOOL	RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst);
	void	LoadRegistryValues();
	void	SaveRegistryValues();
	LPTSTR	GetOpenFile();
	BOOL	OpenFile(LPTSTR lpszFile);
	void	ExtractFiles();
	BOOL	AddFileToList(char *pszFile, long size, long offset, unsigned int i);

	static BOOL	ExtractCallback(long fs, long fr, long ts, long tr, void *param);

private:
	HWND				m_hRebar;
	HWND				m_hToolbar;
	HWND				m_hStatusbar;
	HWND				m_hListview;

	UINT				m_uStatusHeight;
	UINT				m_uRebarHeight;

	HIMAGELIST			m_hImageListToolbar;

	HICON				m_hIcon;

	CCbeaFile			*m_cCurrentFile;
	CSystemImageList	*m_cImgList;

};

#endif // _CBEAEXPLORERWIN_H_
