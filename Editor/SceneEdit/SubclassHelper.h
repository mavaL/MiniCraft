/********************************************************************
	created:	4:4:2013   20:53
	filename	SubclassHelper.h
	author:		maval

	purpose:	´°¿Úsubclassing¸¨ÖúÀà
*********************************************************************/

#ifndef SubclassHelper_h__
#define SubclassHelper_h__

#include "Singleton.h"

class WndSubclassHelper : public CSingleton<WndSubclassHelper>
{
	DECLEAR_SINGLETON(WndSubclassHelper);
// public:
// 	static	void	Subclass(HWND hwnd, void* callee)
// 	{
// 		m_callee = callee;
// 		m_prev = (WNDPROC)SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)MyWndProc);
// 	}
// private:
// 	static LRESULT CALLBACK MyWndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// 	{
// 		if (!)
// 			return CallWindowProcW(m_prev, hWnd, uMsg, wParam, lParam);
// 	}
// 	WNDPROC		m_prev;
// 	T*			m_callee;
// 	T::
};


#endif // SubclassHelper_h__







