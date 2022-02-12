#pragma once


#include <shellapi.h>
#pragma comment(lib, "Shell32")

#include <DarkCore/DLocale.h>
#include <DarkCore/DSingleton.h>



#define WM_TRAYICON     (WM_USER + 100)
#define WM_TRAY_CLOSE	(WM_USER + 101)

class C_TRAY_ICON
	: public dk::C_SINGLETON<C_TRAY_ICON>   // 유일하니까
{
private:
	NOTIFYICONDATAW  m_tnd;

public:
	C_TRAY_ICON(HWND _hWnd, UINT _nCallbackMessage, LPCWSTR _szTip, HICON _hIcon, UINT _nID);
	C_TRAY_ICON(HWND _hWnd, UINT _nCallbackMessage, LPCSTR _szTip, HICON _hIcon, UINT _nID);
	~C_TRAY_ICON();

	//Change or retrieve the Tooltip text
	BOOL SetTooltipText(LPCWSTR pszTooltipText);
	BOOL SetTooltipText(LPCSTR pszTip);
	//BOOL SetTooltipText(UINT nID);
	LPCWSTR GetTooltipText() const;

	//Change or retrieve the icon displayed
	BOOL SetIcon(HICON hIcon);
	BOOL SetIcon(LPCWSTR lpIconName);


	BOOL SetIcon(UINT nIDResource);
	BOOL SetStandardIcon(LPCWSTR lpIconName);

	BOOL SetStandardIcon(UINT nIDResource);
	HICON GetIcon() const;

	//Change or retrieve the window to send notification messages to
	BOOL SetNotificationWnd(HWND hNotifyWnd);
	HWND GetNotificationWnd() const;
};
