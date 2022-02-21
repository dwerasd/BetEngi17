#include "framework.h"
#include "CMain.h"



static C_MAIN* pMain = nullptr;
LPVOID __stdcall BridgeCallbackCreon(ULONG_PTR _dwMessage, LPVOID _pData);
LPVOID __stdcall BridgeCallbackKiwoom(ULONG_PTR _dwMessage, LPVOID _pData);
LPVOID __stdcall BridgeCallbackEBest(ULONG_PTR _dwMessage, LPVOID _pData);

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
		::SendMessageW(_hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIconW(::GetModuleHandleW(0), MAKEINTRESOURCEW(IDI_BETSERV)));
		//::SendMessageW(::GetDlgItem(_hWnd, IDC_RADIO_TEST), BM_SETCHECK, BST_CHECKED, 0);
		return(true);
	case WM_COMMAND:
		switch (LOWORD(_wParam))
		{
		case ID_BTN_HIDE:
			::ShowWindow(_hWnd, SW_HIDE);	// X 버튼을 누르면 윈도우를 숨기기만 한다.
			pMain->ShowWindow(false);
			pMain->bHighPerformance = true;
			break;
		case ID_BTN_SAVE:
			do
			{
				char 임시버퍼[_MAX_PATH] = { 0 };
				::GetPrivateProfileStringA("path", "save", "F:/data/ticks/", 임시버퍼, 배열크기(임시버퍼), pMain->설정파일.c_str());
				std::string 저장경로 = 임시버퍼;

				::time_t tToday = ::time(0);		// 오늘 날짜를 구함
				::tm t;
				::localtime_s(&t, &tToday);
				::strftime(임시버퍼, _countof(임시버퍼), "%Y%m%d", &t);		// "20201215", 오늘 날짜 저장용
				저장경로 += 임시버퍼;
				저장경로 += ".tick";

				for (size_t i = 0; i < pMain->nCountAccrueTick; i++)
				{
					LPTICK_DATA pData = (LPTICK_DATA)(pMain->pTickBuffer + (sizeof(TICK_DATA) * i));
					//sprintf_s(szTime, "%08x", pData->nTime);
					// 모든 체결 데이터 endian 을 변경한다
					dk::ntohl(pData->nTime);
				}
				DBGPRINT("[DlgProcMain] 총 %d 개 endian 변환 완료", pMain->nCountAccrueTick);
				size_t nSaveSize = (pMain->pTickBufferPtr - pMain->pTickBuffer);
				dk::C_FILE whiteFile(저장경로
					, GENERIC_WRITE																// 쓰기만할꺼고
					, 0																			// 열었을때 다른곳에서 접근 불가.
					, dk::FileExists(저장경로) ? OPEN_ALWAYS : CREATE_ALWAYS					// 파일이 있으면 열고 없으면 만든다.
					, FILE_ATTRIBUTE_NORMAL// | FILE_FLAG_WRITE_THROUGH							// 캐싱하지 않고 파일에 바로 쓴다.);
				);

				whiteFile.Write(pMain->pTickBuffer, (ULONG)nSaveSize);
				DBGPRINT("[DlgProcMain] 총 %d 개 쓰기 완료", pMain->nCountAccrueTick);
				whiteFile.Destroy();
			} while (false);
			break;
		case ID_BTN_SAVE_CSV:

			break;
		case IDM_EXIT:
			::EndDialog(_hWnd, 0);
			디뷰("종료");
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
						//pEngine->ReadFile(szDtopPath);
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

							//pEngine->ReadFile(path.c_str());
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
		wcex.hIcon = ::LoadIconW(hInst, MAKEINTRESOURCEW(IDI_BETSERV));
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
		pTrayIcon = new dk::C_TRAY_ICON(hWnd, WM_TRAYICON, wszWindowName, ::LoadIconW(hInst, (LPCWSTR)IDI_BETSERV), IDI_BETSERV);

		this->ThreadStart();

		bShowWindow = true;
		//::ShowWindow(hWnd, SW_SHOWNORMAL);
		//::UpdateWindow(hWnd);

		hDlgMain = ::CreateDialogParamW(hInst, MAKEINTRESOURCEW(IDD_MAIN), NULL, (DLGPROC)DlgProcMain, 0);
		::SetWindowPos(hDlgMain, NULL, 메인윈도우위치.x, 메인윈도우위치.y, 메인윈도우크기.cx, 메인윈도우크기.cy, SWP_NOSIZE);

		::ShowWindow(hDlgMain, SW_SHOW);
		::UpdateWindow(hDlgMain);

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
			if (bShowWindow) { ::SetWindowTextW(hDlgMain, wszWindowName); }
			pTrayIcon->SetTooltipText(wszWindowName);
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
		if (IDI_BETSERV == _wParam)
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
{	// inet::C_IOCP_SERVER 에서 여기로 패킷을 넣어준다.
	queueNetworkPackets.enqueue(_pData);
}

LPTICK_DATA C_MAIN::AppendTick(LPKIWOOM_REALDATA_TRANSACTION _pData)
{	// 일단 틱 데이터를 연속된 메모리에 변환해서 쌓는다.
	LPTICK_DATA pTickData = nullptr;
	if (pTickBuffer)
	{
		pTickData = (LPTICK_DATA)pTickBufferPtr;
		::memset(pTickData, 0, sizeof(TICK_DATA));

		pTickData->nSequence = nCountAccrueTick++;	// 더하고 증가, 순서는 0부터 기록한다.
		// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
		strncpy_s(pTickData->szCode, 배열크기(pTickData->szCode) * sizeof(char), _pData->종목코드, 6);
		pTickData->szCode[6] = 0;
		// 이 함수가 호출된 시스템 시간을 얻어온다.
		dk::DLOCAL_TIME currentTime;
		// 밀리초를 더한다. ( 시간 변환은 여기서 저장용으로 한번만 한다.
		char szTime[(1 << 4)] = { 0 };
		// 아직 저장할게 아니니까 endian 변환은 하지 않는다.
		//sprintf_s(szTime, "%08x", (::atoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		//pTickData->nTime = ::ntohl(::strtol(szTime, NULL, 16));
		sprintf_s(szTime, "%d", (::atoi(_pData->체결시간) * 1000) + currentTime.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pTickData->nTime = ::atoi(szTime);

		// 매수 매도인지 여기서 판단 후에 종목에 넘겨주도록 하자.
		if ('+' == _pData->체결량[0]) { pTickData->nTransType = 1; }		// 매수 체결
		else if ('-' == _pData->체결량[0]) { pTickData->nTransType = 2; }	// 매도체결
		else
		{	/*
			// 단일가 매매의 경우 전일 종가와 비교해 부호가 붙는다.
			if ('+' == _szClose[0])
			{
				pTickData->nTransType = 4;
			}
			else if ('-' == _szClose[0])
			{
				pTickData->nTransType = 5;
			}
			else
			*/
			{
				pTickData->nTransType = 3;	// 전일종가와 동일할 경우
			}
		}
		// 부호가 붙어 있으면 무시하고 숫자만 변환
		pTickData->fPrice = (float)dk::atoi_s(_pData->체결가);
		pTickData->nTransVolume = dk::atoi_s(_pData->체결량);
		pTickData->nAccrueVolume = dk::atoi_s(_pData->누적거래량);
		pTickData->최우선매도호가 = (float)dk::atoi_s(_pData->최우선매도호가);
		pTickData->최우선매수호가 = (float)dk::atoi_s(_pData->최우선매수호가);
		//디뷰("%d / %d", pTickData->nTransVolume, pTickData->nAccrueVolume);	// 왜 이걸 안쓰면 누락이 발생하는거지
		pTickBufferPtr += sizeof(TICK_DATA);	// 채운만큼 포인터 이동
	}
	return(pTickData);
}

LPTICK_DATAEX C_MAIN::AppendTickEx(LPKIWOOM_REALDATA_TRANSACTION _pData, LPORDERBOOK_KIWOOM _pOrderBook)
{	// 일단 틱 데이터를 연속된 메모리에 변환해서 쌓는다.
	LPTICK_DATAEX pTickDataEx = nullptr;
	if (pTickBuffer)
	{
		pTickDataEx = (LPTICK_DATAEX)pTickBufferPtr;
		::memset(pTickDataEx, 0, sizeof(TICK_DATAEX));

		pTickDataEx->nSequence = nCountAccrueTick++;	// 더하고 증가, 순서는 0부터 기록한다.
		// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
		::strncpy_s(pTickDataEx->szCode, _countof(pTickDataEx->szCode) * sizeof(char), _pData->종목코드, 6);
		pTickDataEx->szCode[6] = 0;
		// 이 함수가 호출된 시스템 시간을 얻어온다.
		dk::DLOCAL_TIME currentTime;
		// 밀리초를 더한다. ( 시간 변환은 여기서 저장용으로 한번만 한다.
		char szTime[(1 << 4)] = { 0 };
		// 아직 저장할게 아니니까 endian 변환은 하지 않는다.
		//sprintf_s(szTime, "%08x", (::atoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		//pTickDataEx->nTime = ::ntohl(::strtol(szTime, NULL, 16));
		sprintf_s(szTime, "%d", (::atoi(_pData->체결시간) * 1000) + currentTime.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pTickDataEx->nTime = ::atoi(szTime);

		// 매수 매도인지 여기서 판단 후에 종목에 넘겨주도록 하자.
		if ('+' == _pData->체결량[0]) { pTickDataEx->nTransType = 1; }		// 매수 체결
		else if ('-' == _pData->체결량[0]) { pTickDataEx->nTransType = 2; }	// 매도체결
		else
		{	/*
			// 단일가 매매의 경우 전일 종가와 비교해 부호가 붙는다.
			if ('+' == _szClose[0])
			{
				pTickDataEx->nTransType = 4;
			}
			else if ('-' == _szClose[0])
			{
				pTickDataEx->nTransType = 5;
			}
			else
			*/
			{
				pTickDataEx->nTransType = 3;	// 전일종가와 동일할 경우
			}
		}
		// 부호가 붙어 있으면 무시하고 숫자만 변환
		pTickDataEx->fPrice = (float)dk::atoi_s(_pData->체결가);
		pTickDataEx->nTransVolume = dk::atoi_s(_pData->체결량);
		pTickDataEx->nAccrueVolume = dk::atoi_s(_pData->누적거래량);
		pTickDataEx->최우선매도호가 = (float)dk::atoi_s(_pData->최우선매도호가);
		pTickDataEx->최우선매수호가 = (float)dk::atoi_s(_pData->최우선매수호가);


		pTickDataEx->시가 = dk::atoi_s(_pData->szOpen);
		pTickDataEx->고가 = dk::atoi_s(_pData->szHigh);
		pTickDataEx->저가 = dk::atoi_s(_pData->szLow);

		pTickDataEx->누적거래대금 = dk::atoi_s(_pData->누적거래대금);
		pTickDataEx->시가총액_억 = dk::atoi_s(_pData->시가총액);

		pTickDataEx->등락율 = (float)dk::atoi_s(_pData->등락율);
		pTickDataEx->전일거래량대비_비율 = (float)dk::atoi_s(_pData->전일거래량대비);
		pTickDataEx->거래회전율 = (float)dk::atoi_s(_pData->거래회전율);
		pTickDataEx->체결강도 = (float)dk::atoi_s(_pData->체결강도);
		pTickDataEx->전일동시간거래량비율 = (float)dk::atoi_s(_pData->전일동시간거래량비율);
		if (_pOrderBook)
		{
			pTickDataEx->매도호가총잔량 = _pOrderBook->매도호가총잔량;
			pTickDataEx->매수호가총잔량 = _pOrderBook->매수호가총잔량;
			pTickDataEx->매도비율 = _pOrderBook->매도비율;
		}
		//디뷰("%d / %d", pTickDataEx->nTransVolume, pTickDataEx->nAccrueVolume);	// 왜 이걸 안쓰면 누락이 발생하는거지
		pTickBufferPtr += sizeof(TICK_DATAEX);	// 채운만큼 포인터 이동
	}
	return(pTickDataEx);
}

DWORD C_MAIN::ThreadFunc(LPVOID _pParam)
{
	DBGPRINT("C_NET_SERVER::ThreadFunc() - 네트워크 패킷 처리 스레드");
	_pParam;
	if (!pNet)
	{
		pNet = new net::C_NET_SERVER(this);
	}
	pNet->SetValueWorkerThread(1);
	pNet->ThreadStart();
	pNet->StartAcceptingConnections();
	DBGPRINT("C_NET_SERVER::ThreadFunc() - 네트워크 접속 처리 스레드 생성");

	nCountAccrueTick = 0;
	힙해제(pTickBuffer);
	pTickBuffer = pTickBufferPtr = (LPBYTE)힙할당(sizeof(TICK_DATA) * 40000000);
	디뷰("sizeof(체결틱): %d / 힙크기(pTickBuffer): %zd", sizeof(TICK_DATA), 힙크기(pTickBuffer));

	힙해제(pTickBufferEBest);

	LPNET_PACKET_BUNDLE pNetPacketBundle = nullptr;
	inet::C_SESSION* pSession = nullptr;
	do
	{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에 
		pNetPacketBundle = nullptr;
		try
		{
			queueNetworkPackets.wait_dequeue(pNetPacketBundle);
			if (pNetPacketBundle)
			{	// pSession 이 nullptr 일 수는 없다.
				pSession = (inet::C_SESSION*)pNetPacketBundle->pSession;
				// 사이즈는 이제 의미가 없나? 내용만 처리하면 될 것 같다.
				if (pSession)
				{	// 세션이 없을 수는 없다.

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
							{	// 접속 패킷을 받으면 같은 헤더로 "_PKT_NET_CONNECTED_" 라는 문자열까지 보내준다.
								char szBuffer[] = "_PKT_NET_CONNECTED_";
								pSession->Send(_PKT_NET_CONNECTED_, (LPBYTE)szBuffer, (WORD)::strlen(szBuffer));
							}
							break;
						case _PKT_NET_DISCONNECTED_:
							pSession->Shutdown();
							break;
						case _네트워크패킷_키움_주식체결_:
							do
							{
								LPKIWOOM_REALDATA_TRANSACTION pData = (LPKIWOOM_REALDATA_TRANSACTION)pNetPacket->bytBuffer;
								LPORDERBOOK_KIWOOM pStock = 키움호가데이터[pData->종목코드];
								if (!pStock)
								{	// 아직 저장된적 없는 종목이니 생성함.
									pStock = 키움호가데이터[pData->종목코드] = new ORDERBOOK_KIWOOM();
									::memset(pStock, 0, sizeof(ORDERBOOK_KIWOOM));
								}
								//AppendTick(pData);
								AppendTickEx(pData, pStock);
							} while (false);
							break;
						case _네트워크패킷_키움_주식호가잔량_:
							do
							{
								키움_주식호가잔량포 pData = (키움_주식호가잔량포)pNetPacket->bytBuffer;

								LPORDERBOOK_KIWOOM pOrderBook = 키움호가데이터[pData->종목코드];
								if (!pOrderBook)
								{	// 아직 저장된적 없는 종목이니 생성함.
									pOrderBook = 키움호가데이터[pData->종목코드] = new ORDERBOOK_KIWOOM();
								}
								::strcpy_s(pOrderBook->종목코드, pData->종목코드);
								pOrderBook->매도호가총잔량 = ::atoi(pData->매도호가총잔량);
								pOrderBook->매수호가총잔량 = ::atoi(pData->매수호가총잔량);
								pOrderBook->매도비율 = (float)::atof(pData->매도비율);
							} while (false);
							
							break;
						case _PKT_NET_RECEIVE_STOCKINFO_KIWOOM_:
							do
							{
								LPPACKET_BASE pPacket = (LPPACKET_BASE)pNetPacket->bytBuffer;
								LPSTOCK_INFO_KIWOOM pStockInfo = (LPSTOCK_INFO_KIWOOM)pPacket->bytBuffer;
								디뷰("[%s] %s", pStockInfo->szStockCode, pStockInfo->szStockName);
							} while (false);
							break;
						case _PKT_NET_RECEIVE_TRANSACTION_EBEST_:

							break;
						case _PKT_NET_REQUEST_LOGIN_:
							do
							{
								LPNET_MESSAGE_LOGIN pMessage = (LPNET_MESSAGE_LOGIN)pNetPacket->bytBuffer;
								if (0 == pMessage->bytLocale)
								{
									DBGPRINT("C_NET_SERVER::ThreadFunc() - 로그인 멀바: %s / %s", pMessage->szAccount, pMessage->szPassword);
									pSession->Send(_PKT_NET_RESULT_LOGIN_SUCCEED_);
								}
								else if (1 == pMessage->bytLocale)
								{
									DBGPRINT(L"C_NET_SERVER::ThreadFunc() - 로그인 유니: %s / %s", pMessage->wszAccount, pMessage->wszPassword);
									pSession->Send(_PKT_NET_RESULT_LOGIN_FAILED_);
								}
								//if (pDB->CheckAccount(pLoginPacket->szAccount))
								//{
								//	DBGPRINT("계정이 있어! - %s", pLoginPacket->szAccount);
								//}
								//else
								//{
								//	DBGPRINT("계정이 없어! - %s", pLoginPacket->szAccount);
								//}

							} while (false);
							break;
						case _PKT_NET_EXECUTE_:
							do
							{
								LPNET_MESSAGE_EXECUTE_API pMessage = (LPNET_MESSAGE_EXECUTE_API)pNetPacket->bytBuffer;
								switch (pMessage->nAPIType)
								{
								case 0:
									break;
								case 1:
									break;
								case 2:
									break;
								}
							} while (false);
							break;
						}
					}
				}
				else
				{
					DBGPRINT("C_NET_SERVER::ThreadFunc() - C_NET_HANDLER::ThreadFunc() - 세션이 없을 수는 없는데?");
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
