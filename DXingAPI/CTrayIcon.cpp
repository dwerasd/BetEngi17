#include "framework.h"
#include "CTrayIcon.h"



C_TRAY_ICON::C_TRAY_ICON(HWND _hWnd, UINT _nCallbackMessage, LPCWSTR _szToolTip, HICON _hIcon, UINT _nID)
{
	ZeroMemory(&m_tnd, sizeof(m_tnd));
	m_tnd.cbSize = sizeof(NOTIFYICONDATAW);

	// Set NOTIFYICONDATA
	m_tnd.cbSize = sizeof(NOTIFYICONDATAW);
	m_tnd.hWnd = _hWnd;
	m_tnd.uID = _nID;
	m_tnd.hIcon = _hIcon;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = _nCallbackMessage;
	wcscpy_s(m_tnd.szTip, _szToolTip);

	::Shell_NotifyIconW(NIM_ADD, &m_tnd);
}

C_TRAY_ICON::C_TRAY_ICON(HWND _hWnd, UINT _nCallbackMessage, LPCSTR _szToolTip, HICON _hIcon, UINT _nID)
{
	ZeroMemory(&m_tnd, sizeof(m_tnd));
	m_tnd.cbSize = sizeof(NOTIFYICONDATAW);

	// Set NOTIFYICONDATA
	m_tnd.cbSize = sizeof(NOTIFYICONDATAW);
	m_tnd.hWnd = _hWnd;
	m_tnd.uID = _nID;
	m_tnd.hIcon = _hIcon;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = _nCallbackMessage;
	dk::AnsiToUtf16_s(m_tnd.szTip, sizeof(m_tnd.szTip), _szToolTip);

	::Shell_NotifyIconW(NIM_ADD, &m_tnd);
}

C_TRAY_ICON::~C_TRAY_ICON()
{
	m_tnd.uFlags = 0;
	::Shell_NotifyIconW(NIM_DELETE, &m_tnd);
	ZeroMemory(&m_tnd, sizeof(m_tnd));
}

BOOL C_TRAY_ICON::SetIcon(HICON hIcon)
{
	m_tnd.uFlags = NIF_ICON;
	m_tnd.hIcon = hIcon;

	return ::Shell_NotifyIconW(NIM_MODIFY, &m_tnd);
}

BOOL C_TRAY_ICON::SetIcon(LPCWSTR lpszIconName)
{
	HICON hIcon = ::LoadIconW((HINSTANCE)GetWindowLongW(m_tnd.hWnd, GWL_HINSTANCE), lpszIconName);

	return SetIcon(hIcon);
}

BOOL C_TRAY_ICON::SetIcon(UINT nIDResource)
{
	HICON hIcon = ::LoadIconW((HINSTANCE)GetWindowLongW(m_tnd.hWnd, GWL_HINSTANCE), MAKEINTRESOURCEW(nIDResource));

	return SetIcon(hIcon);
}

BOOL C_TRAY_ICON::SetStandardIcon(LPCWSTR lpIconName)
{
	HICON hIcon = ::LoadIconW(NULL, lpIconName);

	return SetIcon(hIcon);
}

BOOL C_TRAY_ICON::SetStandardIcon(UINT nIDResource)
{
	HICON hIcon = ::LoadIconW(NULL, MAKEINTRESOURCEW(nIDResource));

	return SetIcon(hIcon);
}

HICON C_TRAY_ICON::GetIcon() const
{
	return m_tnd.hIcon;
}

BOOL C_TRAY_ICON::SetTooltipText(LPCWSTR pszTip)
{
	m_tnd.uFlags = NIF_TIP;
	wcscpy_s(m_tnd.szTip, pszTip);

	return ::Shell_NotifyIconW(NIM_MODIFY, &m_tnd);
}

BOOL C_TRAY_ICON::SetTooltipText(LPCSTR pszTip)
{
	wchar_t wszBuffer[(1 << 7)] = { 0 };
	return SetTooltipText(dk::AnsiToUtf16_s(wszBuffer, sizeof(wszBuffer), pszTip));
}

LPCWSTR C_TRAY_ICON::GetTooltipText() const
{
	return m_tnd.szTip;
}

/////////////////////////////////////////////////////////////////////////////
// C_TRAY_ICON notification window stuff

BOOL C_TRAY_ICON::SetNotificationWnd(HWND hNotifyWnd)
{
	//Make sure Notification window is valid
	//ASSERT(hNotifyWnd);

	m_tnd.hWnd = hNotifyWnd;
	m_tnd.uFlags = 0;

	return ::Shell_NotifyIconW(NIM_MODIFY, &m_tnd);
}

HWND C_TRAY_ICON::GetNotificationWnd() const
{
	return m_tnd.hWnd;
}
