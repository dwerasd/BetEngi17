#include "framework.h"
#include "CMain.h"



static C_MAIN* pMain = nullptr;
LRESULT CALLBACK WndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	static C_MAIN* pWindow = nullptr;
	if (pWindow)
	{
		if (pWindow->OnWindowMessage(_hWnd, _nMessage, _wParam, _lParam))
		{
			return(0);
		}
	}
	switch (_nMessage)
	{
	case WM_CREATE:
		if (!pWindow)
		{
			pWindow = (C_MAIN*)((::LPCREATESTRUCTW)_lParam)->lpCreateParams;

			//HBRUSH hBrush = ::CreateSolidBrush(RGB(30, 30, 30));
			//::SetClassLongPtr(_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
			//::DeleteObject(hBrush);
		}
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	}
	return ::DefWindowProcW(_hWnd, _nMessage, _wParam, _lParam);
}

LRESULT CALLBACK DlgProcMain(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	_lParam;
	switch (_nMessage)
	{
	case WM_CLOSE:
		//::DestroyWindow(_hWnd);
		::ShowWindow(_hWnd, SW_HIDE);	// X 버튼을 누르면 윈도우를 숨기기만 한다.
		pMain->ShowWindow(false);
		return(true);
	case WM_INITDIALOG:
		::SendMessageW(_hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIconW(::GetModuleHandleW(0), MAKEINTRESOURCEW(IDI_BETRADAR)));
		//::SendMessageW(::GetDlgItem(_hWnd, IDC_RADIO_TEST), BM_SETCHECK, BST_CHECKED, 0);
		return(true);
	case WM_COMMAND:
		switch (LOWORD(_wParam))
		{
		case ID_BTN_HIDE:
			::ShowWindow(_hWnd, SW_HIDE);	// X 버튼을 누르면 윈도우를 숨기기만 한다.
			pMain->ShowWindow(false);
			pMain->SetHighPerformance();
			break;
			break;
		case IDM_EXIT:
			::EndDialog(_hWnd, 0);
			pMain->ExitProcess();
			break;
		}
		return(true);
	case WM_DROPFILES:
		do
		{
			dk::DPOINT pt; // 어느 위치에 드롭되었는지 그 항목을 알아낸다.
			if (::DragQueryPoint((HDROP)_wParam, &pt))
			{	// 모두 몇 개의 파일이 드롭되었는지 알아낸다.
				UINT nCountDragItem = ::DragQueryFile((HDROP)_wParam, 0xFFFFFFFF, NULL, 0);
				for (UINT nIndex = 0; nIndex < nCountDragItem; nIndex++)
				{	// 만약 폴더가 드롭되었다면 폴더의 이름만 넘어온다.
					char szDtopPath[_MAX_PATH] = { 0 };
					::DragQueryFileA((HDROP)_wParam, nIndex, szDtopPath, _countof(szDtopPath));
					// "\"를 "/"로 변경
					size_t nPathLength = ::strlen(szDtopPath);
					for (size_t i = 0; i < nPathLength; i++) { if (szDtopPath[i] == '\\') { szDtopPath[i] = '/'; } }
					// 해당 경로가 존재하는가?
					long nResult = dk::파일체크(szDtopPath);
					if (1 == nResult)
					{	// 파일이면
						디뷰("파일이네: %s", szDtopPath);
						//DropFile(szDtopPath);
						//pGame->ReadFile(szDtopPath);
					}
					else if (2 == nResult)
					{	// 폴더라면 해당 폴더안의 파일들을 얻어온다.
						디뷰("폴더네: %s", szDtopPath);
						strcat_s(szDtopPath, "/");
						std::vector<std::string> vFileList;
						dk::GetFileListDirectoryA(vFileList, szDtopPath);

						std::sort(vFileList.begin(), vFileList.end());

						//size_t nCountAccrue = 0;
						for (size_t i = 0; i < vFileList.size(); i++)
						{
							std::string path = szDtopPath;
							path += vFileList[i];
							디뷰("파일이네: %s", path.c_str());
							//nCountAccrue += DropFile(path.c_str());

							//pGame->ReadFile(path.c_str());
							//디뷰("파일 처리에 걸린 시간: %0.6f", 퍼포먼스타이머[1].경과된시간());
						}
					}
					else
					{
						//디뷰("파일도 폴더도 아님. 버근가 - %s", szDtopPath);
					}
				}
				::DragFinish((HDROP)_wParam);	// drag and drop 작업을 끝낸다.
			}
		} while (false);
		return(true);
	}
	return(false);

}
C_MAIN::C_MAIN(HINSTANCE _hInst)
	: hInst(_hInst)
{
	if (!pMain) { pMain = this; }
}

C_MAIN::~C_MAIN()
{

}

static wchar_t wszWindowName[(1 << 7)] = { 0 };
bool C_MAIN::Init(LPCWSTR _wszClassName, LPCWSTR _wszWindowName, bool _bWindowMode)
{
	디뷰("C_MAIN::Init() - 메인 스레드: %x", ::GetCurrentThreadId());
	bWindowMode = _bWindowMode;

	bool bResult = false;
	do
	{
		char 임시버퍼[(1 << 10)] = { 0 };
		dk::GetCurrentDirectoryA(임시버퍼);
		기본경로 = 임시버퍼;

		// 실행파일 경로를 구하고
		::GetModuleFileNameA(::GetModuleHandleW(0), 임시버퍼, 배열크기(임시버퍼));
		// '\\' 를 '/' 로 수정
		for (size_t i = 0; i < ::strlen(임시버퍼); i++) { if ('\\' == 임시버퍼[i]) { 임시버퍼[i] = '/'; } }
		// 확장자를 자르고
		임시버퍼[::strlen(임시버퍼) - 3] = 0;
		// ini 를 붙인다.
		::strcat_s(임시버퍼, "ini");
		설정파일 = 임시버퍼;

		// 윈도우 크기를 여기서 얻어오므로 가장 먼저 초기화 해야한다.
		메인윈도우위치.x = ::GetPrivateProfileIntA("location", "x", 0, 설정파일.c_str());
		메인윈도우위치.y = ::GetPrivateProfileIntA("location", "y", 0, 설정파일.c_str());
		메인윈도우크기.cx = ::GetPrivateProfileIntA("location", "w", 1280, 설정파일.c_str());
		메인윈도우크기.cy = ::GetPrivateProfileIntA("location", "h", 800, 설정파일.c_str());

		// 콜백 세팅 떄문에 여기서 생성하도록 한다.

		::tagWNDCLASSEXW wcex = { sizeof(::tagWNDCLASSEXW), 0 };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInst;
		wcex.hIcon = ::LoadIconW(hInst, MAKEINTRESOURCEW(IDI_BETRADAR));
		wcex.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
		//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH);
		wcex.lpszClassName = _wszClassName;
		wcex.hIconSm = ::LoadIconW(hInst, MAKEINTRESOURCEW(IDI_SMALL));
		if (!::RegisterClassExW(&wcex)) { break; }

		pClassName = _wszClassName;

		::wcscpy_s(wszWindowName, _wszWindowName);
		::wcscat_s(wszWindowName, L" - v1.0");

		bResult = true;
	} while (false);
	디뷰(L"C_MAIN::Init(end) - [%s] ret: %d", wszWindowName, bResult);
	return(bResult);
}

bool C_MAIN::Create()
{
	bool bResult = false;
	do
	{
		DWORD dwWindowStyle = dk::GetWinFormStyle();
		dk::DRECT rcTemp(0, 0, 메인윈도우크기.cx, 메인윈도우크기.cy);
		//dk::DSIZE temp(rcTemp.right, rcTemp.bottom);
		::AdjustWindowRect(&rcTemp, dwWindowStyle, FALSE);
		hWnd = ::CreateWindowExW(
			WS_EX_ACCEPTFILES
			, pClassName
			, wszWindowName
			, dwWindowStyle
			, 메인윈도우위치.x
			, 메인윈도우위치.y
			, rcTemp.right - rcTemp.left
			, rcTemp.bottom - rcTemp.top
			, nullptr
			, nullptr
			, hInst
			, this
		);
		if (!hWnd) { break; }
		// 트레이 아이콘 만들고
		pTrayIcon = new dk::C_TRAY_ICON(hWnd, WM_TRAYICON, wszWindowName, ::LoadIconW(hInst, (LPCWSTR)IDI_BETRADAR), IDI_BETRADAR);

		//this->ThreadStart();

		bShowWindow = true;
		//::ShowWindow(hWnd, SW_SHOWNORMAL);
		//::UpdateWindow(hWnd);

		hDlgMain = ::CreateDialogParamW(hInst, MAKEINTRESOURCEW(IDD_MAIN), NULL, (DLGPROC)DlgProcMain, 0);
		::SetWindowPos(hDlgMain, NULL, 메인윈도우위치.x, 메인윈도우위치.y, 메인윈도우크기.cx, 메인윈도우크기.cy, SWP_NOSIZE);

		::SetWindowTextW(hDlgMain, L"BetRadar v1.0 - 서버접속: OFF");

		::ShowWindow(hDlgMain, SW_SHOW);
		::UpdateWindow(hDlgMain);

		if (!pNetClient)
		{	// 네트워크 패킷 처리하는 스레드 생성.
			this->ThreadStart();
			// 무한 재접속 클라
			pNetClient = new net::C_NET_CLIENT(this, "127.0.0.1", 20000);
			pNetClient->ThreadStart();
		}

		bResult = true;
	} while (false);
	return(bResult);
}

long C_MAIN::Calculate()
{
	long nResult = 0;
	do
	{
		if (bExitProcess) { nResult = 1; break; }
		ULONG_PTR nTickCount = dk::GetTickCount();
		if (nTickCount - nSecondTimer >= 1000)
		{
			::swprintf_s(wszWindowName, L"[BetServ]: %lld", nFrame);
			if (!bShowWindow) { pTrayIcon->SetTooltipText(wszWindowName); }
			//else { ::SetWindowTextW(hDlgMain, wszWindowName); }
			
			nFrame = 0;
			nSecondTimer = nTickCount;
		}
		if (bShowWindow)
		{

		}
		nFrame++;
		if (!bHighPerformance)
		{
			dk::멈춰();			// 약 000~015 밀리초 지연
		}
	} while (false);
	return(nResult);
}
void C_MAIN::Display()
{

}
void C_MAIN::Destroy() noexcept
{
	{	// 경로 저장.
		char 임시버퍼[(1 << 4)];
		dk::DRECT 임시렉트;
		::GetWindowRect(hDlgMain, &임시렉트);
		sprintf_s(임시버퍼, "%d", 임시렉트.left);
		::WritePrivateProfileStringA("location", "x", 임시버퍼, 설정파일.c_str());
		sprintf_s(임시버퍼, "%d", 임시렉트.top);
		::WritePrivateProfileStringA("location", "y", 임시버퍼, 설정파일.c_str());

		::GetClientRect(hDlgMain, &임시렉트);
		sprintf_s(임시버퍼, "%d", 임시렉트.right);
		::WritePrivateProfileStringA("location", "w", 임시버퍼, 설정파일.c_str());
		sprintf_s(임시버퍼, "%d", 임시렉트.bottom);
		::WritePrivateProfileStringA("location", "h", 임시버퍼, 설정파일.c_str());

		::DestroyWindow(hDlgMain);
	}
	::DestroyWindow(hWnd);
	::UnregisterClassW(pClassName, hInst);
	bExitProcess = true;
	DSAFE_DELETE(pTrayIcon);
}

bool C_MAIN::OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	switch (_nMessage)
	{
	case WM_COMMAND:
		switch (LOWORD(_wParam))
		{
		case WM_TRAY_CLOSE:
			bExitProcess = true;
			break;
		}
		return(true);
	case WM_TRAYICON:
		if (IDI_BETRADAR == _wParam)
		{
			if (WM_LBUTTONDBLCLK == LOWORD(_lParam))
			{	// 더블클릭 하면 윈도우를 보여준다.
				bShowWindow = true;
				::ShowWindow(hDlgMain, SW_SHOW);
				::UpdateWindow(hDlgMain);
			}
			if (WM_RBUTTONUP == LOWORD(_lParam))
			{	// 트레이 메뉴 사용시 윈도우 메시지를 처리하기 위한 메인스레드가 멈춘다.
				HMENU hMenu = ::CreatePopupMenu();

				//::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, WM_TRAY_CLOSE, __TEXT("CreonAPI"));
				//::InsertMenu(hMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, __TEXT("LINE"));
				//::AppendMenuW(hMenu, MF_STRING, 0, wszWindowName);
				//::AppendMenuW(hMenu, MF_SEPARATOR, 0, L"LINE");
				::AppendMenuW(hMenu, MF_STRING, WM_TRAY_CLOSE, L"종료");

				::SetForegroundWindow(_hWnd);	// 생성된 팝업 메뉴 밖을 클릭할 때 팝업 닫기

				dk::DPOINT pt;
				::GetCursorPos(&pt);
				::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, _hWnd, NULL);
			}
		}
		return(true);
	}
	return(false);
}

void C_MAIN::ReceivePacket(LPNET_PACKET_BUNDLE _pData)
{	// inet::C_NET_CLIENT 에서 여기로 패킷을 넣어준다.
	queueNetworkPackets.enqueue(_pData);
}

DWORD C_MAIN::ThreadFunc(LPVOID _pParam)
{	// TCP 네트워크 클라이언트 스레드
	_pParam;
	do
	{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에 
		LPNET_PACKET_BUNDLE pNetPacketBundle = nullptr;
		try
		{
			queueNetworkPackets.wait_dequeue(pNetPacketBundle);
			if (pNetPacketBundle)
			{
				LPPACKET_BASE pNetPacket = (LPPACKET_BASE)&pNetPacketBundle->bytBuffer[0];
				if (_PKT_NET_PACKET_NONE_ < pNetPacket->nPacketIndex && _PKT_PACKET_INDEX_MAX_ > pNetPacket->nPacketIndex)
				{	// pNetPacket 은 할당받아 온 것이라서 여기에서 쓰고 해제하면 된다.
					switch (pNetPacket->nPacketIndex)
					{
					case _PKT_NET_PACKET_NONE_:
						DBGPRINT("C_NET_SERVER::ThreadFunc() - 받음(_PKT_INDEX_NONE): %d", pNetPacket->nPacketIndex);
						break;
					case _PKT_NET_CONNECTED_:
						DBGPRINT("C_NET_SERVER::ThreadFunc() - 받음(_PKT_NET_CONNECTED_): %s / 세션: %x", pNetPacket->bytBuffer, pNetPacketBundle->pSession);
						break;
					case _PKT_NET_DISCONNECTED_:
						DBGPRINT("C_NET_SERVER::ThreadFunc() - 받음(_PKT_NET_DISCONNECTED_): %s / 세션: %x", pNetPacket->bytBuffer, pNetPacketBundle->pSession);
						pNetClient->Disconnect();
						break;
					}
				}
			}
		}
		catch (...)
		{
			디뷰메시지("Unexpected exception");
		}
		DSAFE_DELETE(pNetPacketBundle);
		//DBGPRINT("처리한 패킷 할당 해제");
	} while (true);
	return(0);
}