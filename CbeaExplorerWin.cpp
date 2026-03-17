// CbeaExplorerWin.cpp: implementation of the CCbeaExplorerWin class.
//
//////////////////////////////////////////////////////////////////////

#include "CbeaExplorerWin.h"

#pragma comment(lib, "shlwapi.lib")

#define	IDT_SELECTIONTIMER	0x000000012

	static BOOL WINAPI 
aboutDlgProc(
	HWND	hWnd,
	UINT	uMsg,
	WPARAM	wParam,
	LPARAM	lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
			return TRUE;

		case WM_CLOSE:
		case WM_COMMAND:
			::EndDialog(hWnd, 0);
			return TRUE;
	}

	return FALSE;
}

IMPLEMENT_MSG_HANDLER(CWindow, CCbeaExplorerWin)

BEGIN_MSG_MAP(CCbeaExplorerWin)
	ADD_MSG_HANDLER(WM_CLOSE, OnClose)
	ADD_MSG_HANDLER(WM_CREATE, OnCreate)
	ADD_MSG_HANDLER(WM_COMMAND, OnCommand)
	ADD_MSG_HANDLER(WM_DESTROY, OnDestroy)
	ADD_MSG_HANDLER(WM_SIZE, OnSize)
	ADD_MSG_HANDLER(WM_NOTIFY, OnNotify)
	ADD_MSG_HANDLER(WM_TIMER, OnTimer)
END_MSG_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCbeaExplorerWin::CCbeaExplorerWin(
	HINSTANCE	hInst,
	char		*cmdParam)
{
	ENABLE_MSG_MAP();

	m_hRebar		= NULL;
	m_hToolbar		= NULL;
	m_hStatusbar	= NULL;
	m_hListview		= NULL;

	m_uStatusHeight	= 0;
	m_uRebarHeight	= 0;
	m_hInst			= hInst;
	m_cCurrentFile	= NULL;

	m_cImgList = new CSystemImageList();

	m_hWnd = CWindow::CreateEx(0, _T("cbeaExplorerWinClass"), _T("CBEA Explorer"), 
				WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
				CW_USEDEFAULT, NULL, ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU_MAIN)), hInst);
}

CCbeaExplorerWin::~CCbeaExplorerWin()
{
	if(m_cCurrentFile)
		delete m_cCurrentFile;

	if(m_cImgList)
		delete m_cImgList;
}

	BOOL 
CCbeaExplorerWin::RegisterClass(
	LPCTSTR		lpszClass, 
	HINSTANCE	hInst)
{
	WNDCLASSEX	wc;

	if(!::GetClassInfoEx(hInst, lpszClass, &wc))
	{
		GetWndClassEx(wc);

		wc.hIcon	= (HICON)::LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_CASIO), 
									IMAGE_ICON, 0, 0, 
									LR_DEFAULTSIZE | LR_LOADTRANSPARENT);
		wc.hInstance		= hInst;
		wc.lpszClassName	= lpszClass;
		wc.style			= 0;

		if(!::RegisterClassEx(&wc))
			return FALSE;
	}

	return TRUE;
}

	LRESULT
CCbeaExplorerWin::OnClose(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{
	SaveRegistryValues();
	::DestroyWindow(m_hRebar);
	::DestroyWindow(m_hStatusbar);
	::DestroyWindow(hWnd);

	return 0;
}

		LRESULT
CCbeaExplorerWin::OnDestroy(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{
	::PostQuitMessage(0);

	return 0;
}

	LRESULT
CCbeaExplorerWin::OnSize(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{
	RECT	rc;
	INT		iWidths[3],
			iWidth		= 0,
			iHeight		= 0;

	::GetClientRect(m_hWnd, &rc);
	iWidth	= rc.right - rc.left;
	iHeight	= rc.bottom - rc.top;

	::SendMessage(m_hStatusbar, WM_SIZE, wParam, lParam);
	::SendMessage(m_hRebar, WM_SIZE, wParam, lParam);

	::MoveWindow(m_hListview, 0, m_uRebarHeight,
			iWidth, iHeight - m_uRebarHeight - m_uStatusHeight, TRUE);

	::GetClientRect(m_hStatusbar, &rc);
	m_uStatusHeight = rc.bottom - rc.top;

	iWidths[0] = 150;
	iWidths[1] = rc.right - 94;
	iWidths[2] = rc.right - 32;
	
	::SendMessage(m_hStatusbar, SB_SETPARTS, 3, (LPARAM)iWidths);

	return 0;
}

	LRESULT
CCbeaExplorerWin::OnCreate(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{
	RECT			rc;
	INT				iWidths[3];

	m_hWnd = hWnd;

	::SendMessage(m_hWnd, WM_SETICON, ICON_SMALL, 
		(LPARAM)(HICON)::LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON_CASIO))); 

	::SendMessage(m_hWnd, WM_SETICON, ICON_BIG, 
		(LPARAM)(HICON)::LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_ICON_CASIO)));

	LoadRegistryValues();

	// create the status bar
	m_hStatusbar = ::CreateWindow(
						STATUSCLASSNAME, NULL,
						WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP | SBT_TOOLTIPS,
						0, 0, 0, 0,
						hWnd, (HMENU)IDC_STATUSBAR, m_hInst, NULL);

	::GetClientRect(m_hStatusbar, &rc);
	m_uStatusHeight = rc.bottom - rc.top;

	// setup status bar
	iWidths[0] = 150;
	iWidths[1] = rc.right - 94;
	iWidths[2] = rc.right - 32;
	
	::SendMessage(m_hStatusbar, SB_SETPARTS, 3, (LPARAM)iWidths);

	CreateToolbar();

	CreateListview();

	return 0;
}

	BOOL 
CCbeaExplorerWin::ExtractCallback(long fs, long fr, long ts, long tr, void *param)
{
	HWND				hwndProgress	= (HWND)param;

	assert(hwndProgress != NULL);

	::SendMessage(hwndProgress, PBM_SETPOS, 
			(WPARAM)((tr*100)/ts), 0);

	return TRUE;
}

	void	
CCbeaExplorerWin::ExtractFiles()
{
	BROWSEINFO		info;
	LPITEMIDLIST	lpitem; 
    LPMALLOC		pMalloc		= NULL;
	TCHAR			szFolder[MAX_PATH];

	if(m_cCurrentFile == NULL)
		return;
	
	::SHGetMalloc(&pMalloc);

	ZeroMemory(szFolder, sizeof(TCHAR) * MAX_PATH);

	info.hwndOwner		= m_hWnd;
	info.pidlRoot		= NULL;
	info.pszDisplayName	= szFolder; 
	info.lpszTitle		= _T("Select a Folder"); 
	info.ulFlags		= BIF_RETURNONLYFSDIRS; 
	info.lpfn			= NULL;
	info.iImage			= 0;

	if((lpitem = ::SHBrowseForFolder(&info)) == NULL)
	{
		pMalloc->Release();
		return; 
	}

	if(!::SHGetPathFromIDList(lpitem, szFolder)) {
		pMalloc->Free(lpitem);
		pMalloc->Release();
		return; 
	}

	pMalloc->Free(lpitem);
	pMalloc->Release();

	if(m_cCurrentFile != NULL)
	{
		RECT	rc;
		HWND	hwndProgress;
		int		aRightCoords[3];
		BOOL	ret					= FALSE;

		ret = ::SendMessage(m_hStatusbar, SB_GETPARTS, 3, (LPARAM)&aRightCoords);
		aRightCoords[1] -= 128;
		ret = ::SendMessage(m_hStatusbar, SB_SETPARTS, 3, (LPARAM)&aRightCoords);

		ret = ::SendMessage(m_hStatusbar, SB_GETRECT, 2, (LPARAM)&rc);

		hwndProgress = ::CreateWindowEx(0, PROGRESS_CLASS,
							(LPCTSTR) NULL, 
							WS_CHILD | WS_VISIBLE | PBS_SMOOTH & ~WS_BORDER,
							rc.left + 5, rc.top + 2,
							(rc.right - rc.left), 
							(rc.bottom - rc.top) - 4, 
							m_hStatusbar, (HMENU)0, m_hInst, NULL);

		ret = ::SendMessage(hwndProgress, PBM_SETRANGE, 
			0, MAKELPARAM(0, 100));

		ret = ::SendMessage(hwndProgress, PBM_SETSTEP, 
				(WPARAM)1, 0);

		if(!m_cCurrentFile->Extract(szFolder, ExtractCallback, (void *)hwndProgress))
		{
			TCHAR	szMsg[256];

			ZeroMemory(szMsg, 256 * sizeof(TCHAR));
			_sntprintf(szMsg, 255, _T("Failed to extract files from %s."), 
				m_cCurrentFile->GetCBEAName());

			::MessageBox(m_hWnd, szMsg, _T("Error"), MB_OK | MB_ICONERROR);
		}

		ret = ::SendMessage(hwndProgress, WM_CLOSE, 0, 0);
		aRightCoords[1] += 128;
		ret = ::SendMessage(m_hStatusbar, SB_SETPARTS, 3, (LPARAM)&aRightCoords);
	}
}

	BOOL 
CCbeaExplorerWin::AddFileToList(char *pszFile, long size, long offset, unsigned int i)
{
	LVITEM	lvI;
	TCHAR	szTmp[256];

	ZeroMemory(&lvI, sizeof(lvI));
	lvI.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvI.lParam		= size;
	lvI.pszText		= pszFile;
	lvI.iItem		= i;
	lvI.iImage		= m_cImgList->GetIndexFromExtension(strrchr(pszFile, '.')); 

	ListView_InsertItem(m_hListview, &lvI);

	ZeroMemory(szTmp, 256 * sizeof(TCHAR));

	lvI.mask		= LVIF_TEXT;
	lvI.iSubItem	= 1;
	lvI.pszText		= ::StrFormatByteSize(size, szTmp, 255);

	ListView_SetItem(m_hListview, &lvI);

	lvI.mask		= LVIF_TEXT;
	lvI.iSubItem	= 2;
	lvI.pszText		= (TCHAR *)m_cImgList->GetTypeNameFromExtension(strrchr(pszFile, '.'));

	ListView_SetItem(m_hListview, &lvI);

	return TRUE;
}

	LPTSTR	
CCbeaExplorerWin::GetOpenFile()
{
	OPENFILENAME	ofn;
	LPTSTR			pszFile	= NULL;
		
	pszFile = (LPTSTR)::GlobalAlloc(0, MAX_PATH * sizeof(TCHAR)); 
	ZeroMemory((void *)pszFile, MAX_PATH * sizeof(TCHAR));
	
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize		= sizeof(ofn);
	ofn.lpstrInitialDir	= NULL;
	ofn.hwndOwner		= m_hWnd;
	ofn.lpstrFilter		= _T("CBEA Archives (*.cbea)\0*.cbea\0All Files (*.*)\0*.*\0\0");
	ofn.lpstrFile		= (LPTSTR)pszFile;
	ofn.nMaxFile		= MAX_PATH;
	ofn.Flags			= OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt		= _T("cbea");

	if(!::GetOpenFileName(&ofn))
	{
		::GlobalFree((void *)pszFile);
		return NULL;
	}

	return pszFile;
}

	__inline int 
ListView_SetView(
	HWND	hwnd,
	DWORD	iView)
{
	DWORD	dwStyle	= 0;

	dwStyle	=	::GetWindowLong(hwnd, GWL_STYLE);
	dwStyle	=	dwStyle & ~(dwStyle & LVS_TYPEMASK);
	dwStyle	|=	iView;

	return ::SetWindowLong(hwnd, GWL_STYLE, dwStyle);
}

	BOOL
CCbeaExplorerWin::OpenFile(LPTSTR lpszFile)
{
	TCHAR	sz[256],
			szTmp[128];

	ListView_DeleteAllItems(m_hListview);
	if(m_cCurrentFile)
	{
		delete m_cCurrentFile;
		m_cCurrentFile = NULL;
	}

	m_cCurrentFile = new CCbeaFile(lpszFile);

	if(m_cCurrentFile)
	{
		long			size		= 0,
						offset		= 0;
		char			szFile[256];
		unsigned int	i			= 0,
						count		= m_cCurrentFile->GetFileCount();

		for(i = 0; i < count; i++)
		{
			ZeroMemory(szFile, 256);

			if(m_cCurrentFile->GetFileInfoAt(i, szFile, 255, &size, &offset))
				AddFileToList(szFile, size, offset, i);
		}
	}

	::EnableMenuItem(::GetSubMenu(::GetMenu(m_hWnd), 0), 
		IDM_FILE_EXTRACT, MF_BYCOMMAND | MF_ENABLED);

	::EnableMenuItem(::GetSubMenu(::GetMenu(m_hWnd), 0), 
		IDM_FILE_CLOSE, MF_BYCOMMAND | MF_ENABLED);

	::EnableMenuItem(::GetSubMenu(::GetMenu(m_hWnd), 0), 
		IDM_FILE_INSTALL, MF_BYCOMMAND | MF_ENABLED);

	_sntprintf(sz, 255, _T("%d files\0"), 
		m_cCurrentFile->GetFileCount());
	::SendMessage(m_hStatusbar, SB_SETTEXT, 0, (LPARAM)sz);

	_sntprintf(sz, 255, _T("%s\0"), 
		::StrFormatByteSize(m_cCurrentFile->GetTotalSize(), szTmp, 127));
	::SendMessage(m_hStatusbar, SB_SETTEXT, 1, (LPARAM)sz);

	return TRUE;
}

	LRESULT
CCbeaExplorerWin::OnCommand(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{

	if((LOWORD(wParam) == IDM_VIEW_ICONS) ||
		(LOWORD(wParam) == IDM_VIEW_LIST) ||
		(LOWORD(wParam) == IDM_VIEW_DETAILS))
	{
		HMENU	hMenu	= ::GetSubMenu(::GetMenu(hWnd), 2);

		::CheckMenuItem(hMenu, IDM_VIEW_ICONS, MF_BYCOMMAND | MF_UNCHECKED);
		::CheckMenuItem(hMenu, IDM_VIEW_LIST, MF_BYCOMMAND | MF_UNCHECKED);
		::CheckMenuItem(hMenu, IDM_VIEW_DETAILS, MF_BYCOMMAND | MF_UNCHECKED);

		switch(LOWORD(wParam))
		{
			case IDM_VIEW_ICONS:
				ListView_SetView(m_hListview, LVS_ICON);
				::CheckMenuItem(hMenu, IDM_VIEW_ICONS, MF_BYCOMMAND | MF_CHECKED);
				break;

			case IDM_VIEW_LIST:
				ListView_SetView(m_hListview, LVS_LIST);
				::CheckMenuItem(hMenu, IDM_VIEW_LIST, MF_BYCOMMAND | MF_CHECKED);
				break;

			case IDM_VIEW_DETAILS:
				ListView_SetView(m_hListview, LVS_REPORT);
				::CheckMenuItem(hMenu, IDM_VIEW_DETAILS, MF_BYCOMMAND | MF_CHECKED);
				break;
		}
	}
	else
	{
		switch(LOWORD(wParam))
		{
			case IDM_FILE_EXIT:
				::PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;

			case IDM_FILE_OPEN:
			{
				LPSTR	lpszFile	= NULL;
				if((lpszFile = GetOpenFile()) != NULL)
				{
					TCHAR	sz[256];

					_sntprintf(sz, 255, _T("CBEA Explorer - [%s]\0"), 
						_tcsrchr(lpszFile, '\\') + 1);
					::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)sz);

					OpenFile(lpszFile);
					::GlobalFree((void *)lpszFile);
				}
				break;
			}

			case IDM_FILE_EXTRACT:
				ExtractFiles();
				break;

			case IDM_FILE_INSTALL:
				if(m_cCurrentFile != NULL)
				{
					CCbeaInstaller	*installer	= new CCbeaInstaller();

					if(!installer->Install(m_cCurrentFile))
					{
						TCHAR	szMsg[256];

						ZeroMemory(szMsg, 256 * sizeof(TCHAR));
						_sntprintf(szMsg, 255, _T("Installation of %s has failed."), 
							m_cCurrentFile->GetCBEAName());

						::MessageBox(hWnd, szMsg, _T("Error"), MB_OK | MB_ICONERROR);
					}
				}
				break;

			case IDM_FILE_SAVE:
				break;

			case IDM_FILE_SAVEAS:
				break;

			case IDM_FILE_NEW:
			case IDM_FILE_CLOSE:
			{
				ListView_DeleteAllItems(m_hListview);
				if(m_cCurrentFile)
				{
					delete m_cCurrentFile;
					m_cCurrentFile = NULL;
				}

				::EnableMenuItem(::GetSubMenu(::GetMenu(m_hWnd), 0), 
					IDM_FILE_EXTRACT, MF_BYCOMMAND | MF_GRAYED);

				::EnableMenuItem(::GetSubMenu(::GetMenu(m_hWnd), 0), 
					IDM_FILE_CLOSE, MF_BYCOMMAND | MF_GRAYED);

				::EnableMenuItem(::GetSubMenu(::GetMenu(m_hWnd), 0), 
					IDM_FILE_INSTALL, MF_BYCOMMAND | MF_GRAYED);

				::SendMessage(m_hStatusbar, SB_SETTEXT, 0, (LPARAM)NULL);
				::SendMessage(m_hStatusbar, SB_SETTEXT, 1, (LPARAM)NULL);
				::SendMessage(hWnd, WM_SETTEXT, 0, (LPARAM)_T("CBEA Explorer"));
				break;
			}

			case IDM_EDIT_COPY:
				break;

			case IDM_EDIT_CUT:
				break;

			case IDM_EDIT_PASTE:
				break;

			case IDM_EDIT_DELETE:
				break;

			case IDM_ABOUT:
				::DialogBoxParam(m_hInst, MAKEINTRESOURCE(IDD_DLG_ABOUT), 
					hWnd, (DLGPROC)aboutDlgProc, 0);
				break;
		}
	}

	return 0;
}

	LRESULT
CCbeaExplorerWin::OnTimer(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{
	switch(wParam)
	{
		case IDT_SELECTIONTIMER:
		{
			INT		count	= ListView_GetSelectedCount(m_hListview);

			if(count)
			{
				TCHAR	sz[256],
						szTmp[128];
				LVITEM	lvi;
				long	lSize	= 0;

				_sntprintf(sz, 255, _T("%d files selected\0"), count);
				
				::SendMessage(m_hStatusbar, SB_SETTEXT, 0, (LPARAM)sz);

				ZeroMemory(&lvi, sizeof(lvi));

				lvi.iItem		= 0;
				lvi.mask		= LVIF_PARAM | LVIF_STATE;
				lvi.stateMask	= LVIS_SELECTED;

				while(ListView_GetItem(m_hListview, &lvi))
				{
					lvi.iItem++;

					if(lvi.state & LVIS_SELECTED)
						lSize	+= lvi.lParam;
				}

				_sntprintf(sz, 255, _T("%s\0"), 
					::StrFormatByteSize(lSize, szTmp, 127));
				::SendMessage(m_hStatusbar, SB_SETTEXT, 1, (LPARAM)sz);
			}

			break;
		}
	}

	return 0L;
}


	LRESULT
CCbeaExplorerWin::OnNotify(
	HWND	hWnd,
	WPARAM	wParam,
	LPARAM	lParam)
{
	static	BOOL	bSel	= FALSE;
	LPNMHDR			pnmh	= (LPNMHDR)lParam;

	switch(pnmh->code)
	{
		case NM_CLICK:
		{
			LPNMITEMACTIVATE	lpnmitem	= (LPNMITEMACTIVATE)lParam;

			// if selected
			if(ListView_GetSelectedCount(m_hListview) && !bSel)
			{
				::SetTimer(hWnd, IDT_SELECTIONTIMER, 100, (TIMERPROC) NULL);
				bSel	= TRUE;
			}
			else if((ListView_GetSelectedCount(m_hListview) == 0) && bSel)
			{
				TCHAR	sz[256],
						szTmp[128];

				::KillTimer(hWnd, IDT_SELECTIONTIMER);
				bSel	= FALSE;

				_sntprintf(sz, 255, _T("%d files\0"), 
					m_cCurrentFile->GetFileCount());
				::SendMessage(m_hStatusbar, SB_SETTEXT, 0, (LPARAM)sz);

				_sntprintf(sz, 255, _T("%s\0"), 
					::StrFormatByteSize(m_cCurrentFile->GetTotalSize(), szTmp, 127));
				::SendMessage(m_hStatusbar, SB_SETTEXT, 1, (LPARAM)sz);
			}

			break;
		}

		case NM_RCLICK:
		{
			LPNMITEMACTIVATE	lpnmitem	= (LPNMITEMACTIVATE)lParam;
			POINT				pt;
			HMENU				hMenu		= ::LoadMenu(m_hInst, MAKEINTRESOURCE(IDR_MENU_POPUP));
			HMENU				hPopup		= ::GetSubMenu(hMenu, 0);

			::GetCursorPos(&pt);

			switch(::TrackPopupMenu(hPopup, 
						TPM_RETURNCMD | TPM_RIGHTBUTTON, 
						pt.x, pt.y, 
						0, hWnd, NULL))
			{
				case IDM_EDIT_CUT:
					break;

				default:
					break;
			}

			::PostMessage(hWnd, 0, 0, 0);
			::DestroyMenu(hMenu);

			break;
		}
	}

	return 0;
}

	void
CCbeaExplorerWin::CreateListview()
{
	RECT		rc;
	LVCOLUMN	lvc;
	int			i	= 0;

	::GetClientRect(m_hWnd, &rc);

	m_hListview = ::CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, _T(""), 
						WS_CHILD | WS_VISIBLE | WS_TABSTOP | LVS_REPORT | LVS_EDITLABELS,
						0, m_uRebarHeight, rc.right, 
						rc.bottom - (m_uRebarHeight + m_uStatusHeight), 
						m_hWnd, (HMENU)IDC_LISTVIEW, 
						m_hInst, (void *)this);

	ZeroMemory(&lvc, sizeof(lvc));
	lvc.mask		= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem	= 0;
	lvc.pszText		= _T("Name");
	lvc.cx			= 250;
	lvc.fmt			= LVCFMT_LEFT;

	i = ListView_InsertColumn(m_hListview, 0, &lvc);

	lvc.iSubItem	= 1;
	lvc.pszText		= _T("Size");
	lvc.cx			= 100;
	lvc.fmt			= LVCFMT_RIGHT;

	i = ListView_InsertColumn(m_hListview, 1, &lvc);

	lvc.iSubItem	= 2;
	lvc.pszText		= _T("Type");
	lvc.cx			= 250;
	lvc.fmt			= LVCFMT_LEFT;

	i = ListView_InsertColumn(m_hListview, 2, &lvc);

	ListView_SetImageList(m_hListview, m_cImgList->GetHandle(FALSE), LVSIL_SMALL);
	ListView_SetImageList(m_hListview, m_cImgList->GetHandle(TRUE), LVSIL_NORMAL);
}

	void 
CCbeaExplorerWin::CreateToolbar()
{
	REBARINFO		rbi;
	REBARBANDINFO	rbbi;
	TBBUTTON		tbArray[NUM_TBBUTTONS];
	BOOL			bIsSeparator[NUM_TBBUTTONS] = {0,1,0,2,1,2,2,2,1,0,0,0};
	INT				i, iBitmap, u;
	HBITMAP			hBmp;

	m_hRebar = ::CreateWindowEx(WS_EX_TOOLWINDOW, REBARCLASSNAME, NULL, 
					WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS |
					WS_CLIPCHILDREN | RBS_VARHEIGHT | CCS_NODIVIDER | WS_BORDER, 
					0, 0, 0, 0, m_hWnd, (HMENU)IDC_REBAR, m_hInst, NULL);

	// init the REBARINFO structure
	ZeroMemory(&rbi, sizeof(REBARINFO));
	rbi.cbSize	= sizeof(REBARINFO);
	rbi.fMask	= 0;
	rbi.himl	= (HIMAGELIST)NULL;
	::SendMessage(m_hRebar, RB_SETBARINFO, 0, (LPARAM)&rbi);

	// create the toolbar control.
	m_hToolbar = ::CreateWindowEx(
					WS_EX_TOOLWINDOW,
					TOOLBARCLASSNAME,
					NULL,
					WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | 
					WS_CLIPSIBLINGS | CCS_NODIVIDER |
					CCS_NOPARENTALIGN | CCS_NORESIZE | 
					TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
					0,0,0,0, 
					m_hRebar, (HMENU)IDC_TOOLBAR,
					m_hInst, NULL);

	// sets the size of the TBBUTTON structure.
	::SendMessage(m_hToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);

	// set the bitmap size.
	::SendMessage(m_hToolbar, TB_SETBITMAPSIZE,
			   0, (LPARAM)MAKELONG(16, 16));

	// set the button size.
	::SendMessage(m_hToolbar, TB_SETBUTTONSIZE,
			   0, (LPARAM)MAKELONG(16+4, 16+4));

	m_hImageListToolbar = ::ImageList_Create(16, 16, ILC_COLOR24|ILC_MASK, 
										9, 0); 
	hBmp = ::LoadBitmap(m_hInst, MAKEINTRESOURCE(IDB_BMP_TOOLBAR));
	::ImageList_AddMasked(m_hImageListToolbar, hBmp, RGB(255, 0, 255));
	::DeleteObject(hBmp);

	::SendMessage(m_hToolbar, TB_SETIMAGELIST, 0, (LPARAM)m_hImageListToolbar);
   
	// Loop to fill the array of TBBUTTON structures.
	iBitmap = 0;
	for(i = 0; i < NUM_TBBUTTONS; i++) 
	{
		tbArray[i].iBitmap   = iBitmap;
		tbArray[i].idCommand = 0;
		tbArray[i].fsStyle   = TBSTYLE_BUTTON;
		tbArray[i].dwData    = 0;
		tbArray[i].iString   = iBitmap;
		if(bIsSeparator[i] == 1) 
		{
			tbArray[i].fsState = 0;
			tbArray[i].fsStyle = TBSTYLE_SEP;
		}
		else if(bIsSeparator[i] == 2)
		{
			tbArray[i].fsState = 0;
			tbArray[i].fsStyle = TBSTYLE_BUTTON;
			iBitmap++;
		}
		else 
		{
			tbArray[i].fsState = TBSTATE_ENABLED;
			tbArray[i].fsStyle = TBSTYLE_BUTTON;
			iBitmap++;
		}
	}

	tbArray[0].idCommand	= IDM_FILE_NEW;
	tbArray[2].idCommand	= IDM_FILE_OPEN;
	tbArray[3].idCommand	= IDM_FILE_SAVE;
	tbArray[5].idCommand	= IDM_EDIT_CUT;
	tbArray[6].idCommand	= IDM_EDIT_COPY;
	tbArray[7].idCommand	= IDM_EDIT_PASTE;
	tbArray[9].idCommand	= IDM_VIEW_ICONS;
	tbArray[10].idCommand	= IDM_VIEW_LIST;
	tbArray[11].idCommand	= IDM_VIEW_DETAILS;

	// add the buttons
	::SendMessage(m_hToolbar,
			TB_ADDBUTTONS, (UINT)NUM_TBBUTTONS, (LPARAM)tbArray);

	// init structure members
	ZeroMemory(&rbbi, sizeof(REBARBANDINFO));
	rbbi.cbSize = sizeof(REBARBANDINFO);

	// get the height of the toolbar.
	u = ::SendMessage(m_hToolbar, TB_GETBUTTONSIZE, 0, 0);

	// set values unique to the band with the toolbar.
	rbbi.fMask		=  RBBIM_STYLE | RBBIM_CHILD  | RBBIM_CHILDSIZE | 
						RBBIM_SIZE;
	rbbi.fStyle		=  RBBS_NOGRIPPER;
	rbbi.hwndChild	= m_hToolbar;
	rbbi.cxMinChild	= 0;
	rbbi.cyMinChild	= HIWORD(u) + 2;
	rbbi.cx			= 250;

	// add the band that has the toolbar.
	::SendMessage(m_hRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbbi);

	m_uRebarHeight = ::SendMessage(m_hRebar, RB_GETROWHEIGHT, 0, 0);
}

	void 
CCbeaExplorerWin::LoadRegistryValues()
{
	unsigned long	reg		= REG_BINARY, 
					size	= sizeof(WINDOWPLACEMENT);
	WINDOWPLACEMENT	wp;

	ZeroMemory(&wp, sizeof(wp));

	if(::SHGetValue(HKEY_CURRENT_USER, 
		_T("SOFTWARE\\Stuff On Fire\\cbeaExplorer"), _T("position"), 
		&reg, (LPVOID)&wp, &size) == ERROR_SUCCESS)
	{
		if(wp.showCmd == SW_SHOWMINIMIZED)
			wp.showCmd = SW_SHOWNORMAL;
		::SetWindowPlacement(m_hWnd, &wp);
	}
}

	void 
CCbeaExplorerWin::SaveRegistryValues()
{
	unsigned long	reg		= REG_BINARY, 
					size	= sizeof(WINDOWPLACEMENT);
	WINDOWPLACEMENT	wp;
	TCHAR			sz[MAX_PATH],
					szName[MAX_PATH];

	ZeroMemory(&wp, sizeof(wp));
	wp.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(m_hWnd, &wp);

	::SHSetValue(HKEY_CURRENT_USER, 
		_T("SOFTWARE\\Stuff On Fire\\cbeaExplorer"), _T("position"), 
		REG_BINARY, (LPVOID)&wp, size);

	::SHSetValue(HKEY_CLASSES_ROOT, _T(".cbea\0"), _T("\0"), REG_SZ, _T("CBEA.1\0"), 13);

	::GetModuleFileName(::GetModuleHandle(NULL), szName, MAX_PATH);

	_sntprintf(sz, MAX_PATH, _T("%s,0"), szName);

	::SHSetValue(HKEY_CLASSES_ROOT, _T("CBEA.1\\DefaultIcon\0"), _T("\0"), REG_SZ, sz, lstrlen(sz));
	_sntprintf(sz, MAX_PATH, _T("%s %%1\0"), szName);
	::SHSetValue(HKEY_CLASSES_ROOT, _T("CBEA.1\\shell\\open\\command\0"), _T("\0"), REG_SZ, sz, lstrlen(sz));
}
