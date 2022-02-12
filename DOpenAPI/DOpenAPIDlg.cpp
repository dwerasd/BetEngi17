
// OpenAPIDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "DOpenAPI.h"
#include "DOpenAPIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenAPIDlg 대화 상자



COpenAPIDlg::COpenAPIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_OPENAPI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void COpenAPIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	pKiwoomOCX = theApp.키움OCX;
	DDX_Control(pDX, IDC_KHOPENAPICTRL, *pKiwoomOCX);	// 키움 필수
}

BEGIN_MESSAGE_MAP(COpenAPIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGING()

	ON_MESSAGE(WM_TRAYICON, &COpenAPIDlg::OnTrayMenu)
	ON_COMMAND(ID_TRAY_CLOSE, &COpenAPIDlg::OnTrayClose)
	ON_WM_SYSCOMMAND()
END_MESSAGE_MAP()


// COpenAPIDlg 메시지 처리기

BOOL COpenAPIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	ShowWindow(SW_SHOWMINIMIZED);
	PostMessage(WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);

	/*
	pTrayIcon = new dk::C_TRAY_ICON(
		m_hWnd
		, WM_TRAYICON
		, L"OpenAPI 1.0"
		, LoadIcon(theApp.m_hInstance, (LPCTSTR)IDR_MAINFRAME)
		, IDR_MAINFRAME
	);
	*/
	NOTIFYICONDATA nid;

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = this->m_hWnd;
	nid.uID = 0;
	Shell_NotifyIcon(NIM_DELETE, &nid);

	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	nid.hWnd = this->m_hWnd;

	CStringA str;
	GetWindowTextA(str);
	strcpy_s(nid.szTip, str.GetBuffer(str.GetLength() + 1));
	str.ReleaseBuffer();
	nid.uID = 0;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON;

	Shell_NotifyIcon(NIM_ADD, &nid);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void COpenAPIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR COpenAPIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT COpenAPIDlg::OnTrayMenu(WPARAM _wParam, LPARAM _lParam)
{
	_wParam;
	if (WM_RBUTTONUP == LOWORD(_lParam))
	{
		CPoint ptMouse;
		::GetCursorPos(&ptMouse);

		//char szStr[MAX_PATH * 3];
		CMenu menu;
		menu.LoadMenu(IDR_MENU_TRAY);
		CMenu *pMenu = menu.GetSubMenu(0);

		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, ptMouse.x, ptMouse.y, AfxGetMainWnd());
	}
	return(0);
}

void COpenAPIDlg::OnTrayClose()
{
	DBGPRINT("COpenAPIDlg::OnTrayClose()");
	Destroy();
}

void COpenAPIDlg::Destroy()							// 키움 필수
{
	if (!종료플래그)
	{
		종료플래그 = true;
		long nResult = pKiwoomOCX->GetConnectState();	// 키움 필수
		if (1 == nResult)
		{
			//theApp.RemoveConditions();
			DBGPRINT(">>> 모든화면 해제 SetRealRemove(ALL, ALL)");
			pKiwoomOCX->SetRealRemove(
				"ALL"									// 화면번호 또는 ALL
				, "ALL"									// 종목코드 또는 ALL
			);
		}
		else
		{
			DBGPRINT("알림: 키움서버에 접속중이 아닙니다");
		}
		//DSAFE_DELETE(pTrayIcon);
		NOTIFYICONDATA nid = { 0 };
		nid.cbSize = sizeof(NOTIFYICONDATA);
		nid.hWnd = m_hWnd;
		Shell_NotifyIcon(NIM_DELETE, &nid);

		this->EndDialog(IDCANCEL);
	}
}


void COpenAPIDlg::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//lpwndpos->flags &= ~SWP_SHOWWINDOW;
	//CDialog::OnWindowPosChanging(lpwndpos);
}


void COpenAPIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (SC_CLOSE == (nID & 0xFFF0))
	{
		Destroy();
	}
	__super::OnSysCommand(nID, lParam);
}
