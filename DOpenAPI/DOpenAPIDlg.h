
// OpenAPIDlg.h: 헤더 파일
//

#pragma once


#include <DarkCore/DSingleton.h>						// 키움 필수
#include <DarkCore/DTrayIcon.h>

#include "CKHOpenAPICtrl.h"			// 키움 필수


// COpenAPIDlg 대화 상자
class COpenAPIDlg
	: public CDialogEx
	, public dk::C_SINGLETON<COpenAPIDlg>				// 키움 필수
{
private:
	//dk::C_TRAY_ICON* pTrayIcon;
// 생성입니다.
public:
	COpenAPIDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPENAPI_DIALOG };
#endif
	bool 종료플래그{ false };
	C_KH_OPEN_API* pKiwoomOCX{ nullptr };				// 키움 필수
	void Destroy();										// 키움 필수

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg LRESULT OnTrayMenu(WPARAM _wParam, LPARAM _lParam);
	afx_msg void OnTrayClose();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
};
