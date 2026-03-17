/*
 *
 * Copyright (C) 2004, John Roark <jroark@cs.usfca.edu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: Window.cpp,v 1.0 2004/01/15 01:45:41 antagonizt Exp $
 */

// Window.cpp: implementation of the CWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "Window.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CWindow::CWindow()
{
	m_pszClassName	= NULL;
	m_hWnd			= NULL;
	m_hInst			= NULL;
}

CWindow::~CWindow()
{
	::DestroyWindow(m_hWnd);
	::UnregisterClass(m_pszClassName, m_hInst);

	if(m_pszClassName) 
		delete m_pszClassName;
}

	void 
CWindow::AddMessageHandler(
	UINT				msgId, 
	fpMessageHandler	fpMsgHandler)
{
	msgEntry	entry;

	entry.fpMsgHandler	= (fpMessageHandler)fpMsgHandler;

	 // insert key & data to map
	m_msgMap.insert(std::map<UINT,msgEntry>::value_type(msgId, entry));
}

	LRESULT 
CWindow::WndProc(
	HWND	hWnd, 
	UINT	uMsg, 
	WPARAM	wParam, 
	LPARAM	lParam)
{
	m_msgMapIterator = m_msgMap.find(uMsg);

	// check if message entry available
	if(m_msgMapIterator == m_msgMap.end()) 
	{
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);;
	}
	else
	{
		fpMessageHandler	msgHandler;
		// dereference iterator and get message entry
		msgEntry entry = (*m_msgMapIterator).second; 
				
		msgHandler = entry.fpMsgHandler;

		// execute function
		return (this->*msgHandler)(hWnd, wParam, lParam);
	}

	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

	LRESULT CALLBACK 
CWindow::WindowProc(
	HWND				hWnd, 
	UINT				uMsg, 
	WPARAM				wParam, 
	LPARAM				lParam)
{
	CWindow				*pWindow	= NULL;
	MDICREATESTRUCT		*pMDIC		= NULL;

	if(uMsg == WM_NCCREATE)
	{
		assert(!::IsBadReadPtr((void *)lParam, sizeof(CREATESTRUCT)));

		pMDIC	= (MDICREATESTRUCT *)((LPCREATESTRUCT)lParam)->lpCreateParams;
		pWindow	= (CWindow *)(pMDIC->lParam);

		assert(!::IsBadReadPtr(pWindow, sizeof(CWindow)));

		::SetWindowLong(hWnd, GWL_USERDATA, (LONG) pWindow);
	}
	else
		pWindow = (CWindow *)::GetWindowLong(hWnd, GWL_USERDATA);

	if ((pWindow))
		return pWindow->WndProc(hWnd, uMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}

	void 
CWindow::GetWndClassEx(
	WNDCLASSEX	&wc)
{
	ZeroMemory(&wc, sizeof(wc));

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW;//0;
	wc.lpfnWndProc		= WindowProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= NULL;
	wc.hIcon			= NULL;
	wc.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)::GetStockObject(WHITE_BRUSH);//(HBRUSH)NULL;
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= NULL;
	wc.hIconSm			= NULL;
}

	HWND 
CWindow::CreateEx(
	DWORD			dwExStyle,
	LPCTSTR			lpszClass, 
	LPCTSTR			lpszName, 
	DWORD			dwStyle, 
	int				x, 
	int				y, 
	int				nWidth, 
	int				nHeight, 
	HWND			hParent, 
	HMENU			hMenu, 
	HINSTANCE		hInst)
{
	MDICREATESTRUCT	mdic;
	SIZE_T			nLen	= 0;

	if(!RegisterClass(lpszClass, hInst))
		return NULL;
	
	ZeroMemory(&mdic, sizeof(mdic));
	mdic.lParam		= (LPARAM)this;
	m_hWnd			= ::CreateWindowEx(dwExStyle, lpszClass, lpszName, 
							dwStyle, x, y, nWidth, nHeight, 
							hParent, hMenu, hInst, &mdic);

	m_hInst			= hInst;
	nLen			= _tcslen(lpszClass) + 1;
	m_pszClassName	= new TCHAR[nLen];

	ZeroMemory(m_pszClassName, nLen);
	_sntprintf(m_pszClassName, nLen - 1, _T("%s"), lpszClass);

	return m_hWnd;
}

	BOOL 
CWindow::RegisterClass(
	LPCTSTR		lpszClass, 
	HINSTANCE	hInst)
{
	WNDCLASSEX	wc;

	if(!::GetClassInfoEx(hInst, lpszClass, &wc))
	{
		GetWndClassEx(wc);

		wc.hInstance		= hInst;
		wc.lpszClassName	= lpszClass;

		if (!::RegisterClassEx(&wc))
			return FALSE;
	}

	return TRUE;
}

	WPARAM 
CWindow::MessageLoop()
{
	MSG	msg;

	while(::GetMessage(&msg, NULL, 0, 0))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return msg.wParam;
}

