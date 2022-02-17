#include "framework.h"
#include "CMain.h"



// [0]: 날짜, [1]: 시간, [2]~[5]: 시고저종, [8]: 거래량, [9]: 거래대금, [16]: 외국인 보유수량, [17]: 외국인보유비율
	// [18]: 수정주가일자, [19]: 수정주가비율, [20]: 기관순매수, [21]: 기관누적순매수, [25]: 주식회전율, [26]: 거래성립율
	//long nOption[] = { 0, 1, 2, 3, 4, 5, 8, 9, 16, 17, 18, 19, 20, 21, 25, 26 };
enum
{
	_날짜_ = 0							// [0]: 날짜
	, _시간_							// [1]: 시간
	, _시가_							// [2]: 시가
	, _고가_							// [3]: 고가
	, _저가_							// [4]: 저가
	, _종가_							// [5]: 종가(현재가)
	, _거래량_							// [6]: 거래량
	, _거래대금_						// [7]: 거래대금
	, _누적체결_매도수량_				// [8]: 누적 체결 매도 수량
	, _누적체결_매수수량_				// [9]: 누적 체결 매수 수량
	, _외국인_보유수량_					// [10]: 외국인 보유 수량
	, _외국인_보유비율_					// [11]: 외국인 보유 비율
	, _기관_순매수_						// [12]: 기관 순매수
	, _기관_누적매수_					// [13]: 기관 누적매수
	, _주식_회젼율_						// [14]: 주식 회전율
	, _거래_성립율_						// [15]: 거래 성립율
	, _MAX_ARRAY_SIZE_
};

C_MAIN::C_MAIN(HINSTANCE _hInst)
	: hInst(_hInst)
#if (_MSC_VER <= 1900)	// vs2017 이하
	, bEnable(true)
	, bVisible(false)
	, bChangeSizeWindow(false)
	, bExitProcess(false)
	, 대용량임시버퍼_시작포인터(nullptr)
	, 대용량임시버퍼_유동포인터(nullptr)

	, pCybos(nullptr)
	, pStockCode(nullptr)
	, pCodeMgr(nullptr)
	, pStockBid(nullptr)
	, pMarketEye(nullptr)
	, pStockChart(nullptr)
#endif
{
	if (!pLog)
	{
		pLog = new dk::C_LOG("DCreonAPI");
	}
#if _WIN32_WINNT >= 0x0400
	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
#else
	HRESULT hRes = ::CoInitialize(NULL);
#endif
	if (S_OK == hRes)
	{
	}
}

C_MAIN::~C_MAIN()
{
	::CoUninitialize();
}

bool C_MAIN::Init(LPCSTR _tszClassName, LPCSTR _tszWindowName, WNDPROC _pWndProc)
{
	pLog->Write("--- 로그 기록을 시작합니다 ---");

	char 임시버퍼[_MAX_PATH] = { 0 };
	dk::GetCurrentDirectoryA(임시버퍼);
	기본경로 = 임시버퍼;

	char 설정파일[_MAX_PATH] = { 0 };
	//::swprintf_s(설정파일, L"%sXeonPlus.ini", 기본경로);
	::GetModuleFileNameA(::GetModuleHandleW(0), 설정파일, 배열크기(설정파일));
	for (size_t i = 0; i < ::strlen(설정파일); i++) { if ('\\' == 설정파일[i]) { 설정파일[i] = '/'; } }
	설정파일[::strlen(설정파일) - 3] = 0;
	strcat_s(설정파일, "ini");
	디뷰("설정파일: %s", 설정파일);

	확장캔들사용 = true;
	테스트모드 = 업데이트분봉 = false;
	접속서버 = 2;
	if (!dk::파일체크(설정파일))
	{
		디뷰("C_MAIN::Init() - 설정파일이 없음: %s", 설정파일);
		sprintf_s(임시버퍼, "F:/data/StockSticks/");
		캔들저장소 = 임시버퍼;

		::WritePrivateProfileStringA("path", "sticks", 임시버퍼, 설정파일);
	}
	else
	{
		디뷰("C_MAIN::Init() - 설정파일: %s", 설정파일);
		접속서버 = (BYTE)::GetPrivateProfileIntA("base", "server", 2, 설정파일);

		테스트모드 = (bool)::GetPrivateProfileIntA("base", "test_mode", 0, 설정파일);
		확장캔들사용 = (bool)::GetPrivateProfileIntA("base", "use_extra_candle", 1, 설정파일);
		업데이트분봉 = (bool)::GetPrivateProfileIntA("update", "update_candle_minute", 0, 설정파일);
		::GetPrivateProfileStringA("path", "sticks", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		캔들저장소 = 임시버퍼;
		::GetPrivateProfileStringA("account", "creonplus_id", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		크레온계정 = 임시버퍼;
		::GetPrivateProfileStringA("account", "creonplus_pwd", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		크레온비밀번호 = 임시버퍼;
		::GetPrivateProfileStringA("account", "creonplus_certpwd", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		인증서비밀번호 = 임시버퍼;

		//디뷰("%s / %s / %s", 크레온계정.c_str(), 크레온비밀번호.c_str(), 인증서비밀번호.c_str());
		
	}
	if (테스트모드)
	{	
		접속서버 = 2;								// 업데이트를 강제로 활성화
		sprintf_s(임시버퍼, "F:/data/teststicks/");	// 테스트 경로로 박음
		캔들저장소 = 임시버퍼;
	}
	if (2 == 접속서버)
	{
		CMDOPEN("콘솔뷰 - 업데이트모드", CMD_YELLOW);
	}
	디뷰("캔들저장소: %s", 캔들저장소.c_str());
	디뷰("접속서버: %d", 접속서버);
	디뷰("테스트모드: %d", 테스트모드);

	WNDCLASSEXA wcex = { sizeof(WNDCLASSEXA), 0 };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = _pWndProc;
	wcex.hInstance = hInst;
	wcex.hCursor = ::LoadCursorA(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)::GetStockObject(GRAY_BRUSH);
	wcex.lpszClassName = _tszClassName;

	if (0 != ::RegisterClassExA(&wcex))
	{
		strClassName = _tszClassName;
		strWindowName = _tszWindowName;

		sizeWindow.Set(1280, 800);
		return(true);
	}
	return(false);
}

DWORD C_MAIN::GetWinFormStyle(bool _bWindowMode)
{
	DWORD dwWindowStyle = WS_OVERLAPPED;
	if (_bWindowMode)
	{
#if defined(LAYERED_WINDOW)
		// 캡션이 있을 경우 레이어 윈도우 사용시 ClientToScreen 함수가 이상 반응
		dwWindowStyle |= (WS_OVERLAPPED
			| WS_CLIPCHILDREN			// 이동시 깜박거림 없애기, 자식과 겹치는 부분을 부모가 그리지 않음.
			| WS_VISIBLE);
#else
		dwWindowStyle |=
			(WS_OVERLAPPED
				| WS_CAPTION				// 타이틀바
				| WS_SYSMENU				// X 버튼
				| WS_MINIMIZEBOX			// 최소화 버튼
				| WS_MAXIMIZEBOX			// 최대화 버튼
				| WS_THICKFRAME				// 크기조절 가능
				| WS_CLIPCHILDREN			// 이동시 깜박거림 없애기
				//| WS_VISIBLE
				);
#endif
	}
	else
	{
		dwWindowStyle |= WS_VISIBLE;
	}
	return dwWindowStyle;
}

bool C_MAIN::Create()
{
	bool bResult = false;
	do
	{
		DWORD dwWindowStyle = GetWinFormStyle(true);
		dk::DRECT rcTemp(ptWindow.x, ptWindow.y, sizeWindow.cx, sizeWindow.cy);
		::AdjustWindowRect(&rcTemp, dwWindowStyle, FALSE);
		dk::DPOINT ptWindowCenter(((::GetSystemMetrics(SM_CXSCREEN) - (int)sizeWindow.cx) / 2), ((::GetSystemMetrics(SM_CYSCREEN) - (int)sizeWindow.cy) / 2));
		hWnd = ::CreateWindowExA(
			WS_EX_ACCEPTFILES
			, strClassName.c_str()
			, strWindowName.c_str()
			, dwWindowStyle
			, ptWindowCenter.x
			, ptWindowCenter.y
			, rcTemp.right - rcTemp.left
			, rcTemp.bottom - rcTemp.top
			, nullptr
			, nullptr
			, hInst
			, this
		);
		if (!hWnd) { break; }
		pTrayIcon = new C_TRAY_ICON(hWnd, WM_TRAYICON, L"CreonAPI 1.0", LoadIconW(hInst, (LPCWSTR)IDI_DCREONAPI), IDI_DCREONAPI);
		if (!InitCreonAPI())
		{
			break;
		}
		// 파이프 생성.
#if defined(_TRACING_DEBUGVIEW_)
		디뷰("C_MAIN::Create() - new pipe::C_PIPE_CLIENT() ");
#endif
		if (2 > 접속서버)
		{	// 업데이트모드는 2이다. 
			pPipe = new pipe::C_PIPE_CLIENT(L"PipeClientCreon", L"PipeServerCreon");
			pPipe->ThreadStart();
#if defined(_TRACING_DEBUGVIEW_)
			디뷰("C_MAIN::Create() - pPipe->IsOnThread() 기다린다");
#endif
			do
			{	// 크레온 중계기 스레드 시작을 기다린다.
				dk::멈춰(1);
		} while (!pPipe->IsOnThread());
}
#if defined(_TRACING_DEBUGVIEW_)
		디뷰("C_MAIN::Create() - InitCreonAPI() ");
#endif
		// 일단 기록용인데, 이거 BetMain 으로 보내기만 하는 용도로 바꿔야한다.
		대용량임시버퍼_시작포인터 = (LPBYTE)힙할당((sizeof(CANDLE_DATA_TYPE2) * 10000000));	// 4천만건이면 960메가, 어차피 기록만 할거니까
		if (대용량임시버퍼_시작포인터)
		{
			디뷰("C_MAIN::Create() - 대용량임시버퍼 = %d", 힙크기(대용량임시버퍼_시작포인터));
			대용량임시버퍼_유동포인터 = 대용량임시버퍼_시작포인터;
		}
		if (2 > 접속서버)
		{
			bResult = true;
		}
		else
		{	// 업데이트모드는 여기만 진행하고 프로세스를 종료한다.
			if (!테스트모드)
			{
				퍼포먼스타이머[8].Start();
				GetStockList();
				디뷰("C_MAIN::Create() - 종목 분류 완료 %0.6f", 퍼포먼스타이머[8].TimeLeft());
			}
			else
			{
				디뷰("C_MAIN::Create() - 테스트모드: A000020, U001, U201, U180, U390");
				거른종목벡터.push_back({ 0, (LPCSTR)"A000020", (LPCSTR)"테스트", (LPCSTR)"" });
				//거른종목벡터.push_back({ 0, (LPCSTR)"U001", (LPCSTR)"코스피", (LPCSTR)"" });
				//거른종목벡터.push_back({ 0, (LPCSTR)"U201", (LPCSTR)"코스닥", (LPCSTR)"" });
				//거른종목벡터.push_back({ 0, (LPCSTR)"U180", (LPCSTR)"코스피200", (LPCSTR)"" });
				//거른종목벡터.push_back({ 0, (LPCSTR)"U390", (LPCSTR)"KQ150", (LPCSTR)"" });
			}
			퍼포먼스타이머[9].Start();
			캔들파일읽어서갱신();
			디뷰("C_MAIN::Create() - 모든 종목 업데이트 완료 %0.6f", 퍼포먼스타이머[9].TimeLeft());

			char 임시텍스트버퍼[(1 << 4)] = { 0 };
			time_t tToday = ::time(0);
			tm t;
			::localtime_s(&t, &tToday);
			::strftime(임시텍스트버퍼, 배열크기(임시텍스트버퍼), "%Y%m%d", &t);

			char 스케줄파일명[_MAX_PATH] = { 0 };
			sprintf_s(스케줄파일명, "F:/data/Schedule/%s.ini", 임시텍스트버퍼);
			::WritePrivateProfileStringA("update", "sticks", "1", 스케줄파일명);
		}
		
	} while (false);
#if defined(_TRACING_DEBUGVIEW_)
	디뷰("C_MAIN::Create() - ret: %d ", bResult);
#endif
	return(bResult);
}

bool C_MAIN::Calculate()
{
	long nResult = 0;
	do
	{
		if (bExitProcess) { nResult = 1; break; }
		if (2 > 접속서버)
		{	// 업데이트 모드가 아니라면 BetMain 에서 오더를 받는다.
			LPPACKET_BASE pPipePacket = nullptr;
			queueNetworkPackets.try_dequeue(pPipePacket);
			if (pPipePacket)
			{
				switch (pPipePacket->nPacketIndex)
				{
				case _PKT_STOCK_PACKET_INDEX_NONE_:
					디뷰("C_MAIN::Calculate() - 에러: _PKT_INDEX_NONE -  %d", pPipePacket->nPacketIndex);
					break;
				case _PKT_PIPE_CONNECTED_:
					디뷰("C_MAIN::Calculate() - BetMain에 접속 성공(_PKT_PIPE_CONNECTED): %s", pPipePacket->bytBuffer);
					pPipe->Send(_PKT_PIPE_CONNECTED_CREON_);
					break;
				case _PKT_BRIDGE_REQUEST_INIT_CREON_:
					디뷰("C_MAIN::Calculate() - 받음(_PKT_BRIDGE_REQUEST_INIT_CREON_)");
					do
					{
						char szBuffer[(1 << 4)];
						sprintf_s(szBuffer, "%04d", pCodeMgr->GetMarketStartTime());
						DWORD dwStartTime = _HM_TO_SEC_((((szBuffer[0] - 0x30) * 10) + ((szBuffer[1] - 0x30))), (((szBuffer[2] - 0x30) * 10) + ((szBuffer[3] - 0x30))));

						sprintf_s(szBuffer, "%04d", pCodeMgr->GetMarketEndTime());
						DWORD dwEndTime = _HM_TO_SEC_((((szBuffer[0] - 0x30) * 10) + ((szBuffer[1] - 0x30))), (((szBuffer[2] - 0x30) * 10) + ((szBuffer[3] - 0x30))));

						PACKET_BASE netPacket =
						{
							sizeof(DWORD) * 2					// 데이터크기는 에러코드 크기인 4
							, _PKT_BRIDGE_SUCCESSED_INIT_CREON_
							, { 0 }
						};
						*(LPDWORD)netPacket.bytBuffer = dwStartTime;
						*(LPDWORD)(netPacket.bytBuffer + sizeof(DWORD)) = dwEndTime;

						pPipe->Send(&netPacket);
						pLog->Write("C_MAIN::Calculate() - 시작시간: %d, 마감시간: %d", dwStartTime, dwEndTime);
						GetStockList();
					} while (false);
					break;
				case _PKT_PIPE_GET_STOCK_OHLCV_:
					디뷰("받음: _PKT_PIPE_GET_STOCK_OHLCV_");
					// 코스피
					퍼포먼스타이머[9].Start();
					캔들파일읽어서갱신();
					디뷰("모든 종목 업데이트 완료 %0.6f", 퍼포먼스타이머[9].TimeLeft());
					break;
				case _PKT_PIPE_DESTROY_:
					디뷰("받음(_PKT_PIPE_DESTROY_)");
					pLog->Write("C_MAIN::Calculate() - 받음(_PKT_PIPE_DESTROY_)");
					bExitProcess = true;
					break;
				default:
					pLog->Write("C_MAIN::Calculate() - 에러: default - %d", pPipePacket->nPacketIndex);
					break;
				}
			}
			dk::멈춰();
		}
		else
		{
			// 업데이트 모드라면 직접 루프를 돌아가면서 시간을 체크한다.
			// 이걸 쓰면 쓰면 GetLocalTime 을 쓰면 비교 1회 후 분기, 초가 바뀌고 밀리초 000~001 이면 들어온다.
			dk::DLOCAL_TIME currentTime;
			if (currentTime.wSecond != nCountSecond)
			{	// 여긴 초가 바뀔때마다 한번씩 들어온다.
				//ULONG nCurrentSecond = _HMS_TO_SEC_(currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
				if (!currentTime.wDayOfWeek || 6 == currentTime.wDayOfWeek)
				{	// 일요일과 토요일이면

				}
				else
				{
					//if (_HM_TO_SEC_(15, 40) < nCurrentSecond)
					//{	// 오전 8시 20분이 지났으면 종료
					//	디뷰("오전 8시 20분이 지나서 종료한다");
					//}
				}
				nCountSecond = currentTime.wSecond;
			}
			dk::멈춰();
		}
	} while (false);
	return(nResult);
}

void C_MAIN::Display()
{

}

void C_MAIN::Destroy()
{
	디뷰("C_MAIN::Destroy() - DHEAP_FREE(대용량임시버퍼_시작포인터)");
	DHEAP_FREE(대용량임시버퍼_시작포인터);
	if (2 > 접속서버)
	{
		디뷰("C_MAIN::Destroy() - DSAFE_DELETE(pPipe)");
		DSAFE_DELETE(pPipe);
	}
	디뷰("C_MAIN::Destroy() - DSAFE_DELETE(pTrayIcon)");
	DSAFE_DELETE(pTrayIcon);
	bExitProcess = true;
	pLog->Write("--- 로그 기록을 종료합니다 ---");
}

bool C_MAIN::OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	_hWnd;
	// 조건에 다 맞는지 확인하는것보다. 하나라도 틀리면 리턴하는게 빠르다.
	switch (_nMessage)
	{
	case WM_COMMAND:
		if (WM_TRAY_CLOSE == _wParam) {	bExitProcess = true; }
		return(true);
	case WM_TRAYICON:
		if (IDI_DCREONAPI == _wParam)
		{
			if (WM_RBUTTONUP == LOWORD(_lParam))
			{
				dk::DPOINT pt;
				::GetCursorPos(&pt);

				HMENU hMenu = ::CreatePopupMenu();

				//::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, WM_TRAY_CLOSE, "CreonAPI");
				//::InsertMenu(hMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, "LINE");
				::AppendMenuA(hMenu, MF_STRING, 0, "CreonAPI");
				::AppendMenuA(hMenu, MF_SEPARATOR, 0, "LINE");
				
				::AppendMenuA(hMenu, MF_STRING, WM_TRAY_CLOSE, "종료");

				::SetForegroundWindow(_hWnd);	// 생성된 팝업 메뉴 밖을 클릭할 때 팝업 닫기
				::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, _hWnd, NULL);
			}
		}
		return(true);
	}
	return(false);
}

bool C_MAIN::InitCreonAPI()
{
	bool bResult = false;
	do
	{
		디워드 크레온아이디 = dk::프로세스체크(L"CpStart.exe");
		디워드 스타터아이디 = dk::프로세스체크(L"coStarter.exe");
		if (!크레온아이디 && !스타터아이디)	// 크레온플러스를 실행해야하는거다.
		{
			char 파라미터[_MAX_PATH] = { 0 };
			sprintf_s(파라미터, " /prj:cp /id:%s /pwd:%s /pwdcert:%s /autostart", 크레온계정.c_str(), 크레온비밀번호.c_str(), 인증서비밀번호.c_str());
			if (1 == dk::파일체크("C:/CREON/STARTER/coStarter.exe"))
			{
				bool 성공여부 = dk::파일실행("C:/CREON/STARTER/coStarter.exe", 파라미터, true, nullptr);
				if (!성공여부)
				{
					디뷰("C_MAIN::InitCreonAPI() - 크레온플러스 실행에 실패함");
					break;
				}
			}
			else
			{
				디뷰("C_MAIN::InitCreonAPI() - 파일이 없음: C:/CREON/STARTER/coStarter.exe");
				break;
			}
			디워드 타임카운트 = 0, 시간버퍼 = 0;
			do
			{	// 스타터 종료 대기.
				dk::DLOCAL_TIME currentTime;
				if (currentTime.wSecond != 시간버퍼)
				{	// 
					타임카운트++;
					시간버퍼 = currentTime.wSecond;
				}
				if (타임카운트 > 300)
				{	// 
					디뷰("C_MAIN::InitCreonAPI() - 5분동안 실행이 안됨");
					디뷰("C_MAIN::InitCreonAPI() - 크레온플러스를 종료합니다.");
					dk::TerminateProcess(L"CpStart.exe");
					dk::TerminateProcess(L"DibServer.exe");
				}
				스타터아이디 = dk::프로세스체크(L"coStarter.exe");
				if (!스타터아이디)
				{
					크레온아이디 = dk::프로세스체크(L"CpStart.exe");
					if (크레온아이디) { break; }
				}
				dk::멈춰(200);
			} while (true);
			타임카운트 = 시간버퍼 = 0;
			do
			{	// 크레온플러스가 실행된거다.
				dk::DLOCAL_TIME currentTime;
				if (currentTime.wSecond != 시간버퍼)
				{	// 
					타임카운트++;
					시간버퍼 = currentTime.wSecond;
				}
				if (타임카운트 > 300)
				{	// 
					디뷰("C_MAIN::InitCreonAPI() - 5분동안 실행이 안됨");
					디뷰("C_MAIN::InitCreonAPI() - 크레온플러스를 종료합니다.");
					dk::TerminateProcess(L"CpStart.exe");
					dk::TerminateProcess(L"DibServer.exe");
				}
				HWND hFound = ::FindWindowW(L"Afx:00400000:0", L"공지사항");
				if (hFound)
				{	// 공지사항을 닫고 빠져나감.
					::PostMessageW(hFound, WM_CLOSE, 0, 0);
					break;
				}
				dk::멈춰(200);
			} while (true);
		}
		
		if (!pCybos)
		{
			//디뷰("CoCreateInstance(CpCybos)");
			HRESULT hr = ::CoCreateInstance(__uuidof(CPUTILLib::CpCybos), 0, CLSCTX_ALL, __uuidof(CPUTILLib::ICpCybos), (void**)&pCybos);
			if (FAILED(hr)) { pLog->Write("(CPUTILLib::CpCybos 생성 실패"); break; }

			if (!pCybos->GetIsConnect())
			{
				디뷰("C_MAIN::InitCreonAPI() - 크레온플러스를 종료합니다.");
				dk::TerminateProcess(L"CpStart.exe");
				dk::TerminateProcess(L"DibServer.exe");
				디뷰("C_MAIN::InitCreonAPI() - 크레온 중계기를 재실행합니다.");

				char 실행파일[_MAX_PATH] = { 0 }, 파라미터[_MAX_PATH] = { "restart" };
				::GetModuleFileNameA(::GetModuleHandleW(0), 실행파일, 배열크기(실행파일));
				dk::파일실행(실행파일, 파라미터, true, 0);

				break;
			}
		}
		if (!pStockCode)
		{
			//디뷰("CoCreateInstance(CpStockCode)");
			HRESULT hr = ::CoCreateInstance(__uuidof(CPUTILLib::CpStockCode), 0, CLSCTX_ALL, __uuidof(CPUTILLib::ICpStockCode), (void**)&pStockCode);
			if (FAILED(hr)) { pLog->Write("(CPUTILLib::CpStockCode 생성 실패"); break; }
		}
		if (!pCodeMgr)
		{
			//디뷰("CoCreateInstance(pCodeMgr)");
			HRESULT hr = ::CoCreateInstance(__uuidof(CPUTILLib::CpCodeMgr), 0, CLSCTX_ALL, __uuidof(CPUTILLib::ICpCodeMgr), (void**)&pCodeMgr);
			if (FAILED(hr)) { pLog->Write("(CPUTILLib::pCodeMgr 생성 실패"); break; }
		}
		if (!pStockBid)
		{
			//디뷰("CoCreateInstance(StockBid)");
			HRESULT hr = ::CoCreateInstance(__uuidof(DSCBO1Lib::StockBid), 0, CLSCTX_ALL, __uuidof(DSCBO1Lib::IDib), (void**)&pStockBid);
			if (FAILED(hr)) { pLog->Write("(DSCBO1Lib::StockBid 생성 실패"); break; }
		}
		if (!pMarketEye)
		{
			//디뷰("CoCreateInstance(MarketEye)");
			HRESULT hr = ::CoCreateInstance(__uuidof(CPSYSDIBLib::MarketEye), 0, CLSCTX_ALL, __uuidof(CPSYSDIBLib::ISysDib), (void**)&pMarketEye);
			if (FAILED(hr)) { pLog->Write("(CPSYSDIBLib::MarketEye 생성 실패"); break; }
		}
		if (!pStockChart)
		{
			//디뷰("CoCreateInstance(pStockChart)");
			HRESULT hr = ::CoCreateInstance(__uuidof(CPSYSDIBLib::StockChart), 0, CLSCTX_ALL, __uuidof(CPSYSDIBLib::ISysDib), (void**)&pStockChart);
			if (FAILED(hr)) { pLog->Write("(CPSYSDIBLib::pStockChart 생성 실패"); break; }
		}
		bResult = true;
	} while (false);
	return(bResult);
}

void C_MAIN::GetStockList()
{
	/*
	typedef enum
	{
	  [helpstring("구분없음")] CPC_KSE_SECTION_KIND_NULL = 0,
	  [helpstring("주권")]   CPC_KSE_SECTION_KIND_ST = 1,
	  [helpstring("투자회사")]   CPC_KSE_SECTION_KIND_MF = 2,
	  [helpstring("부동산투자회사"]   CPC_KSE_SECTION_KIND_RT = 3,
	  [helpstring("선박투자회사")]   CPC_KSE_SECTION_KIND_SC = 4,
	  [helpstring("사회간접자본투융자회사")]CPC_KSE_SECTION_KIND_IF = 5,
	  [helpstring("주식예탁증서")]   CPC_KSE_SECTION_KIND_DR = 6,
	  [helpstring("신수인수권증권")]   CPC_KSE_SECTION_KIND_SW = 7,
	  [helpstring("신주인수권증서")]   CPC_KSE_SECTION_KIND_SR = 8,
	  [helpstring("주식워런트증권")]   CPC_KSE_SECTION_KIND_ELW = 9,
	  [helpstring("상장지수펀드(ETF)")]CPC_KSE_SECTION_KIND_ETF = 10,
	  [helpstring("수익증권")]    CPC_KSE_SECTION_KIND_BC = 11,
	  [helpstring("해외ETF")]      CPC_KSE_SECTION_KIND_FETF = 12,
	  [helpstring("외국주권")]    CPC_KSE_SECTION_KIND_FOREIGN = 13,
	  [helpstring("선물")]      CPC_KSE_SECTION_KIND_FU = 14,
	  [helpstring("옵션")]      CPC_KSE_SECTION_KIND_OP = 15,
	  [helpstring("KONEX")]      CPC_KSE_SECTION_KIND_KN = 16,
	  [helpstring("ETN")]      CPC_KSE_SECTION_KIND_ETN = 17,
	} CPE_KSE_SECTION_KIND;
	*/
	vKseSectionKind.clear();
	vKseSectionKind.push_back("구분없음");
	vKseSectionKind.push_back("주권");
	vKseSectionKind.push_back("투자회사");
	vKseSectionKind.push_back("부동산투자회사");
	vKseSectionKind.push_back("선박투자회사");
	vKseSectionKind.push_back("사회간접자본투융자회사");
	vKseSectionKind.push_back("주식예탁증서");
	vKseSectionKind.push_back("신수인수권증권");
	vKseSectionKind.push_back("신주인수권증서");
	vKseSectionKind.push_back("주식워런트증권(ELW)");
	vKseSectionKind.push_back("상장지수펀드(ETF)");
	vKseSectionKind.push_back("수익증권");
	vKseSectionKind.push_back("해외ETF");
	vKseSectionKind.push_back("외국주권");
	vKseSectionKind.push_back("선물");
	vKseSectionKind.push_back("옵션");
	vKseSectionKind.push_back("KONEX");
	vKseSectionKind.push_back("ETN");

	/*
	typedef enum
	{
		[helpstring("구분없음")] CPC_MARKET_NULL = 0,
		[helpstring("거래소")]   CPC_MARKET_KOSPI = 1,
		[helpstring("코스닥")]   CPC_MARKET_KOSDAQ = 2,
		[helpstring("K-OTC")] CPC_MARKET_FREEBOARD = 3,
		[helpstring("KRX")]       CPC_MARKET_KRX = 4,
		[helpstring("KONEX")] CPC_MARKET_KONEX = 5,
	} CPE_MARKET_KIND;
	*/
	vMarketType.clear();
	vMarketType.push_back("구분없음");
	vMarketType.push_back("거래소");
	vMarketType.push_back("코스닥");
	vMarketType.push_back("K-OTC");
	vMarketType.push_back("KRX");
	vMarketType.push_back("KONEX");

	/*
	typedef enum
	{
		[helpstring("정상")] CPC_CONTROL_NONE = 0,
		[helpstring("주의")]   CPC_CONTROL_ATTENTION = 1,
		[helpstring("경고")]   CPC_CONTROL_WARNING = 2,
		[helpstring("위험예고")]CPC_CONTROL_DANGER_NOTICE = 3,
		[helpstring("위험")]   CPC_CONTROL_DANGER = 4,
		[helpstring("경고예고")]   CPC_CONTROL_WARNING_NOTICE = 5,
	}CPE_CONTROL_KIND;
	*/
	vControlType.clear();
	vControlType.push_back("정상");
	vControlType.push_back("주의");
	vControlType.push_back("경고");
	vControlType.push_back("위험예고");
	vControlType.push_back("위험");
	vControlType.push_back("경고예고");
	/*
	typedefenum
	{
		[helpstring("일반종목")] CPC_SUPERVISION_NONE = 0,
		[helpstring("관리")]   CPC_SUPERVISION_NORMAL = 1,
	}CPE_SUPERVISION_KIND;
	*/
	vSupervisionKind.clear();
	vSupervisionKind.push_back("일반종목");
	vSupervisionKind.push_back("관리");

	/*
	typedef enum
	{
		[helpstring("정상")]    CPC_STOCK_STATUS_NORMAL = 0,
		[helpstring("거래정지")] CPC_STOCK_STATUS_STOP = 1,
		[helpstring("거래중단")] CPC_STOCK_STATUS_BREAK = 2,
	}CPE_STOCK_STATUS_KIND;
	*/
	vStockStatusKind.clear();
	vStockStatusKind.push_back("정상");
	vStockStatusKind.push_back("거래정지");
	vStockStatusKind.push_back("거래중단");

	// 요청 필드
	// rqField = [코드, 대비부호, 대비, 현재가, 시가, 고가, 저가, 매도호가, 매수호가, 거래량, 장구분, 매도잔량, 매수잔량, 공매도수량, 공매도날짜]
	// rqField = [0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 15, 16, 127, 128]  # 요청 필드
	// long fieldList[] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 15, 16 };
	// pMarketEye->SetInputValue();

	// 모든 종목 총 갯수
	short nStockCount = pStockCode->GetCount();
	디뷰("GetIsConnect(): %d / 종목 총 갯수 = %d", pCybos->GetIsConnect(), nStockCount);
	for (short i = 0; i < nStockCount; i++)
	{
		모든종목벡터.push_back(
			{
				0
				, (LPCSTR)((_bstr_t)pStockCode->GetData(0, i))	// 코드
				, (LPCSTR)((_bstr_t)pStockCode->GetData(1, i))	// 종목명
				, (LPCSTR)((_bstr_t)pStockCode->GetData(2, i))	// 풀코드
			}
		);
		//디뷰("[%s] %s", 모든종목벡터[i].strCode.c_str(), 모든종목벡터[i].strName.c_str());
		/*
		CPUTILLib::CPE_KSE_SECTION_KIND kseSectionType = pCodeMgr->GetStockSectionKind(모든종목벡터[i].strCode.c_str());		// ETF, ELW 등의 종류
		CPUTILLib::CPE_MARKET_KIND marketType = pCodeMgr->GetStockMarketKind(모든종목벡터[i].strCode.c_str());					// 거래소, 코스닥 등의 종류
		CPUTILLib::CPE_CONTROL_KIND controlType = pCodeMgr->GetStockControlKind(모든종목벡터[i].strCode.c_str());				// 위험, 경고 등의 종류
		CPUTILLib::CPE_SUPERVISION_KIND supervisionType = pCodeMgr->GetStockSupervisionKind(모든종목벡터[i].strCode.c_str());	// 관리 종목 구분
		CPUTILLib::CPE_STOCK_STATUS_KIND stockStatus = pCodeMgr->GetStockStatusKind(모든종목벡터[i].strCode.c_str());			// 거래 정지 구분
		long 스팩주 = pCodeMgr->IsSPAC(모든종목벡터[i].strCode.c_str());														// 스팩주 구분
		long 전일종가 = pCodeMgr->GetStockYdClosePrice(모든종목벡터[i].strCode.c_str());										// 전일종가
		*/
		// 여기에서 모든 종목의 구조체를 완성해서 보낸다.
		PACKET_BASE netPacket =
		{
			sizeof(STOCK_INFO_CREON)				// 데이터크기
			, _PKT_BRIDGE_RECEIVE_STOCK_INFO_CREON_			// 헤더
			, { 0 }									// 보낼 내용
		};
		LPSTOCK_INFO_CREON pStockInfo = (LPSTOCK_INFO_CREON)&netPacket.bytBuffer[0];
		pStockInfo->장구분 = (BYTE)pCodeMgr->GetStockMarketKind(모든종목벡터[i].strCode.c_str());				// 거래소, 코스닥 등의 종류
		pStockInfo->nKseSectionKind = (BYTE)pCodeMgr->GetStockSectionKind(모든종목벡터[i].strCode.c_str());			// ETF, ELW 등의 종류
		pStockInfo->상태 = (BYTE)pCodeMgr->GetStockStatusKind(모든종목벡터[i].strCode.c_str());						// 거래 정지 구분
		pStockInfo->nSupervisionType = (BYTE)pCodeMgr->GetStockSupervisionKind(모든종목벡터[i].strCode.c_str());	// 관리 종목 구분
		pStockInfo->nControlType = (BYTE)pCodeMgr->GetStockControlKind(모든종목벡터[i].strCode.c_str());			// 위험, 경고 등의 종류
		pStockInfo->스팩주 = (BYTE)pCodeMgr->IsSPAC(모든종목벡터[i].strCode.c_str());								// 스팩주 구분
		pStockInfo->전일종가 = pCodeMgr->GetStockYdClosePrice(모든종목벡터[i].strCode.c_str());						// 전일종가
		strcpy_s(pStockInfo->크레온코드, (모든종목벡터[i].strCode.c_str()));
		strcpy_s(pStockInfo->종목코드, &pStockInfo->크레온코드[1]);
		strcpy_s(pStockInfo->풀코드, (모든종목벡터[i].strFullCode.c_str()));
		strcpy_s(pStockInfo->종목명, (모든종목벡터[i].strName.c_str()));
		if (2 > 접속서버)
		{
			pPipe->Send(&netPacket);		// 일단 모든 종목을 보내고, 거래할 종목은 서버에서 판단한다.
		}
		do
		{	// 이 아래는 현물만 들어간다.
			if ((CPUTILLib::CPC_MARKET_KOSPI == pStockInfo->장구분 || CPUTILLib::CPC_MARKET_KOSDAQ == pStockInfo->장구분)
				&& CPUTILLib::CPC_KSE_SECTION_KIND_ST == pStockInfo->nKseSectionKind
				&& 0 == pStockInfo->상태			// 거래중지나 거래중단을 거르고
				&& 0 == pStockInfo->nSupervisionType		// 관리종목 거르고
				&& 0 == pStockInfo->nControlType			// 정상 외 거르고
				&& 0 == pStockInfo->스팩주					// 스팩 거르고
				&& 0 < pStockInfo->전일종가				// 전일종가 없는건 아예 없는 종목.
				)	// 이러면 종목이 3324개에서 2237개정도 나옴.
			{
				//if (9 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
				//if (10 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
				//if (12 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
				//if (14 < pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
				if (0 == pStockInfo->전일종가) { break; }					// 전일종가 없는건 아예 없는 종목. ( 사라진 ELW 등 )

				거른종목벡터.push_back(모든종목벡터[i]);
			}
			else
			{
				//if (!모든종목벡터[i].strCode.compare("U001"))
				//{
				//	디뷰("찾았는데?");
				//}
				if (!모든종목벡터[i].strName.compare("KODEX 레버리지")
					|| !모든종목벡터[i].strName.compare("KODEX 인버스")
					|| !모든종목벡터[i].strName.compare("KODEX 코스닥 150")
					|| !모든종목벡터[i].strName.compare("KODEX 코스닥150 레버리지")
					|| !모든종목벡터[i].strName.compare("KODEX 코스닥150선물인버스")
					|| !모든종목벡터[i].strName.compare("KODEX 200")
					|| !모든종목벡터[i].strName.compare("KODEX 200선물인버스2X")
					|| !모든종목벡터[i].strName.compare("KODEX 코스피")
					|| !모든종목벡터[i].strName.compare("KODEX 코스피100")
					|| !모든종목벡터[i].strName.compare("KODEX 골드선물(H)")
					|| !모든종목벡터[i].strName.compare("KODEX 골드선물인버스(H)")
					)
				{
					거른종목벡터.push_back(모든종목벡터[i]);
				}
			}
		} while (false);
	}
	거른종목벡터.push_back({ 0, (LPCSTR)"U001", (LPCSTR)"코스피", (LPCSTR)"" });
	거른종목벡터.push_back({ 0, (LPCSTR)"U201", (LPCSTR)"코스닥", (LPCSTR)"" });
	거른종목벡터.push_back({ 0, (LPCSTR)"U180", (LPCSTR)"코스피200", (LPCSTR)"" });
	거른종목벡터.push_back({ 0, (LPCSTR)"U390", (LPCSTR)"KQ150", (LPCSTR)"" });

	//거른종목벡터.push_back({ 0, (LPCSTR)"U530", (LPCSTR)"MSCI Korea Index", (LPCSTR)"" });
	//거른종목벡터.push_back({ 0, (LPCSTR)"A261220", (LPCSTR)"KODEX WTI 원유선물", (LPCSTR)"" });
	//거른종목벡터.push_back({ 0, (LPCSTR)"A219480", (LPCSTR)"미국S&P500", (LPCSTR)"" });
	//거른종목벡터.push_back({ 0, (LPCSTR)"A304940", (LPCSTR)"미국나스닥100선물", (LPCSTR)"" });
	디뷰("총 종목수: %d / 정지 중단 거르고 남은 현물: %d", nStockCount, 거른종목벡터.size());
	//for (사이즈 인덱스 = 0; 인덱스 < 거른종목벡터.size(); 인덱스++)
	//{
	//	디뷰("[%s] %s - %s", 거른종목벡터[인덱스].strCode.c_str(), 거른종목벡터[인덱스].strFullCode.c_str(), 거른종목벡터[인덱스].strName.c_str());
	//}
	// 종목 다 보냈다고 서버에 알린다.
	if (2 > 접속서버)
	{
		pPipe->Send(_PKT_BRIDGE_SUCCEEDED_STOCK_INFO_CREON_);
	}
	//디뷰("발송(_PKT_PIPE_INIT_FINISH_CREON): %d", _PKT_BRIDGE_SUCCEEDED_STOCK_INFO_CREON_);
}

LPSTR C_MAIN::캔들경로세팅(LPSTR _경로포인터, size_t _경로버퍼크기, BYTE _캔들의종류, WORD _캔들의길이)
{
	// 여기는 종목 추리는 함수다
	dk::초기화(_경로포인터, _경로버퍼크기);
	strcpy_s(_경로포인터, _경로버퍼크기, 캔들저장소.c_str());
	if (!::PathIsDirectoryA(_경로포인터)) { ::CreateDirectoryA(_경로포인터, NULL); }	// StockSticks 폴더가 없다면 생성.
	char 임시버퍼[(1 << 5)] = { 0 };
	if (0 == _캔들의종류)
	{	// 분봉, StockSticks/minute1/, StockSticks/minute15/
		sprintf_s(임시버퍼, "minute%d/", _캔들의길이);
	}
	else if (1 == _캔들의종류)
	{	// StockSticks/day/
		sprintf_s(임시버퍼, "day%d/", _캔들의길이);
	}
	else if (2 == _캔들의종류)
	{	// StockSticks/week/
		sprintf_s(임시버퍼, "week%d/", _캔들의길이);
	}
	else if (3 == _캔들의종류)
	{	// StockSticks/month/
		sprintf_s(임시버퍼, "month%d/", _캔들의길이);
	}
	else if (4 == _캔들의종류)
	{	// StockSticks/month/
		sprintf_s(임시버퍼, "ticks/");
	}
	strcat_s(_경로포인터, _경로버퍼크기, 임시버퍼);
	// 폴더가 없으면 생성한다.
	if (!::PathIsDirectoryA(_경로포인터)) { ::CreateDirectoryA(_경로포인터, NULL); }
	return(_경로포인터);
}

void C_MAIN::캔들요청세팅(LPCSTR _종목코드, BYTE _캔들의종류, WORD _캔들의길이)
{
	//LPSAFEARRAYBOUND pSafeArrayBound = new SAFEARRAYBOUND();
	//pSafeArrayBound->cElements = 16;	// 배열의 항목 개수는 16개
	//pSafeArrayBound->lLbound = 0;		// 인덱스는 0부터
	tagSAFEARRAYBOUND safeArrayBound = { _MAX_ARRAY_SIZE_, 0 };
	// 배열 생성.
	LPSAFEARRAY pSafeArray = ::SafeArrayCreate(VT_I4, 1, &safeArrayBound);	// VT_I4, SAFEARRAYBOUND는 1개, SAFEARRAYBOUND의 주소.
	// 배열 세팅.
	PLONG pElem = nullptr;
	if (NO_ERROR == ::SafeArrayAccessData(pSafeArray, (LPVOID*)&pElem))
	{
		pElem[_날짜_] = 0;								// [0]: 날짜
		pElem[_시간_] = 1;								// [1]: 시간
		pElem[_시가_] = 2;								// [2]: 시가
		pElem[_고가_] = 3;								// [3]: 고가
		pElem[_저가_] = 4;								// [4]: 저가
		pElem[_종가_] = 5;								// [5]: 종가(현재가)
		pElem[_거래량_] = 8;							// [6]: 거래량
		pElem[_거래대금_] = 9;							// [7]: 거래대금
		pElem[_누적체결_매도수량_] = 10;				// [8]: 호가비교방식 누적체결매도수량 (분 틱 시에만 제공)
		pElem[_누적체결_매수수량_] = 11;				// [9]: 호가비교방식 누적체결매수수량 (분 틱 시에만 제공)
		pElem[_외국인_보유수량_] = 16;					// [10]: 외국인 보유수량
		pElem[_외국인_보유비율_] = 17;					// [11]: 외국인 보유비율
		//pElem[10] = 18;	// 수정주가일자
		//pElem[11] = 19;	// 수정주가비율
		pElem[_기관_순매수_] = 20;						// [12]: 기관순매수
		pElem[_기관_누적매수_] = 21;					// [13]: 기관누적매수
		pElem[_주식_회젼율_] = 25;						// [14]: 주식회전율
		pElem[_거래_성립율_] = 26;						// [15]: 거래성립율
		::SafeArrayUnaccessData(pSafeArray);
	}
	_variant_t vt;
	vt.parray = pSafeArray;
	vt.vt = VT_ARRAY | VT_I4;

	char 캔들의종류 = 'm';	// 분봉
	if (1 == _캔들의종류) { 캔들의종류 = 'D'; }		// 일봉
	if (2 == _캔들의종류) { 캔들의종류 = 'W'; }		// 주봉
	if (3 == _캔들의종류) { 캔들의종류 = 'M'; }		// 월봉
	if (4 == _캔들의종류) { 캔들의종류 = 'T'; }		// 틱

	pStockChart->SetInputValue(0, _종목코드);									// 종목코드, "U001" 코스피, "U201" 코스닥
	pStockChart->SetInputValue(1, (char)'2');									// [2]: 갯수로 요청
	pStockChart->SetInputValue(4, (ULONG)2000);									// 최근 390개의 봉	(한번 요청마다 얻는 갯수가 1999 에서 1249개로 줄음)
	pStockChart->SetInputValue(7, (USHORT)_캔들의길이);							// [1]: 1분봉, [15]: 15분봉
	//디뷰("[%d/%d] 갯수로 요청 %c", nRemainRequestCount, pCybos->LimitRequestRemainTime, cRequestType);
	pStockChart->SetInputValue(5, vt);											// 요청항목
	pStockChart->SetInputValue(6, 캔들의종류);									// D 일봉, W 주봉, M 월봉, m 분봉, T 틱
	// [1]: 시간외거래량포함, [2]: 장종료시간외거래량만포함, [3]: 시간외거래량모두제외, [4]: 장전시간외거래량만포함
	pStockChart->SetInputValue(9, '1');											// [0]: 수정주가 적용 안함, [1]: 수정주가 적용
	pStockChart->SetInputValue(10, '3');
}

long C_MAIN::캔들받아서리턴(std::vector<확장캔들포>& _리턴벡터, LPCSTR _종목코드, BYTE _캔들의종류, WORD _캔들의길이, bool _새로요청)
{
	tagSAFEARRAYBOUND safeArrayBound = { _MAX_ARRAY_SIZE_, 0 };
	// 배열 생성.
	LPSAFEARRAY pSafeArray = ::SafeArrayCreate(VT_I4, 1, &safeArrayBound);	// VT_I4, SAFEARRAYBOUND는 1개, SAFEARRAYBOUND의 주소.
	// 배열 세팅.
	PLONG pElem = nullptr;
	if (NO_ERROR == ::SafeArrayAccessData(pSafeArray, (LPVOID*)&pElem))
	{
		pElem[_날짜_] = 0;								// [0]: 날짜
		pElem[_시간_] = 1;								// [1]: 시간
		pElem[_시가_] = 2;								// [2]: 시가
		pElem[_고가_] = 3;								// [3]: 고가
		pElem[_저가_] = 4;								// [4]: 저가
		pElem[_종가_] = 5;								// [5]: 종가(현재가)
		pElem[_거래량_] = 8;							// [6]: 거래량
		pElem[_거래대금_] = 9;							// [7]: 거래대금
		pElem[_누적체결_매도수량_] = 10;				// [8]: 호가비교방식 누적체결매도수량 (분 틱 시에만 제공)
		pElem[_누적체결_매수수량_] = 11;				// [9]: 호가비교방식 누적체결매수수량 (분 틱 시에만 제공)
		pElem[_외국인_보유수량_] = 16;					// [10]: 외국인 보유수량
		pElem[_외국인_보유비율_] = 17;					// [11]: 외국인 보유비율
		//pElem[10] = 18;	// 수정주가일자
		//pElem[11] = 19;	// 수정주가비율
		pElem[_기관_순매수_] = 20;						// [12]: 기관순매수
		pElem[_기관_누적매수_] = 21;					// [13]: 기관누적매수
		pElem[_주식_회젼율_] = 25;						// [14]: 주식회전율
		pElem[_거래_성립율_] = 26;						// [15]: 거래성립율
		::SafeArrayUnaccessData(pSafeArray);
	}
	_variant_t vt;
	vt.parray = pSafeArray;
	vt.vt = VT_ARRAY | VT_I4;

	char 캔들의종류 = 'm';	// 분봉
	if (1 == _캔들의종류) { 캔들의종류 = 'D'; }		// 일봉
	if (2 == _캔들의종류) { 캔들의종류 = 'W'; }		// 주봉
	if (3 == _캔들의종류) { 캔들의종류 = 'M'; }		// 월봉
	if (4 == _캔들의종류) { 캔들의종류 = 'T'; }		// 틱

	long 남은요청가능횟수 = pCybos->GetLimitRemainCount(CPUTILLib::LIMIT_TYPE::LT_NONTRADE_REQUEST);
	if (0 == 남은요청가능횟수)
	{	// 처음부터 없는 상태라면 15초 기다린다.
		디뷰("기다린다: %d / %d", 남은요청가능횟수, pCybos->LimitRequestRemainTime);
		dk::Sleep(pCybos->LimitRequestRemainTime + 10);
	}
	남은요청가능횟수 = pCybos->GetLimitRemainCount(CPUTILLib::LIMIT_TYPE::LT_NONTRADE_REQUEST);
	if (0 < 남은요청가능횟수 || 15000 == pCybos->GetLimitRequestRemainTime())
	{	// 요청할 수 있는 횟수가 남아있거나, 잔여 시간이 15초 남았으면
		if (_새로요청)
		{	// 첫 요청이면 요청사항을 세팅한다.
			pStockChart->SetInputValue(0, _종목코드);									// 종목코드, "U001" 코스피, "U201" 코스닥
			pStockChart->SetInputValue(1, (char)'2');									// [1]: 기간으로 요청, [2]: 갯수로 요청 - 기간 요청시 주,월,분,틱은 불가
			//pStockChart->SetInputValue(2, );											// 종료일
			//pStockChart->SetInputValue(3, );											// 시작일
			pStockChart->SetInputValue(4, (ULONG)99999999);								// 최근 390개의 봉
			pStockChart->SetInputValue(7, (USHORT)_캔들의길이);							// [1]: 1분봉, [15]: 15분봉
			//디뷰("[%d/%d] 파일경로: %s / 갯수로 요청 %c", nRemainRequestCount, pCybos->LimitRequestRemainTime, szSavePath, cRequestType);
			pStockChart->SetInputValue(5, vt);											// 요청항목
			pStockChart->SetInputValue(6, 캔들의종류);								// D 일봉, W 주봉, M 월봉, m 분봉, T 틱
			// [1]: 시간외거래량포함, [2]: 장종료시간외거래량만포함, [3]: 시간외거래량모두제외, [4]: 장전시간외거래량만포함
			pStockChart->SetInputValue(9, '1');											// [0]: 수정주가 적용 안함, [1]: 수정주가 적용
			pStockChart->SetInputValue(10, '3');
		}
		// 요청한다. SetInputValue 를 진행하지 않았다면 연속 조회. ( 이거 한번에 1씩 소모한다. )
		pStockChart->BlockRequest();
		// 수신개수
		long 수신갯수 = pStockChart->GetHeaderValue(3);
		if (0 < 수신갯수)
		{	// 수신을 받았으면,
			for (long 캔들인덱스 = 0; 캔들인덱스 < 수신갯수; 캔들인덱스++)
			{
				LPBET_STICK_EX 생성된캔들포 = new BET_STICK_EX();
				생성된캔들포->초기화();

				생성된캔들포->nDate = pStockChart->GetDataValue(_날짜_, 캔들인덱스);
				
				if (생성된캔들포)
				{	// 지수는 일자, 시간, 시고저종만 기록한다.
					생성된캔들포->nTime = pStockChart->GetDataValue(_시간_, 캔들인덱스);										// 시간
					생성된캔들포->fOpen = (float)pStockChart->GetDataValue(_시가_, 캔들인덱스);									// 시가
					생성된캔들포->fHigh = (float)pStockChart->GetDataValue(_고가_, 캔들인덱스);									// 고가
					생성된캔들포->fLow = (float)pStockChart->GetDataValue(_저가_, 캔들인덱스);									// 저가
					생성된캔들포->fClose = (float)pStockChart->GetDataValue(_종가_, 캔들인덱스);								// 종가(현재가)
					if (4 < strlen(_종목코드))	// U001, U180, U201, U380
					{	// 지수는 거래량이 없다.
						생성된캔들포->nTotalVolume = pStockChart->GetDataValue(_거래량_, 캔들인덱스);							// 거래량
						생성된캔들포->nTransferPayment = pStockChart->GetDataValue(_거래대금_, 캔들인덱스);						// 거래대금
						if (0 == _캔들의종류)
						{	// 분봉, 틱봉이면 누적체결 수량도 기록한다.
							생성된캔들포->누적매도체결량 = pStockChart->GetDataValue(_누적체결_매도수량_, 캔들인덱스);		// 누적체결매도수량
							생성된캔들포->누적매수체결량 = pStockChart->GetDataValue(_누적체결_매수수량_, 캔들인덱스);		// 누적체결매수수량
						}
					}
					_리턴벡터.push_back(생성된캔들포);	// 배열에 매우 담는다
				}
			}
		}
		return(pStockChart->Continue);	// 더 조회할 것이 남았다면 0 이 아닌 값을 리턴한다.
	}
	return(0);
}

void C_MAIN::요청가능확인()
{
	do
	{
		long 남은요청가능횟수 = pCybos->GetLimitRemainCount(CPUTILLib::LIMIT_TYPE::LT_NONTRADE_REQUEST);
		if (0 == 남은요청가능횟수)
		{	// 처음부터 없는 상태라면 15초 기다린다.
			if (2 > 접속서버)
			{
				디뷰("기다린다: %d / %d", 남은요청가능횟수, pCybos->LimitRequestRemainTime);
			}
			else
			{
				CMDPRINT("기다린다: %d / %d", 남은요청가능횟수, pCybos->LimitRequestRemainTime);
			}
			
			dk::Sleep(pCybos->LimitRequestRemainTime + 10);
		}
		남은요청가능횟수 = pCybos->GetLimitRemainCount(CPUTILLib::LIMIT_TYPE::LT_NONTRADE_REQUEST);
		if (0 < 남은요청가능횟수 || 15000 == pCybos->GetLimitRequestRemainTime())
		{	// 요청할 수 있는 횟수가 남아있거나, 잔여 시간이 15초 남았으면
			break;
		}
	} while (true);
}

void C_MAIN::종목갱신(LPCSTR _종목코드, 바이트 _캔들종류, 워드 _캔들길이)
{
	std::vector<확장캔들포> 읽은캔들보관벡터;
	std::vector<확장캔들포> 받은캔들포벡터;

	char 캔들파일경로[_MAX_PATH] = { 0 };
	캔들경로세팅(캔들파일경로, 배열크기(캔들파일경로), _캔들종류, _캔들길이);
	멀바추가(캔들파일경로, 배열크기(캔들파일경로), _종목코드);	// 경로에 종목코드를 붙인다.
	멀바추가(캔들파일경로, 배열크기(캔들파일경로), ".bet");		// 경로에 확장자를 붙인다.
	디뷰("경로: %s", 캔들파일경로);

	dk::C_FILE 파일;
	//bool 초기세팅 = false;	// 혹시 모를 예외를 위해서 종목이 전환되면 세팅을 새로한다.
	bool 마지막데이터갱신 = false;
	롱 파일상태 = dk::_파일없음_;
	롱 저장된마지막날짜 = 0, 저장된마지막시간 = 0, 저장된캔들수 = 0;
	char 임시텍스트버퍼[(1 << 4)] = { 0 };

	bool 재요청해야하는가 = false;
	do
	{
		저장된마지막날짜 = 저장된마지막시간 = 저장된캔들수 = 파일상태 = 0;
		마지막데이터갱신 = false;
		// 파일이 있으면 읽어온다.
		파일상태 = dk::파일체크(캔들파일경로);
		if (dk::_파일있음_ == 파일상태)
		{
			//디뷰("연다: %s", 캔들파일경로);
			파일.열기(캔들파일경로);
			//디뷰("결과: %d / %x", 파일열기결과, 파일.GetFileData());
			바이트포 파일내용포 = 파일.GetFileData();
			베트파일헤더포 헤더포 = (베트파일헤더포)파일내용포;
			if (0x0000E90C == 헤더포->nFileType)
			{
				//디뷰("_캔들종류: %d / %d", _캔들종류, 헤더포->nStickType);
				if (헤더포->nStickType != _캔들종류)
				{
					//디뷰("캔들종류인덱스가 다름 확인해야함.");
					return;
				}
				dk::초기화(임시텍스트버퍼, 배열크기(임시텍스트버퍼));
				switch (헤더포->nStickType)
				{
				case 0:
					strcpy_s(임시텍스트버퍼, "분봉");
					break;
				case 1:
					strcpy_s(임시텍스트버퍼, "일봉");
					break;
				case 2:
					strcpy_s(임시텍스트버퍼, "주봉");
					break;
				case 3:
					strcpy_s(임시텍스트버퍼, "월봉");
					break;
				case 4:
					strcpy_s(임시텍스트버퍼, "틱봉");
					break;
				}
				저장된마지막날짜 = ::ntohl(헤더포->nLastDate);
				저장된마지막시간 = ::ntohl(헤더포->nLastTime);
				저장된캔들수 = 헤더포->nCountSticks;
			}
			//else
			//{
			//	디뷰("베트파일 아님");
			//}
			//디뷰("읽음: %d%s %d개 - 날짜 %d, 시간 %d", 헤더포->nStickLength, 임시텍스트버퍼, 저장된캔들수, 저장된마지막날짜, 저장된마지막시간);
			파일.닫기();
			if (!저장된마지막날짜 || (!_캔들종류 && !저장된마지막시간) || !저장된캔들수)
			{	// 마지막 날짜를 못 얻었으면 처음부터 저장해야한다.
				//디뷰("헤더에서 정보를 얻지 못함, 파일을 삭제한다 %d / %d / %d / %d", 저장된마지막날짜, 저장된마지막시간, 저장된캔들수, 캔들종류인덱스);
				do
				{
					if (::DeleteFileA(캔들파일경로))
					{
						파일상태 = dk::파일체크(캔들파일경로);
					}
					dk::멈춰(200);
				} while (파일상태);
				//디뷰("재요청모드 진행한다.");
				재요청해야하는가 = true;	// 날짜를 못얻었으면 재요청 해야함.
			}
			//else
			//{
			//	디뷰("재요청 모드 아님.");
			//}
		}
		else
		{	// 파일이 없으면 재요청을 해야함.
			재요청해야하는가 = true;
		}
	} while (true);
}

void C_MAIN::캔들갱신()
{
	//WORD 캔들의길이들[] = { 1, 3, 5, 10, 15, 30, 45, 60, 130, 135, 240 };
	WORD 캔들의길이들[] = { 1 };

	사이즈 거른종목수 = 거른종목벡터.size();
	퍼포먼스타이머[0].시작();
	for (사이즈 종목인덱스 = 0; 종목인덱스 < 거른종목수; 종목인덱스++)
	{	// 여기는 종목별 초기화 하는 부분.
		char 종목코드[(1 << 4)] = { 0 };
		strcpy_s(종목코드, 배열크기(종목코드), 거른종목벡터[종목인덱스].strCode.c_str());

		바이트 캔들종류인덱스 = 3;						// [0]: 분봉, [1]: 일봉, [2]: 주봉, [3]: 월봉
		롱 캔들길이인덱스 = (롱)배열크기(캔들의길이들);
		워드 요청할캔들길이 = 1;
		do
		{
			요청할캔들길이 = 1;
			if (!캔들종류인덱스)
			{
				if (0 > --캔들길이인덱스)
				{
					//디뷰("캔들길이인덱스가 0 이므로 다음종목으로 넘어간다");
					break;
				}
				요청할캔들길이 = 캔들의길이들[캔들길이인덱스];
			}
			퍼포먼스타이머[1].시작();
			종목갱신(종목코드, 캔들종류인덱스, 요청할캔들길이);
			디뷰("C_MAIN::캔들갱신(): 종목 한개 끝남: %0.6f", 퍼포먼스타이머[1].경과된시간());

		} while (true);
	}
	디뷰("C_MAIN::캔들갱신(end): %0.6f", 퍼포먼스타이머[0].경과된시간());
}

bool C_MAIN::장중인가()
{
	dk::DLOCAL_TIME currentTime;
	ULONG nCurrentSecond = _HMS_TO_SEC_(currentTime.wHour, currentTime.wMinute, currentTime.wSecond);
	if (!currentTime.wDayOfWeek || 6 == currentTime.wDayOfWeek)
	{	// 일요일과 토요일이면

	}
	else
	{
		if (_HM_TO_SEC_(8, 20) < nCurrentSecond		// 오전 8시 20분을 지났고
			&& _HM_TO_SEC_(15, 31) > nCurrentSecond	// 오후 3시 40분이 되기 전이라면
			)
		{	// 장중에는 돌아가면 안됨.
			return(true);
		}
	}
	return(false);
}

void C_MAIN::캔들파일읽어서갱신(bool _덮어쓰기)
{
	//WORD 캔들의길이들[] = { 1, 3, 5, 10, 15, 30, 45, 60, 130, 135, 240 };
	WORD 캔들의길이들[] = { 1, 3, 5, 10, 15, 30, 45, 60, 130, 135, 240 };
	사이즈 거른종목수 = 거른종목벡터.size();
	std::vector<확장캔들포> 읽은캔들보관벡터;
	std::vector<확장캔들포> 받은캔들포벡터;

	dk::C_FILE 파일;
	bool 초기세팅 = false;	// 혹시 모를 예외를 위해서 종목이 전환되면 세팅을 새로한다.
	bool 마지막데이터갱신 = false;
	롱 파일상태 = dk::_파일없음_;
	롱 저장된마지막날짜 = 0, 저장된마지막시간 = 0, 저장된캔들수 = 0, 저장된캔들크기 = 0;

	char 임시텍스트버퍼[(1 << 4)] = { 0 };
	// 오늘날짜 파일을 읽어와서 기록된 종목들을 리스트에 담는다.
	time_t tToday = ::time(0);
	tm t;
	::localtime_s(&t, &tToday);
	::strftime(임시텍스트버퍼, 배열크기(임시텍스트버퍼), "%Y%m%d", &t);

	char 세이브파일경로[_MAX_PATH] = { 0 };
	sprintf_s(세이브파일경로, "%s%s.log", 캔들저장소.c_str(), 임시텍스트버퍼);
	디뷰(세이브파일경로);

	std::vector<std::string> 받은종목리스트;
	{
		롱 세이브파일상태 = dk::파일체크(세이브파일경로);
		if (1 == 세이브파일상태)
		{	// 파일이 있으면
			dk::C_FILE 세이브파일(세이브파일경로);
			char 한줄버퍼[(1 << 10)] = { 0 };
			do
			{
				dk::초기화(한줄버퍼, 배열크기(한줄버퍼));
				if (0 >= 세이브파일.한줄읽기(한줄버퍼, 배열크기(한줄버퍼))) { break; }
				//디뷰("끝낸 종목: %s", 한줄버퍼);
				받은종목리스트.push_back(한줄버퍼);
			} while (true);
			세이브파일.닫기();
		}
	}
	//디뷰("받은종목갯수: %d", 받은종목갯수);
	사이즈 받은종목갯수 = 받은종목리스트.size();
	char 캔들파일경로[_MAX_PATH] = { 0 };
	if (!장중인가())
	{
		워드 분이바뀌었나 = 0;
		퍼포먼스타이머[0].시작();
		for (사이즈 종목인덱스 = 0; 종목인덱스 < 거른종목수; 종목인덱스++)
		{
			dk::DLOCAL_TIME currentTime;
			if (currentTime.wMinute != 분이바뀌었나)
			{	// 여기는 1분에 한번씩 들어온다.
				if (!currentTime.wDayOfWeek || 6 == currentTime.wDayOfWeek)
				{	// 일요일과 토요일이면

				}
				else
				{
					ULONG nCurrentMinute = _HM_TO_SEC_(currentTime.wHour, currentTime.wMinute);
					if (_HM_TO_SEC_(8, 20) < nCurrentMinute
						&& _HM_TO_SEC_(15, 31) > nCurrentMinute
						)
					{	// 오전 8시 20분이 지났으면 종료
						디뷰("이 로그가 남는 경우는 평일 데이터 갱신중 오전 8시 20분이 지났을 경우이다.");
						break;
					}
				}
				분이바뀌었나 = currentTime.wMinute;
			}
			// 여기는 종목별 초기화 하는 부분.
			바이트 캔들종류인덱스 = 3;						// [0]: 분봉, [1]: 일봉, [2]: 주봉, [3]: 월봉
			롱 캔들길이인덱스 = (롱)배열크기(캔들의길이들);
			워드 요청할캔들길이 = 1;

			char 종목코드[(1 << 4)] = { 0 };
			strcpy_s(종목코드, 배열크기(종목코드), 거른종목벡터[종목인덱스].strCode.c_str());

			bool 이미받음 = false;
			for (사이즈 임시인덱스 = 0; 임시인덱스 < 받은종목갯수; 임시인덱스++)
			{
				//디뷰("비교: %s / %s", 종목코드, 받은종목리스트[임시인덱스].c_str());
				if (!받은종목리스트[임시인덱스].compare(종목코드))
				{
					이미받음 = true;
					break;
				}
			}
			if (이미받음)
			{
				//디뷰("이미받음: %s", 종목코드);
				dk::멈춰();
				continue;
			}
			// 캔들파일의 경로는 종목, 종류, 길이에 따라 전부 다르다.
			dk::초기화(캔들파일경로, 배열크기(캔들파일경로));

			bool 재요청해야하는가 = false;
			퍼포먼스타이머[1].시작();
			do
			{
				if (!pCybos->GetIsConnect())
				{
					디뷰("크레온 플러스 접속이 끊김");
					return;
				}
				요청가능확인();
				if (!재요청해야하는가)
				{
					초기세팅 = false;
					마지막데이터갱신 = false;
					파일상태 = dk::_파일없음_;
					저장된마지막날짜 = 0, 저장된마지막시간 = 0, 저장된캔들수 = 0;

					요청할캔들길이 = 1;
					if (!캔들종류인덱스)
					{
						if (!업데이트분봉) { break; }
						if (0 > --캔들길이인덱스)
						{
							//디뷰("캔들길이인덱스가 0 이므로 다음종목으로 넘어간다");
							break;
						}
						요청할캔들길이 = 캔들의길이들[캔들길이인덱스];
					}
					캔들경로세팅(캔들파일경로, 배열크기(캔들파일경로), 캔들종류인덱스, 요청할캔들길이);
					멀바추가(캔들파일경로, 배열크기(캔들파일경로), 종목코드);	// 경로에 종목코드를 붙인다.
					멀바추가(캔들파일경로, 배열크기(캔들파일경로), ".bet");		// 경로에 확장자를 붙인다.
					//디뷰("[%d/%d] 경로: %s", 종목인덱스, 거른종목수, 캔들파일경로);
					// 
					저장된마지막날짜 = 저장된마지막시간 = 저장된캔들수 = 저장된캔들크기 = 파일상태 = 0;
					마지막데이터갱신 = false;
					// 파일이 있으면 읽어온다.
					파일상태 = dk::파일체크(캔들파일경로);
					if (dk::_파일있음_ == 파일상태)
					{
						//디뷰("연다: %s", 캔들파일경로);
						파일.열기(캔들파일경로);
						//디뷰("결과: %d / %x", 파일열기결과, 파일.GetFileData());
						바이트포 파일내용포 = 파일.GetFileData();
						베트파일헤더포 헤더포 = (베트파일헤더포)파일내용포;
						if (0x0000E90C == 헤더포->nFileType)
						{
							//디뷰("캔들종류인덱스: %d / %d", 캔들종류인덱스, 헤더포->nStickType);
							if (헤더포->nStickType != 캔들종류인덱스)
							{
								//디뷰("캔들종류인덱스가 다름 확인해야함.");
								파일.닫기();
								return;
							}
							dk::초기화(임시텍스트버퍼, 배열크기(임시텍스트버퍼));
							switch (헤더포->nStickType)
							{
							case 0:
								strcpy_s(임시텍스트버퍼, "분봉");
								break;
							case 1:
								strcpy_s(임시텍스트버퍼, "일봉");
								break;
							case 2:
								strcpy_s(임시텍스트버퍼, "주봉");
								break;
							case 3:
								strcpy_s(임시텍스트버퍼, "월봉");
								break;
							case 4:
								strcpy_s(임시텍스트버퍼, "틱봉");
								break;
							}
							저장된마지막날짜 = ::ntohl(헤더포->nLastDate);
							저장된마지막시간 = ::ntohl(헤더포->nLastTime);
							저장된캔들수 = 헤더포->nCountSticks;
							저장된캔들크기 = 헤더포->nStickSize;
							if (_덮어쓰기)
							{
								사이즈 nPos = 크기(베트파일헤더);
								퍼포먼스타이머[2].시작();
								for (롱 임시인덱스 = 0; 임시인덱스 < 저장된캔들수; 임시인덱스++)
								{	// 메모리를 할당받고, 스틱을 채운다.
									LPBET_STICK_EX 생성된캔들포 = new BET_STICK_EX();
									생성된캔들포->초기화();

									메모리복사(생성된캔들포, 저장된캔들크기, (LPBET_STICK_EX)(파일내용포 + nPos), 저장된캔들크기);
									읽은캔들보관벡터.push_back(생성된캔들포);
									dk::비트반전_4바이트(읽은캔들보관벡터[임시인덱스]->nDate);
									dk::비트반전_4바이트(읽은캔들보관벡터[임시인덱스]->nTime);
									//디뷰("%d / %d", ::ntohl(pCompany->dqSticks_Min1[i].nDate), ::ntohl(pCompany->dqSticks_Min1[i].nTime));
									nPos += 저장된캔들크기;
								}
								디뷰("메모리 적재 완료 %0.6f", 퍼포먼스타이머[2].경과된시간());
							}
						}
						//else
						//{
						//	디뷰("베트파일 아님");
						//}
						//디뷰("읽음: %d%s %d개 - 날짜 %d, 시간 %d", 헤더포->nStickLength, 임시텍스트버퍼, 저장된캔들수, 저장된마지막날짜, 저장된마지막시간);
						파일.닫기();
						if (!저장된마지막날짜 || (!캔들종류인덱스 && !저장된마지막시간) || !저장된캔들수)
						{	// 마지막 날짜를 못 얻었으면 처음부터 저장해야한다.
							//디뷰("헤더에서 정보를 얻지 못함, 파일을 삭제한다 %d / %d / %d / %d", 저장된마지막날짜, 저장된마지막시간, 저장된캔들수, 캔들종류인덱스);
							do
							{
								if (::DeleteFileA(캔들파일경로))
								{
									파일상태 = dk::파일체크(캔들파일경로);
								}
								dk::멈춰(200);
							} while (파일상태);
							//디뷰("재요청모드 진행한다.");
							재요청해야하는가 = true;	// 날짜를 못얻었으면 재요청 해야함.
						}
						//else
						//{
						//	디뷰("재요청 모드 아님.");
						//}
					}
					else
					{	// 파일이 없으면 재요청을 해야함.
						재요청해야하는가 = true;
					}
				}
				//디뷰("첫세팅 해야하는가: %d / %d", pStockChart->Continue, 초기세팅);
				if (!pStockChart->Continue || !초기세팅)
				{	// 첫 요청이면 요청사항을 세팅한다.
					캔들요청세팅(종목코드, 캔들종류인덱스, 요청할캔들길이);
					초기세팅 = true;
					퍼포먼스타이머[3].시작();
				}
				pStockChart->BlockRequest();	// 요청한다. 캔들요청세팅을 호출하지 않으면 남은 데이터를 연속조회한다. ( 이거 한번에 1씩 소모한다. )
				롱 수신갯수 = pStockChart->GetHeaderValue(3);
				bool 반복중단 = false;
				if (0 < 수신갯수)
				{	// 수신을 받았으면,
					//디뷰("받은갯수: %d", 수신갯수);
					for (롱 캔들인덱스 = 0; 캔들인덱스 < 수신갯수; 캔들인덱스++)
					{
						if (반복중단)
						{
							//디뷰("요청중단 - 생성된 캔들 수: %d", 받은캔들포벡터.size());
							break;
						}
						long 얻어온날짜 = pStockChart->GetDataValue(_날짜_, 캔들인덱스);
						if (0 < 저장된마지막날짜)
						{	// 저장된 날짜를 얻어왔고 (파일을 읽어왔고)
							if (1 > 캔들종류인덱스)
							{	// 분봉, 일봉
								if (저장된마지막날짜 >= 얻어온날짜)	// 20211130 > 20211128 얻어온 날짜가 작으면 중단.
								{	// 저장된 날짜보다 작거나 같으면 이미 저장된 날짜다.
									//디뷰("날짜: %d / %d", 저장된마지막날짜, 얻어온날짜);
									반복중단 = true;
									continue;	// 캔들 생성하지 않는다.
								}
							}
							else
							{	// 주봉, 월봉
								if (저장된마지막날짜 > 얻어온날짜)
								{	// 이미 저장된게 있을 수 있으니 for문만 나가서 다음종목으로
									//디뷰("여기 오긴 오나? 날짜: %d / %d", 저장된마지막날짜, 날짜);
									break;
								}
								if (저장된마지막날짜 == 얻어온날짜)
								{	// 같으면 마지막 데이터 최신값으로 업데이트.
									//디뷰("마지막데이터 갱신: %d", 날짜);
									마지막데이터갱신 = true;
								}
								반복중단 = true;	// 캔들 생성을 진행한다
							}
						}
						LPBET_STICK_EX 생성된캔들포 = new BET_STICK_EX();
						if (생성된캔들포)
						{
							생성된캔들포->초기화();
							생성된캔들포->nDate = 얻어온날짜;
							// 지수는 일자, 시간, 시고저종만 기록한다.
							생성된캔들포->nTime = pStockChart->GetDataValue(_시간_, 캔들인덱스);
							생성된캔들포->fOpen = (플롯)pStockChart->GetDataValue(_시가_, 캔들인덱스);
							생성된캔들포->fHigh = (플롯)pStockChart->GetDataValue(_고가_, 캔들인덱스);
							생성된캔들포->fLow = (플롯)pStockChart->GetDataValue(_저가_, 캔들인덱스);
							생성된캔들포->fClose = (플롯)pStockChart->GetDataValue(_종가_, 캔들인덱스);
							if (4 < 거른종목벡터[종목인덱스].strCode.length())
							{
								생성된캔들포->nTotalVolume = pStockChart->GetDataValue(_거래량_, 캔들인덱스);
								생성된캔들포->nTransferPayment = pStockChart->GetDataValue(_거래대금_, 캔들인덱스);
								if (0 == 캔들종류인덱스)
								{	// 분봉, 틱봉이면 누적체결 수량도 기록한다.
									생성된캔들포->누적매도체결량 = pStockChart->GetDataValue(_누적체결_매도수량_, 캔들인덱스);
									생성된캔들포->누적매수체결량 = pStockChart->GetDataValue(_누적체결_매수수량_, 캔들인덱스);
								}
							}
							받은캔들포벡터.push_back(생성된캔들포);	// 배열에 매우 담는다
						}
						//else
						//{
						//	디뷰("메모리할당 실패");
						//}
					}
				}
				else
				{
					디뷰("[%s] 받은게 없음(실패) 요청: %c", 종목코드, 캔들종류인덱스);
				}
				if (!pStockChart->Continue									// 더 요청할 수 있는 데이터가 없거나
					|| !재요청해야하는가									// 재요청 할 필요 없으면 저장하고 끝냄.
					)
				{	// 이제 데이터를 정렬해서 저장하도록 하자.
					사이즈 저장할캔들수 = 받은캔들포벡터.size();
					if (0 < 저장할캔들수)
					{	// 헤더부터 쓴다.
						//디뷰("저장할캔들수: %d", 저장할캔들수);
						dk::초기화(대용량임시버퍼_시작포인터, 힙크기(대용량임시버퍼_시작포인터));
						대용량임시버퍼_유동포인터 = 대용량임시버퍼_시작포인터;						// 포인터 초기화
						베트파일헤더포 헤더포 = (베트파일헤더포)대용량임시버퍼_유동포인터;
						헤더포->nFileType = 0x0000E90C;
						헤더포->nFileStyle = 0x00000001;
						헤더포->nStockExchange = 1;			// 크레온
						멀바복사(헤더포->크레온코드, 배열크기(헤더포->크레온코드), 종목코드);
						멀바복사(헤더포->풀코드, 배열크기(헤더포->풀코드), 거른종목벡터[종목인덱스].strFullCode.c_str());
						멀바복사(헤더포->종목코드, 배열크기(헤더포->종목코드), &헤더포->크레온코드[1]);
						헤더포->nStickType = 캔들종류인덱스;
						헤더포->nStickLength = 요청할캔들길이;
						헤더포->nStickSize = 확장캔들사용 ? 크기(BET_STICK_EX) : 크기(BET_STICK_DATAF);

						if (1 == 저장할캔들수)
						{	// 오늘 것만
							if (!마지막데이터갱신)
							{	// 붙인다.
								헤더포->nCountSticks = 저장된캔들수++;
								//디뷰("1개 붙인다");
							}
							else
							{	// 업데이트 한다.
								헤더포->nCountSticks = 저장된캔들수;	// 파일 헤더의 캔들수를 증가하지 않음.
								//디뷰("1개 갱신한다");
							}
						}
						else// if (1 < 저장할캔들수)
						{	// 가장 최근 데이터가 [0] 에 있으므로 순서를 뒤집는다.
							std::reverse(받은캔들포벡터.begin(), 받은캔들포벡터.end());
							헤더포->nCountSticks = 저장된캔들수 + 저장할캔들수;
							//디뷰("%d개 붙인다", 저장할캔들수);
						}
						//else
						//{	// 여기는 오면 안되는거라 로그만 남겨볼까
						//	디뷰("받은 캔들정리중 - 여기는 오면 안됨");
						//}
						// ntohl 함수는 변수안의 내용은 놔두고 결과를 리턴만 한다.
						헤더포->nLastDate = ::ntohl(받은캔들포벡터[저장할캔들수 - 1]->nDate);
						if (0 < 받은캔들포벡터[저장할캔들수 - 1]->nTime)
						{
							헤더포->nLastTime = ::ntohl(받은캔들포벡터[저장할캔들수 - 1]->nTime);
						}
						// 잉여버퍼의 포인터를 헤더 다음으로 이동.
						대용량임시버퍼_유동포인터 += 크기(베트파일헤더);
						if (_덮어쓰기)
						{	// 파일로 읽어온걸 잉여버퍼에 복사한다.
							사이즈 읽은캔들수 = 읽은캔들보관벡터.size();
							for (사이즈 인덱스 = 0; 인덱스 < 읽은캔들수; 인덱스++)
							{
								확장캔들포 임시캔들포인터 = 읽은캔들보관벡터[인덱스];
								dk::비트반전_4바이트(임시캔들포인터->nDate);	// dk::ntohl 함수는 변수 안의 내용도 변경한다.
								if (0 < 임시캔들포인터->nTime)
								{	// 시간이 있으면 분봉이다.
									dk::비트반전_4바이트(임시캔들포인터->nTime);
								}
								메모리복사(대용량임시버퍼_유동포인터, 헤더포->nStickSize, 임시캔들포인터, 헤더포->nStickSize);
								대용량임시버퍼_유동포인터 += 헤더포->nStickSize;
							}
						}
						for (사이즈 인덱스 = 0; 인덱스 < 저장할캔들수; 인덱스++)
						{	// 얻어온 내용들을 잉여버퍼에 복사하는거다.
							확장캔들포 임시캔들포인터 = 받은캔들포벡터[인덱스];
							dk::비트반전_4바이트(임시캔들포인터->nDate);		// dk::ntohl 함수는 변수 안의 내용도 변경한다.
							if (0 < 임시캔들포인터->nTime)
							{	// 시간이 있으면 분봉이다.
								임시캔들포인터->nTime *= 1000000;
								dk::비트반전_4바이트(임시캔들포인터->nTime);
							}
							메모리복사(대용량임시버퍼_유동포인터, 헤더포->nStickSize, 임시캔들포인터, 헤더포->nStickSize);
							대용량임시버퍼_유동포인터 += 헤더포->nStickSize;
						}

						사이즈 저장할데이터의크기 = (대용량임시버퍼_유동포인터 - 대용량임시버퍼_시작포인터);
						// 캔들을 임시버퍼로 모두 복사한거다.
						파일.열기(캔들파일경로, GENERIC_READ | GENERIC_WRITE, 0, 파일상태 ? OPEN_EXISTING : CREATE_NEW, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH);
						if (_덮어쓰기)
						{	// 헤더와 내용 몽땅 그냥 덮어쓴다.
							파일.처음에가서쓰기(대용량임시버퍼_시작포인터, 저장할데이터의크기);
						}
						else
						{
							파일.처음에가서쓰기(헤더포, 크기(베트파일헤더));
							// 헤더는 썼으니까 크기에서 헤더크기를 빼준다.
							저장할데이터의크기 -= 크기(베트파일헤더);
							if (!마지막데이터갱신)
							{	// 붙이기 하는 경우, 임시버퍼에서 헤더사이즈를 더한곳부터 캔들데이터다.
								파일.쓰기(대용량임시버퍼_시작포인터 + 크기(베트파일헤더), 저장할데이터의크기);
							}
							else
							{	// 업데이트 하는 경우.
								디워드 마지막데이터의위치 = 파일.파일크기() - 저장할데이터의크기;
								파일.위치이동(마지막데이터의위치);
								파일.현재위치에쓰기(대용량임시버퍼_시작포인터 + 크기(베트파일헤더), 저장할데이터의크기);
							}
						}
						//디뷰("파일을 닫는다");
						파일.닫기();
						if (2 > 접속서버)
						{
							디뷰("[%d/%d] %s %d개(%d), 경로: [%s] - %0.6f"
								, 종목인덱스 + 1
								, 거른종목수
								, 마지막데이터갱신 ? "갱신" : "추가"
								, 저장할캔들수
								, 저장할데이터의크기
								, 캔들파일경로
								, 퍼포먼스타이머[3].경과된시간()
							);
						}
						else
						{
							CMDPRINT("[%d/%d] %s %d개(%d), 경로: [%s] - %0.6f"
								, 종목인덱스 + 1
								, 거른종목수
								, 마지막데이터갱신 ? "갱신" : "추가"
								, 저장할캔들수
								, 저장할데이터의크기
								, 캔들파일경로
								, 퍼포먼스타이머[3].경과된시간()
							);
						}
						// 벡터에 할당된 메모리는 new 로 각자 할당받은거라 수동으로 해제해야한다.
						if (_덮어쓰기)
						{
							//for (사이즈 i = 0; i < 읽은캔들보관벡터.size(); i++) { 메모리해제(읽은캔들보관벡터[i]); } 
							//읽은캔들보관벡터.clear();
							벡터초기화(읽은캔들보관벡터);
						}
						// 이 부분을 나중에 연속된 메모리를 할당하고 한번에 해제하도록 메모리풀을 만들어야한다.
						//for (사이즈 i = 0; i < 저장할캔들수; i++) { 메모리해제(받은캔들포벡터[i]); }
						//받은캔들포벡터.clear();	// 주소들만 보관된거라서 clear 하면 된다.
						벡터초기화(받은캔들포벡터);

						if (0 < 캔들종류인덱스) { 캔들종류인덱스--; }
						재요청해야하는가 = false;
					}
					else
					{	// 저장할 캔들이 없는것.
						if (_덮어쓰기)
						{
							//for (사이즈 i = 0; i < 읽은캔들보관벡터.size(); i++) { 메모리해제(읽은캔들보관벡터[i]); }
							//읽은캔들보관벡터.clear();
							벡터초기화(읽은캔들보관벡터);
						}
						사이즈 벡터크기 = 받은캔들포벡터.size();
						if (0 < 벡터크기)
						{	// 이 부분을 나중에 연속된 메모리를 할당하고 한번에 해제하도록 메모리풀을 만들어야한다.
							//for (사이즈 i = 0; i < 벡터크기; i++) { 메모리해제(받은캔들포벡터[i]); }
							//받은캔들포벡터.clear();	// 주소들만 보관된거라서 clear 하면 된다.
							벡터초기화(받은캔들포벡터);
						}
						디뷰("추가 또는 갱신할 캔들이 없음.");
						if (0 < 캔들종류인덱스) { 캔들종류인덱스--; }
						재요청해야하는가 = false;
					}
					//break;	// 해당 종목을 빠져나가면 안되네.
				}
			} while (true);
			if (2 > 접속서버)
			{
				디뷰(">>> 종목 한개 걸린시간: %0.6f초 ", 퍼포먼스타이머[1].경과된시간());
			}
			else
			{
				CMDPRINT(">>> 종목 한개 걸린시간: %0.6f초 ", 퍼포먼스타이머[1].경과된시간());
			}
			// 끝낸 종목을 기록한다.
			{
				롱 세이브파일상태 = dk::파일체크(세이브파일경로);
				dk::C_FILE 세이브파일(
					세이브파일경로
					, GENERIC_WRITE
					, FILE_SHARE_WRITE
					, (1 == 세이브파일상태) ? OPEN_ALWAYS : CREATE_ALWAYS
					, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH
				);
				char 저장할버퍼[(1 << 10)] = { 0 };
				int 저장할길이 = sprintf_s(저장할버퍼, "%s\n", 종목코드);
				세이브파일.쓰기(저장할버퍼, 저장할길이);
				세이브파일.닫기();
			}

		}
		디뷰("C_MAIN::캔들파일읽어서갱신(end): %0.6f", 퍼포먼스타이머[0].경과된시간());
	}
	else
	{
		디뷰("C_MAIN::캔들파일읽어서갱신(end) - 장중이라 종료합니다");
	}
}

void C_MAIN::PushData(LPPACKET_BASE _pData)
{	// 여기로 온전한 패킷 한개가 들어온다.
	queueNetworkPackets.enqueue(_pData);
}
