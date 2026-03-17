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
 * $Id: Window.h,v 1.0 2004/01/15 01:45:41 antagonizt Exp $
 */

// Window.h: interface for the CWindow class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _WINDOW_H_
#define _WINDOW_H_

#pragma warning(disable:4786)
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <map>

class CWindow;

typedef LRESULT(CWindow::*fpMessageHandler)(HWND hWnd,WPARAM wParam,LPARAM lParam);

typedef struct _msgEntry
{
	fpMessageHandler fpMsgHandler;
} msgEntry, *pmsgEntry;

//
// Macros for msg handling
//
#define IMPLEMENT_MSG_HANDLER(base,derived) void derived::AddHandler(UINT msgId, LRESULT(derived::*Handler)(HWND hWnd,WPARAM wParam,LPARAM lParam))\
												{\
													AddMessageHandler(msgId, (LRESULT(base::*)(HWND hWnd,WPARAM wParam,LPARAM lParam))Handler);\
												}
#define DECLARE_MSG_HANDLER(derived) void AddHandler(UINT msgId, LRESULT(derived::*Handler)(HWND hWnd,WPARAM wParam,LPARAM lParam));\
										 void HandleManager(void);
#define BEGIN_MSG_MAP(derived) void derived::HandleManager(void) {
#define ADD_MSG_HANDLER(message,handler) AddHandler(message, handler);
#define END_MSG_MAP() }
#define ENABLE_MSG_MAP() HandleManager();

//
//	CWindow class
//
class CWindow  
{
public:
	CWindow();
	virtual ~CWindow();

	void					AddMessageHandler(UINT msgId, fpMessageHandler fpMsgHandler);

	BOOL					UpdateWindow(void) const { return ::UpdateWindow(m_hWnd); }
	virtual	WPARAM			MessageLoop(void);
	BOOL					ShowWindow(int nCmdShow) const { return ::ShowWindow(m_hWnd, nCmdShow); }
	virtual BOOL			RegisterClass(LPCTSTR lpszClass, HINSTANCE hInst);
	HWND					CreateEx(DWORD dwExStyle, LPCTSTR lpszClass, LPCTSTR lpszName, 
									DWORD dwStyle, int x, int y, int nWidth, int nHeight, 
									HWND hParent, HMENU hMenu, HINSTANCE hInst);
	
	virtual void			GetWndClassEx(WNDCLASSEX &wc);
	static LRESULT CALLBACK	WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT			WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	operator HWND() const { return m_hWnd; }

protected:
	TCHAR		*m_pszClassName;
	HWND		m_hWnd;
	HINSTANCE	m_hInst;

private:
	std::map<UINT,msgEntry>				m_msgMap;
	std::map<UINT,msgEntry>::iterator	m_msgMapIterator;

};

#endif // _WINDOW_H_
