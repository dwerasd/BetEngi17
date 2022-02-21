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
	return DefWindowProcW(_hWnd, _nMessage, _wParam, _lParam);
}

C_ENGINE* g_pGame = nullptr;
static _TCHAR wszWindowName[(1 << 7)] = { 0 };
C_MAIN::C_MAIN(HINSTANCE _hInst)
	: hInst(_hInst)
	, nStartTime(0)
	, nEndTime(0)
	, bInSecond5(false)
{
	if (!pMain) { pMain = this; }
	if (!pLog)
	{
		pLog = new dk::C_LOG_EX("DBetMain");
	}
	디뷰("sizeof(C_ENGINE): %d", 크기(C_ENGINE));

	//디뷰("sizeof(C_ENGINE): %d", 크기(C_ENGINE));
	//디뷰("sizeof(_데이터_배열_확장<762>): %d", 크기(_데이터_배열_확장<762>));
	//디뷰("sizeof(종목_데이터_리얼): %d", 크기(종목_데이터_리얼));
}

C_MAIN::~C_MAIN()
{
	DSAFE_DELETE(pBridgeCreon);
	DSAFE_DELETE(pBridgeKiwoom);
	DSAFE_DELETE(pBridgeEBest);
	DSAFE_DELETE(pEngine);
}

bool C_MAIN::Init(LPCWSTR _wszClassName, LPCWSTR _wszWindowName, bool _bWindowMode)
{
	디뷰("C_MAIN::Init() - 메인 스레드: %x", ::GetCurrentThreadId());
	//디뷰("프로세스의 코어: %d", ::GetCurrentProcessorNumber());
	//SetProcessAffinityMask();
	//SetThreadAffinityMask();

	pLog->Write("C_MAIN::Init() - 로그 기록을 시작합니다");
	디뷰(L"C_MAIN::Init(start) - %s / %s / winmode: %d", _wszClassName, _wszWindowName, _bWindowMode);
	bWindowMode = _bWindowMode;
	bool bResult = false;
	do
	{	// 윈도우 크기를 여기서 얻어오므로 가장 먼저 초기화 해야한다.
		if (!설정포) { 설정포 = new C_CONFIG(); }
		if (설정포) { 설정포->초기화(); }
#if defined(_USE_LIB_IMGUI_)
		bDraw[_팝업_메인메뉴_] = (bool)::GetPrivateProfileIntA("base", "popup_menu_main", 0, 설정포->경로_설정파일());
		bDraw[_팝업_현황판_] = (bool)::GetPrivateProfileIntA("popup", "popup_statusboard", 0, 설정포->경로_설정파일());
		bDraw[_팝업_옵션_] = (bool)::GetPrivateProfileIntA("popup", "popup_option", 0, 설정포->경로_설정파일());
		bDraw[_팝업_명령_] = (bool)::GetPrivateProfileIntA("popup", "popup_order", 0, 설정포->경로_설정파일());
		bDraw[_팝업_정보_] = (bool)::GetPrivateProfileIntA("popup", "popup_info", 0, 설정포->경로_설정파일());
		bDraw[_팝업_잔고_] = (bool)::GetPrivateProfileIntA("popup", "popup_bank", 0, 설정포->경로_설정파일());
		bDraw[_팝업_미체결_] = (bool)::GetPrivateProfileIntA("popup", "popup_waiting", 0, 설정포->경로_설정파일());

		bDraw[_팝업_시뮬레이션_] = (bool)::GetPrivateProfileIntA("popup", "popup_simulation", 0, 설정포->경로_설정파일());
#endif
		::tagWNDCLASSEXW wcex = { sizeof(::tagWNDCLASSEXW), 0 };
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.hInstance = hInst;
		wcex.hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(IDI_BETMAIN));
		wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
		//wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH);
		wcex.lpszClassName = _wszClassName;
		wcex.hIconSm = ::LoadIconW(hInst, MAKEINTRESOURCE(IDI_SMALL));

		if (!::RegisterClassExW(&wcex)) { break; }

		pClassName = _wszClassName;

		::wcscpy_s(wszWindowName, _wszWindowName);
		::wcscat_s(wszWindowName, L" - v1.0");

		bResult = true;
	} while (false);
	디뷰(L"C_MAIN::Init(end) - %s / ret: %d", wszWindowName, bResult);
	return(bResult);
}

bool C_MAIN::Create()
{
	bool bResult = false;
	do
	{
		DWORD dwWindowStyle = dk::GetWinFormStyle();
		dk::DRECT rcTemp(0, 0, 설정포->메인윈도우크기.cx, 설정포->메인윈도우크기.cy);
		//dk::DSIZE temp(rcTemp.right, rcTemp.bottom);
		::AdjustWindowRect(&rcTemp, dwWindowStyle, FALSE);
		hWnd = ::CreateWindowExW(
			WS_EX_ACCEPTFILES
			, pClassName
			, wszWindowName
			, dwWindowStyle
			, 설정포->메인윈도우위치.x
			, 설정포->메인윈도우위치.y
			, rcTemp.right - rcTemp.left
			, rcTemp.bottom - rcTemp.top
			, nullptr
			, nullptr
			, hInst
			, this
		);
		if (!hWnd) { break; }
		// 트레이 아이콘 만들고
		pTrayIcon = new dk::C_TRAY_ICON(hWnd, WM_TRAYICON, wszWindowName, ::LoadIcon(hInst, (LPCWSTR)IDI_BETMAIN), IDI_BETMAIN);

		bShowWindow = true;
		::ShowWindow(hWnd, SW_SHOWNORMAL);
		::UpdateWindow(hWnd);

		if (!pEngine)
		{
			pEngine = new C_ENGINE();
			pEngine->초기화();
		}
		// 콜백 세팅 떄문에 여기서 생성하도록 한다.
		if (!pBridgeCreon)
		{
			pBridgeCreon = new C_BRIDGE_BASE(BridgeCallbackCreon, L"PipeServerCreon", L"PipeClientCreon");
			pBridgeCreon->Init("DCreonAPI.exe");
		}
		if (!pBridgeKiwoom)
		{	// 키움 실행을 누르면 생성하는걸로 하자.
			pBridgeKiwoom = new C_BRIDGE_KIWOOM(BridgeCallbackKiwoom, L"PipeServerKiwoom", L"PipeClientKiwoom");
			pBridgeKiwoom->Init("DOpenAPI.exe");
		}
		if (!pBridgeEBest)
		{	// 이베스트 실행을 누르면 생성하는걸로 하자.
			pBridgeEBest = new C_BRIDGE_BASE(BridgeCallbackEBest, L"PipeServerEBest", L"PipeClientEBest");
			pBridgeEBest->Init("DXingAPI.exe");
		}
#if defined(_USE_LIB_IMGUI_)
		디뷰("C_MAIN::Create() - 임구이 초기화 시작");
		C_IMGUI::SetWindowSize(설정포->메인윈도우크기.cx, 설정포->메인윈도우크기.cy);
		C_IMGUI::Init_ImGui(hWnd, nullptr, false);
		디뷰("C_MAIN::Create() - 임구이 초기화 끝.");
#endif
		//if (자동실행_크레온)
		//{
		//	do
		//	{	// 크레온 중계기 스레드 시작을 기다린다.
		//		dk::멈춰(1);
		//	} while (!파이프서버[_크레온_]->IsOnThread() && !파이프받는스레드);
		//	// 크레온 중계기를 파이프 모드로 실행한다.
		//	::WritePrivateProfileStringA("base", "server", "0", pEngine->크레온설정파일.c_str());
		//	dk::LPPROCESS_INFOMATION_EX 프로세스 = pProcessInfo["DCreonAPI"];
		//	bool 성공여부 = dk::파일실행(프로세스->파일이름, 프로세스->파라미터, true, (LPPROCESS_INFORMATION)프로세스);
		//	if (!성공여부) { 디뷰("C_MAIN::Create() - 크레온 중계기 실행에 실패함"); }
		//	else { 디뷰("C_MAIN::Create() - 크레온 중계기 실행 완료"); }
		//}
#if 0
		size_t 카운트 = 0;
		do
		{	// 타이머 시작을 최대한 0.000 초에 맞추기 위해.
			dk::DLOCAL_TIME currentTime;
			if (currentTime.wSecond != 연산함수_초버퍼)
			{	// 여긴 초가 바뀔때마다 한번씩 들어온다.
				if (0 < 카운트++)
				{
					::SetTimer(hWnd, _기본타이머_, 200, 0);	// TIMERPROC 함수를 지정하지 않으면 WM_TIMER 메시지로 발생한다.
					break;
				}
				연산함수_초버퍼 = currentTime.wSecond;
			}
		} while (true);
#else
		::SetTimer(hWnd, _기본타이머_, 200, 0);	// TIMERPROC 함수를 지정하지 않으면 WM_TIMER 메시지로 발생한다.
#endif
		bResult = true;
	} while (false);
	return(bResult);
}

#if defined(_USE_LIB_IMGUI_)
long C_MAIN::Update_ImGui()
{
	long nResult = 0;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	do
	{
		nResult = Draw_MainMenu(bDraw[_팝업_메인메뉴_]);
		if (0 > nResult)
		{
			break;
		}
		Draw_Popup_StatusBoard(bDraw[_팝업_현황판_]);
		Draw_Popup_Option(bDraw[_팝업_옵션_]);
		Draw_Popup_Order(bDraw[_팝업_명령_]);
		Draw_Popup_Info(bDraw[_팝업_정보_]);
		Draw_Popup_Bank(bDraw[_팝업_잔고_]);
		Draw_Popup_WaitingList(bDraw[_팝업_미체결_]);
	} while (false);
	ImGui::EndFrame();

	return(nResult);
}
#endif

long C_MAIN::Calculate()
{	// 메인 스레드는 최대한 가볍게 가자.
	long nResult = 0;
	do
	{
		if (bExitProcess) { nResult = 1; break; }
		
		ULONG_PTR nTickCount = dk::GetTickCount();
		if (nTickCount - nSecondTimer >= 1000)
		{
			::_stprintf_s(wszWindowName, __TEXT("[BetMain]: %lld"), nFrame);
			if (bShowWindow) { ::SetWindowText(hWnd, wszWindowName); }
			pTrayIcon->SetTooltipText(wszWindowName);
			nFrame = 0;
			nSecondTimer = nTickCount;
		}
#if defined(_USE_LIB_IMGUI_)
		if (bShowWindow)
		{
			if (0 > Update_ImGui())
			{
				//디뷰("종료눌린거다");
				bExitProcess = true;
				break;
			}
		}
#endif
		/* 뭐야 이걸 무한호출 하면 느리니까 GetTickCount 를 쓰기로 한다.
		dk::DLOCAL_TIME currentTime;
		if (currentTime.wSecond != nSecondTimer)
		{	// 여긴 초가 바뀔때마다 한번씩 들어온다. ( 1초에 한번씩 )
			swprintf_s(wszRealTimeWindowName, L"[BetMain]: %zd", nFrame);
			SetWindowTextW(hWnd, wszRealTimeWindowName);

			nFrame = 0;
			nSecondTimer = currentTime.wSecond;
		}
		*/
		nFrame++;
		/*
		dk::DLOCAL_TIME currentTime;
		if (currentTime.wSecond != 연산함수_초버퍼)
		{	// 여긴 초가 바뀔때마다 한번씩 들어온다. ( 1초에 한번씩 )
			ULONG nCurrentSecond = _HMS_TO_SEC_(currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
			if (!currentTime.wDayOfWeek || 6 == currentTime.wDayOfWeek)
			{	// 일요일과 토요일이면

			}
			else
			{	// 주중이면
				if (!(nCurrentSecond % (_SECS_PER_HOUR_ / 12)))
				{	// 여긴 5분에 한번씩 들어온다.

					if (nCurrentSecond > _H_TO_SEC_(16))
					{	// 오후 네시보다 크면

					}
				}
			}
			if (!(nCurrentSecond % (_SECS_PER_HOUR_ / 12)))
			{	// 여긴 매일 5분에 한번씩 들어온다.
				//디뷰("%d시 %d분", currentTime.wHour, currentTime.wMinute);

			}
			연산함수_초버퍼 = currentTime.wSecond;
		}
		if (!(currentTime.wSecond % 5) && !bInSecond5)
		{	// 여긴 5초에 한번씩 들어온다.
			//디뷰("%d", currentTime.wSecond % 5);

			bInSecond5 = true;
		}
		else if (bInSecond5 && currentTime.wSecond % 5)
		{
			bInSecond5 = false;
		}
		*/
		if (!bHighPerformance)
		{
			dk::멈춰();
		}
	} while (false);
	return(nResult);
}

void C_MAIN::Display()
{
#if defined(_USE_LIB_IMGUI_)
	if (bShowWindow)
	{
		C_IMGUI::Draw_ImGui();
	}
#endif
}

void C_MAIN::Destroy() noexcept
{
	디뷰("C_MAIN::Destroy(start)");

	::KillTimer(hWnd, _기본타이머_);
	{	// 경로 저장.
		char 임시버퍼[(1 << 4)];
		dk::DRECT 임시렉트;
		::GetWindowRect(hWnd, &임시렉트);
		sprintf_s(임시버퍼, "%d", 임시렉트.left);
		::WritePrivateProfileStringA("location", "x", 임시버퍼, 설정포->설정파일.c_str());
		sprintf_s(임시버퍼, "%d", 임시렉트.top);
		::WritePrivateProfileStringA("location", "y", 임시버퍼, 설정포->설정파일.c_str());

		::GetClientRect(hWnd, &임시렉트);
		sprintf_s(임시버퍼, "%d", 임시렉트.right);
		::WritePrivateProfileStringA("location", "w", 임시버퍼, 설정포->설정파일.c_str());
		sprintf_s(임시버퍼, "%d", 임시렉트.bottom);
		::WritePrivateProfileStringA("location", "h", 임시버퍼, 설정포->설정파일.c_str());
	}
	::DestroyWindow(hWnd);
#if defined(_USE_LIB_IMGUI_)
	C_IMGUI::Destroy_ImGui();
#endif
	::UnregisterClass(pClassName, hInst);
	DSAFE_DELETE(pEngine);

	DSAFE_DELETE(pBridgeCreon);
	DSAFE_DELETE(pBridgeKiwoom);
	DSAFE_DELETE(pBridgeEBest);

	if (설정포)
	{
		설정포->소멸();
		DSAFE_DELETE(설정포);
	}
	bExitProcess = true;
	DSAFE_DELETE(pTrayIcon);
	pLog->Write("C_MAIN::Destroy(end) - 프로그램을 종료합니다");
}

void C_MAIN::스케줄생성(LPCSTR _파일명)
{
	::WritePrivateProfileStringA("execute", "DCreonAPI", "0", _파일명);
	::WritePrivateProfileStringA("execute", "DOpenAPI", "0", _파일명);
	::WritePrivateProfileStringA("execute", "DXingAPI", "0", _파일명);

	::WritePrivateProfileStringA("update", "sticks", "0", _파일명);
}

void C_MAIN::timer200()
{
	dk::DLOCAL_TIME currentTime;
	if (currentTime.wSecond != 기본타이머_초버퍼)
	{	// 여긴 초가 바뀔때마다 한번씩 들어온다.
		ULONG nCurrentSecond = _HMS_TO_SEC_(currentTime.wHour, currentTime.wMinute, currentTime.wSecond);

		if (currentTime.wDay != 기본타이머_일버퍼)
		{	// 날짜가 바뀌었는지 체크한다.

			char 임시텍스트버퍼[(1 << 4)] = { 0 };
			time_t tToday = ::time(0);
			tm t;
			::localtime_s(&t, &tToday);
			::strftime(임시텍스트버퍼, 배열크기(임시텍스트버퍼), "%Y%m%d", &t);

			char 스케줄파일명[_MAX_PATH] = { 0 };
			sprintf_s(스케줄파일명, "%s%s.ini", 설정포->스케줄저장소.c_str(), 임시텍스트버퍼);
			if (!dk::파일체크(스케줄파일명))
			{	// 스케줄 파일이 없으면 새로 생성한다 - 스케줄 생성
				디뷰("C_MAIN::timer200() - 스케줄 파일을 생성한다: %s", 스케줄파일명);
				스케줄생성(스케줄파일명);
			}

			if (기본타이머_일버퍼)
			{	// 처음이 아니면 날짜가 바뀐거다.


			}
			기본타이머_일버퍼 = currentTime.wDay;
		}
		//디뷰("C_MAIN::timer200(1초카운터) - %d시 %d분 nCurrentSecond: %d", currentTime.wHour, currentTime.wMinute, nCurrentSecond);
		if (!currentTime.wDayOfWeek || 6 == currentTime.wDayOfWeek)
		{	// 일요일과 토요일이면

		}
		else
		{	// 주중이면
			if (!(nCurrentSecond % (_SECS_PER_HOUR_ / 60)))
			{	// 여긴 1분에 한번씩 들어온다.
				//디뷰("C_MAIN::timer200(1분카운터) - %d시 %d분 nCurrentSecond: %d", currentTime.wHour, currentTime.wMinute, nCurrentSecond);
				if (currentTime.wDay != 기본타이머_일버퍼)
				{	// 날짜가 바뀌었는지 체크한다.
					char 임시텍스트버퍼[(1 << 4)] = { 0 };
					time_t tToday = ::time(0);
					tm t;
					::localtime_s(&t, &tToday);
					::strftime(임시텍스트버퍼, 배열크기(임시텍스트버퍼), "%Y%m%d", &t);

					char 스케줄파일명[_MAX_PATH] = { 0 };
					sprintf_s(스케줄파일명, "%s%s.ini", 설정포->스케줄저장소.c_str(), 임시텍스트버퍼);
					if (!dk::파일체크(스케줄파일명))
					{	// 스케줄 파일이 없으면 새로 생성한다 - 스케줄 생성
						디뷰("C_MAIN::timer200() - 스케줄 파일을 생성한다: %s", 스케줄파일명);
						스케줄생성(스케줄파일명);
					}

					if (기본타이머_일버퍼)
					{	// 처음이 아니면 날짜가 바뀐거다.


					}
					기본타이머_일버퍼 = currentTime.wDay;
				}
				else
				{	// 날짜가 변경되지 않았다면 스케줄을 진행한다.
					char 임시텍스트버퍼[(1 << 4)] = { 0 };
					time_t tToday = ::time(0);
					tm t;
					::localtime_s(&t, &tToday);
					::strftime(임시텍스트버퍼, 배열크기(임시텍스트버퍼), "%Y%m%d", &t);

					char 스케줄파일명[_MAX_PATH] = { 0 };
					sprintf_s(스케줄파일명, "%s%s.ini", 설정포->스케줄저장소.c_str(), 임시텍스트버퍼);
					if (!dk::파일체크(스케줄파일명))
					{	// 스케줄 파일이 없으면 새로 생성한다 - 스케줄 생성
						디뷰("C_MAIN::timer200() - 스케줄 파일을 생성한다: %s", 스케줄파일명);
						::WritePrivateProfileStringA("execute", "DCreonAPI", "0", 스케줄파일명);
						::WritePrivateProfileStringA("execute", "DOpenAPI", "0", 스케줄파일명);
						::WritePrivateProfileStringA("execute", "DXingAPI", "0", 스케줄파일명);
					}
					else
					{	// 스케줄 파일이 있으면 시간을 체크한다.
						if (_HM_TO_SEC_(7, 50) < nCurrentSecond && _HM_TO_SEC_(7, 53) > nCurrentSecond)
						{	// 주중 오전 07시 50분에서 7시 53분까지 관련 프로세스를 모두 종료하고 변수들을 초기화한다.
							디뷰("C_MAIN::timer200() - 모든 중계기를 종료한다");
							if (pBridgeCreon) { pBridgeCreon->SendPipe(_PKT_PIPE_DESTROY_); }
							if (pBridgeKiwoom) { pBridgeKiwoom->SendPipe(_PKT_PIPE_DESTROY_); }
							if (pBridgeEBest) { pBridgeEBest->SendPipe(_PKT_PIPE_DESTROY_); }
						}
						if (_H_TO_SEC_(8) < nCurrentSecond && _HM_TO_SEC_(8, 26) > nCurrentSecond)
						{

						}
						/*
						if (_H_TO_SEC_(17) < nCurrentSecond && _H_TO_SEC_(22) > nCurrentSecond)
						{	// 당일 데이터 업데이트
							//if (1 == 옵션_장후_업데이트_크레온)
							{
								int nResult = ::GetPrivateProfileIntA("update", "sticks", 0, 스케줄파일명);
								if (!nResult)
								{	// 업데이트가 완료되지 않은 상태이다.
									if (중계기_통신상태[_크레온_]) { 파이프서버[_크레온_]->Send(_PKT_PIPE_DESTROY_); }

									DWORD 프로세스아이디 = dk::GetProcessOf(pProcessInfo["DCreonAPI"]->파일이름);
									if (!프로세스아이디)
									{	// 업데이터가 실행중인 상태도 아니다.
										디뷰("C_MAIN::timer200() - 크레온 업데이터를 실행한다");
										::WritePrivateProfileStringA("base", "server", "2", pEngine->크레온설정파일.c_str());
										dk::LPPROCESS_INFOMATION_EX 프로세스 = pProcessInfo["DCreonAPI"];
										bool 성공여부 = dk::파일실행(프로세스->파일이름, 프로세스->파라미터, true, (LPPROCESS_INFORMATION)프로세스);
										if (!성공여부) { 디뷰("C_MAIN::timer200() - 크레온 업데이터 실행에 실패함"); }
										else { 디뷰("C_MAIN::timer200() - 크레온 업데이터 실행 완료"); }
									}
								}
							}
						}
						*/
					}
				}
			}

			if (!(nCurrentSecond % (_SECS_PER_HOUR_ / 12)))
			{	// 여긴 5분에 한번씩 들어온다.
				//디뷰("C_MAIN::timer200(5분카운터) - %d시 %d분 nCurrentSecond: %d", currentTime.wHour, currentTime.wMinute, nCurrentSecond);
			}

		}
		기본타이머_초버퍼 = currentTime.wSecond;
	}
}

bool C_MAIN::OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	// 조건에 다 맞는지 확인하는것보다. 하나라도 틀리면 리턴하는게 빠르다.
#if defined(_USE_LIB_IMGUI_)
	if (bShowWindow && C_IMGUI::WndProc_ImGui(_hWnd, _nMessage, _wParam, _lParam))
	{
		return(true);
	}
#endif
	switch (_nMessage)
	{
	case WM_SIZE:
		if (LOWORD(_lParam) != 설정포->메인윈도우크기.cx
			|| HIWORD(_lParam) != 설정포->메인윈도우크기.cy
			)
		{
			설정포->메인윈도우크기.cx = LOWORD(_lParam);
			설정포->메인윈도우크기.cy = HIWORD(_lParam);
#if defined(_USE_LIB_IMGUI_)
			C_IMGUI::SetWindowSize(설정포->메인윈도우크기.cx, 설정포->메인윈도우크기.cy);
			C_IMGUI::ResetDevice();
#endif
			bChangeSizeWindow = true;
		}
		return(true);
	case WM_CLOSE:
		::ShowWindow(hWnd, SW_HIDE);	// X 버튼을 누르면 윈도우를 숨기기만 한다.
		bShowWindow = false;
		return(true);
	case WM_QUERYENDSESSION:	// 윈도우 종료시 이 메시지가 온다고 한다
		pLog->Write("--- 윈도우 종료 메시지 받음 ---");
		bExitProcess = true;
		return(true);
	case WM_GETMINMAXINFO:	// 창이 아래의 크기 이하로 작아지지 않게 한다
		dk::GetWorkArea(&rcWorkSize);
		//LPMINMAXINFO lpmmi = (LPMINMAXINFO)_lParam;
		((LPMINMAXINFO)_lParam)->ptMaxSize.x = rcWorkSize.right;
		((LPMINMAXINFO)_lParam)->ptMaxSize.y = rcWorkSize.bottom;
		((LPMINMAXINFO)_lParam)->ptMaxPosition.x = 0;
		((LPMINMAXINFO)_lParam)->ptMaxPosition.y = 0;
		// 창이 아래의 크기 이하로 작아지지 않게 한다
		((LPMINMAXINFO)_lParam)->ptMinTrackSize.x = _MIN_MAIN_WINDOW_WIDTH_;	//GetSystemMetrics(SM_CXMINTRACK);	// 최소 크기 X 
		((LPMINMAXINFO)_lParam)->ptMinTrackSize.y = _MIN_MAIN_WINDOW_HEIGHT_;	// GetSystemMetrics(SM_CYMINTRACK);	// 최소 크기 Y
		((LPMINMAXINFO)_lParam)->ptMaxTrackSize.x = ::GetSystemMetrics(SM_CXMAXTRACK);	// 최대 크기 Width
		((LPMINMAXINFO)_lParam)->ptMaxTrackSize.y = ::GetSystemMetrics(SM_CYMAXTRACK);	// 최대 크기 Height
		return(true);
	case WM_COMMAND:
		switch (LOWORD(_wParam))
		{
		case WM_TRAY_CLOSE:
			bExitProcess = true;
			break;
		}
		return(true);
	case WM_TIMER:
		if (_기본타이머_ == _wParam)	// _기본타이머_
		{	// 200 짜리 타이머
			timer200();
		}
		return(true);
	case WM_MENUCHAR:
		MAKELRESULT(0, MNC_CLOSE);	//Alt-Enter를 눌렀을 때 삐 소리가 나지 않게 한다.
		return(true);
	case WM_ENTERSIZEMOVE:
		if (bChangeSizeWindow)
		{
			//bEnable = false;	// 모든 작업을 중지해야한다.
		}
		return(true);
	case WM_EXITSIZEMOVE:
		if (bChangeSizeWindow)
		{
			bChangeSizeWindow = false;
			//bEnable = true;
		}
		return(true);
	case WM_DROPFILES:
		do
		{
			퍼포먼스타이머[0].시작();
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
						DropFile(szDtopPath);
						//pEngine->ReadFile(szDtopPath);
					}
					else if (2 == nResult)
					{	// 폴더라면 해당 폴더안의 파일들을 얻어온다.
						디뷰("폴더네: %s", szDtopPath);
						strcat_s(szDtopPath, "/");
						std::vector<std::string> vFileList;
						dk::GetFileListDirectoryA(vFileList, szDtopPath);

						std::sort(vFileList.begin(), vFileList.end());

						size_t nCountAccrue = 0;
						퍼포먼스타이머[0].시작();
						for (size_t i = 0; i < vFileList.size(); i++)
						{
							std::string path = szDtopPath;
							path += vFileList[i];
							디뷰("파일이네: %s", path.c_str());
							nCountAccrue += DropFile(path.c_str());
							
							//pEngine->ReadFile(path.c_str());
							//디뷰("파일 처리에 걸린 시간: %0.6f", 퍼포먼스타이머[1].경과된시간());
						}
						디뷰("WM_DROPFILES() - 총 %d개 폴더처리에 걸린 시간: %0.6f", nCountAccrue, 퍼포먼스타이머[0].경과된시간());
					}
					else
					{
						//디뷰("파일도 폴더도 아님. 버근가 - %s", szDtopPath);
					}
				}
				::DragFinish((HDROP)_wParam);	// drag and drop 작업을 끝낸다.
				디뷰("드래그 처리 끝: %0.6f", 퍼포먼스타이머[0].경과된시간());
			}
		} while (false);
		break;
	case WM_TRAYICON:
		if (IDI_BETMAIN == _wParam)
		{
			if (WM_LBUTTONDBLCLK == LOWORD(_lParam))
			{	// 더블클릭 하면 윈도우를 보여준다.
				bShowWindow = true;
				::ShowWindow(hWnd, SW_SHOWNORMAL);
				::UpdateWindow(hWnd);
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
