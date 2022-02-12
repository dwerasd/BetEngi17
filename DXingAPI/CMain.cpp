#include "framework.h"
#include "CMain.h"



C_MAIN* pMain = nullptr;
LRESULT CALLBACK WndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	//static C_MAIN* pMain = nullptr;
	if (nullptr != pMain)
	{
		if (pMain->OnWindowMessage(_hWnd, _nMessage, _wParam, _lParam))
		{
			return(0);
		}
	}
	switch (_nMessage)
	{
	case WM_CREATE:
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)_lParam;
		if (nullptr == pMain)
		{
			pMain = (C_MAIN*)pcs->lpCreateParams; // 파라미터에 this 가 들어있다 그걸 뽑아서
		}
	}
	break;
	case WM_DESTROY:
		DBGPRINT("WndProc() - WM_DESTROY");
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(_hWnd, _nMessage, _wParam, _lParam);
}

C_MAIN::C_MAIN(HINSTANCE _hInst)
	: hInst(_hInst)
	, bEnable(true)
	, bVisible(false)
	, bChangeSizeWindow(false)
	, bExitProcess(false)
	, pTrayIcon(nullptr)

	, pPipe(nullptr)
	, pXingAPI(nullptr)
{

}

C_MAIN::~C_MAIN()
{

}

bool C_MAIN::Init(LPCSTR _tszClassName, LPCSTR _tszWindowName)
{
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

		테스트모드 = (bool)::GetPrivateProfileIntA("base", "test_mode", 0, 설정파일);

		::GetPrivateProfileStringA("path", "sticks", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		캔들저장소 = 임시버퍼;

		접속서버 = (int)::GetPrivateProfileIntA("base", "server", 0, 설정파일);
		디뷰("C_MAIN::Init() - 접속서버: %s", 접속서버 ? "모의서버" : "실서버");
		에러메시지표시여부 = (BOOL)::GetPrivateProfileIntA("base", "active_error_message", 1, 설정파일);

		::GetPrivateProfileStringA("account", "ebest_id", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		이베스트계정 = 임시버퍼;
		::GetPrivateProfileStringA("account", "ebest_pwd", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		이베스트비밀번호 = 임시버퍼;
		::GetPrivateProfileStringA("account", "ebest_certpwd", "", 임시버퍼, 배열크기(임시버퍼), 설정파일);
		인증서비밀번호 = 임시버퍼;

		//디뷰("%s / %s / %s", 크레온계정.c_str(), 크레온비밀번호.c_str(), 인증서비밀번호.c_str());
	}

	WNDCLASSEXA wcex = { sizeof(WNDCLASSEXA), 0 };
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursorA(nullptr, MAKEINTRESOURCEA(32512));
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	wcex.lpszClassName = _tszClassName;

	if (RegisterClassExA(&wcex))
	{
		strClassName = _tszClassName;
		strWindowName = _tszWindowName;

		rcWindow.Set(0, 0, 1280, 800);
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
		//dwWindowStyle &= WS_CAPTION;
		//dwWindowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE | WS_MINIMIZEBOX | WS_MAXIMIZEBOX
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
	do
	{
		DWORD dwWindowStyle = GetWinFormStyle(true);
		dk::DRECT rcTemp(rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom);
		AdjustWindowRect(
			&rcTemp
			, dwWindowStyle
			, FALSE
		);
		dk::DPOINT ptWindowCenter(
			((GetSystemMetrics(SM_CXSCREEN) - (int)rcWindow.right) / 2)
			, ((GetSystemMetrics(SM_CYSCREEN) - (int)rcWindow.bottom) / 2)
		);
		hWnd = CreateWindowA(
			strClassName.c_str()
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
		if (0 != hWnd)
		{
			if (!pTrayIcon)
				pTrayIcon = new C_TRAY_ICON(
					hWnd
					, WM_TRAYICON
					, L"테스트"
					, LoadIconA(hInst, (LPCSTR)IDI_XINGAPI)
					, IDI_XINGAPI
				);

			if (!pXingAPI)
				pXingAPI = new C_XING_API();

			if (!pXingAPI->Init(hWnd))
			{
				DBGPRINT("pXingAPI 초기화 실패");
				pXingAPI->Destroy();
				DSAFE_DELETE(pXingAPI);
			}
			// 파이프 생성.
			pPipe = new pipe::C_PIPE_CLIENT(L"PipeClientEBest", L"PipeServerEBest");
			pPipe->ThreadStart();
			return(true);
		}
		else
		{
			DBGPRINT("윈도우 생성 실패");
		}
	} while (false);
	return(false);
}

long C_MAIN::Calculate()
{
	long nResult = 0;
	do
	{
		if (bExitProcess)
		{
			nResult = 1;
			break;
		}
		LPPACKET_BASE 파이프패킷 = nullptr;
		if (queueNetworkPackets.try_dequeue(파이프패킷))
		{
			switch (파이프패킷->nPacketIndex)
			{
			case _PKT_STOCK_PACKET_INDEX_NONE_:
				DBGPRINT("받음(_PKT_INDEX_NONE): %d", 파이프패킷->nPacketIndex);
				break;
			case _PKT_PIPE_CONNECTED_:
				DBGPRINT("받음(_PKT_PIPE_CONNECTED): %s", 파이프패킷->bytBuffer);
				pPipe->Send(_PKT_PIPE_CONNECTED_XINGAPI_);
				break;
			case _PKT_PIPE_INIT_XINGAPI_:
				;
				{	// "아이디", "비밀번호", "인증서 비밀번호", "모의서버 여부"를 서버에서 받는다.
					{
						디뷰("접속서버: %s", (!접속서버) ? "hts.ebestsec.co.kr" : "demo.etrade.co.kr");
						BOOL bResult = pXingAPI->Connect(
							(!접속서버) ? "hts.ebestsec.co.kr" : "demo.etrade.co.kr"
							, 20001
							// XingAPI에서 사용하는 메시지의 시작번호, Windows에서는 사용자메시지를 0x400(=WM_USER) 이상을
							// 사용해야 함. 기본적으로는 WM_USER를 사용하면 되지만 프로그램 내부에서 메시지 ID가 겹치게 되면
							// 이 값을 조정하여 메시지 ID 충돌을 피할수 있음
							, XM_DEFAULT_VALUE
							// 지정한 시간이상(1/1000 초 단위)으로 시간이 걸리게 될 경우 연결실패로 간주함
							, -1
							// 보내어지는 Packet Size, -1 이면 기본값 사용
							// 인터넷 공유기등에서는 특정 크기 이상의 데이터를 한번에 보내면 에러가 떨어지는 경우가 발생
							// 이럴 경우에 한번에 보내는 Packet Size를 지정하여 그 이상 되는 Packet은 여러번에 걸쳐 전송
							, 1024
						);
						// 접속실패 처리
						if (!bResult)
						{
							int nErrorCode = pXingAPI->GetLastError();
							std::string strMsg = pXingAPI->GetErrorMessage(nErrorCode);
							DBGPRINT("이베스트 서버 접속 실패: %s", strMsg.c_str());
							// send failed xingapi connect
							break;
						}
						DBGPRINT("이베스트 서버 접속 성공");
					}
					{
						BOOL bResult = pXingAPI->Login(
							이베스트계정.c_str()
							, (!접속서버) ? 이베스트비밀번호.c_str() : "ahdml12"
							, (!접속서버) ? 인증서비밀번호.c_str() : ""
							, 접속서버
							, 에러메시지표시여부	// 공인인증 에러 발생시 에러메시지 표시 여부
						);
						if (!bResult)
						{
							int nErrorCode = pXingAPI->GetLastError();
							std::string strMsg = pXingAPI->GetErrorMessage(nErrorCode);
							DBGPRINT("이베스트 로그인 요청 실패: %s", strMsg.c_str());
							// send failed request login
							pPipe->Send(_PKT_PIPE_INIT_FAILED_);
							break;
						}
	
						DBGPRINT("이베스트 로그인 요청 성공");
					}
					// send success request login
				}
				break;
			case _PKT_PIPE_DESTROY_:
				DBGPRINT("받음(_PKT_PIPE_DESTROY_)");
				bExitProcess = true;
				break;
			case _PKT_PIPE_REALTIME_DATA_XINGAPI_:
				do
				{	// 실시간 데이터 요청.
					요청_실시간_체결_이베스트포 요청패킷 = (요청_실시간_체결_이베스트포)&파이프패킷->bytBuffer[0];
					BOOL bResult = FALSE;
					if (1 == 요청패킷->장구분)
					{
						//bResult = pXingAPI->AdviseRealData("S2_", 요청패킷->종목코드, sizeof(_S3_InBlock));			// 코스피 우선호가
						bResult = pXingAPI->AdviseRealData("S3_", 요청패킷->종목코드, sizeof(_S3_InBlock));			// 코스피 체결
						//bResult = pXingAPI->AdviseRealData("S4_", 요청패킷->종목코드, sizeof(_S3_InBlock));		// 코스피 기세
						bResult = pXingAPI->AdviseRealData("H1_", 요청패킷->종목코드, sizeof(_S3_InBlock));			// 코스피 호가잔량
						//bResult = pXingAPI->AdviseRealData("K1_", 요청패킷->종목코드, sizeof(_S3_InBlock));		// 코스피 거래원
						//bResult = pXingAPI->AdviseRealData("PH_", 요청패킷->종목코드, sizeof(_S3_InBlock));		// 코스피 프로그램매매종목별
					}
					if (2 == 요청패킷->장구분)
					{
						//bResult = pXingAPI->AdviseRealData("KS_", 요청패킷->종목코드, sizeof(_K3_InBlock));			// 코스닥 우선호가
						bResult = pXingAPI->AdviseRealData("K3_", 요청패킷->종목코드, sizeof(_K3_InBlock));			// 코스닥 체결
						
						bResult = pXingAPI->AdviseRealData("HA_", 요청패킷->종목코드, sizeof(_S3_InBlock));			// 코스닥 호가잔량
						//bResult = pXingAPI->AdviseRealData("OK_", 요청패킷->종목코드, sizeof(_K3_InBlock));		// 코스닥 거래원
						//bResult = pXingAPI->AdviseRealData("KH_", 요청패킷->종목코드, sizeof(_S3_InBlock));		// 코스닥 프로그램매매종목별
					}
					//실시간등록코드.insert(std::unordered_map<std::string, BYTE>::value_type(요청패킷->종목코드, 요청패킷->장구분));
					실시간_등록_이베스트 등록 = { 0 };
					등록.장구분 = 요청패킷->장구분;
					strcpy_s(등록.종목코드, 요청패킷->종목코드);
					등록.등록여부 = true;
					실시간등록코드.push_back(등록);
				} while (false);
				break;
			default:
				DBGPRINT("받음(default): %d", 파이프패킷->nPacketIndex);
				break;
			}
			DSAFE_DELETE(파이프패킷);
		}
	} while (false);
	return(nResult);
}

void C_MAIN::Display()
{
	
}

void C_MAIN::Destroy()
{
	BOOL bResult = FALSE;
	bResult = pXingAPI->UnadviseRealData("IJ_", "001", 3);
	bResult = pXingAPI->UnadviseRealData("IJ_", "101", 3);
	bResult = pXingAPI->UnadviseRealData("IJ_", "201", 3);

	for (auto itr : 실시간등록코드)
	{
		if (itr.등록여부)
		{
			if (1 == itr.장구분)
			{
				디뷰("실시간 해제: %s", itr.종목코드);
				//bResult = pXingAPI->UnadviseRealData("S2_", itr.종목코드, sizeof(_S3_InBlock));
				bResult = pXingAPI->UnadviseRealData("S3_", itr.종목코드, sizeof(_S3_InBlock));
				//bResult = pXingAPI->UnadviseRealData("S4_", itr.종목코드, sizeof(_S3_InBlock));
				bResult = pXingAPI->UnadviseRealData("H1_", itr.종목코드, sizeof(_S3_InBlock));
				//bResult = pXingAPI->UnadviseRealData("K1_", itr.종목코드, sizeof(_S3_InBlock));
				//bResult = pXingAPI->UnadviseRealData("PH_", itr.종목코드, sizeof(_S3_InBlock));
			}
			if (2 == itr.장구분)
			{
				//pXingAPI->UnadviseRealData("KS_", itr.종목코드, sizeof(_K3_InBlock));
				pXingAPI->UnadviseRealData("K3_", itr.종목코드, sizeof(_K3_InBlock));
				//pXingAPI->UnadviseRealData("OK_", itr.종목코드, sizeof(_K3_InBlock));
				pXingAPI->UnadviseRealData("HA_", itr.종목코드, sizeof(_K3_InBlock));
				//pXingAPI->UnadviseRealData("KH_", itr.종목코드, sizeof(_K3_InBlock));
			}
			itr.초기화();
		}
	}
	DSAFE_DELETE(pPipe);
	DBGPRINT("C_MAIN::Destroy() - XingAPI 로그아웃");
	pXingAPI->Logout();
	DBGPRINT("C_MAIN::Destroy() - XingAPI 접속종료");
	pXingAPI->Disconnect();
	DBGPRINT("C_MAIN::Destroy() - XingAPI 언로드");
	pXingAPI->Destroy();
	DSAFE_DELETE(pXingAPI);

	DSAFE_DELETE(pTrayIcon);
	
	bExitProcess = true;
}

bool C_MAIN::OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	// XingAPI 메시지를 먼저 확인한다.
	if (pXingAPI && pXingAPI->OnWindowMessage(_nMessage, _wParam, _lParam))
	{	// XingAPI 였다면 리턴해서 끝낸다.
		return(true);
	}
	// 조건에 다 맞는지 확인하는것보다. 하나라도 틀리면 리턴하는게 빠르다.
	switch (_nMessage)
	{
	case WM_COMMAND:
		if (WM_TRAY_CLOSE == _wParam) { bExitProcess = true; }
		return(true);
	case WM_TRAYICON:
		if (IDI_XINGAPI == _wParam)
		{
			if (WM_RBUTTONUP == LOWORD(_lParam))
			{
				POINT pt;
				::GetCursorPos(&pt);

				HMENU hMenu = ::CreatePopupMenu();

				//::InsertMenu(hMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, WM_TRAY_CLOSE, __TEXT("XingAPI"));
				//::InsertMenu(hMenu, 0xFFFFFFFF, MF_SEPARATOR, 0, __TEXT("LINE"));
				::AppendMenu(hMenu, MF_STRING, 0, __TEXT("XingAPI"));
				::AppendMenu(hMenu, MF_SEPARATOR, 0, __TEXT("LINE"));

				::AppendMenu(hMenu, MF_STRING, WM_TRAY_CLOSE, __TEXT("종료"));

				::SetForegroundWindow(_hWnd);	// 생성된 팝업 메뉴 밖을 클릭할 때 팝업 닫기
				::TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, _hWnd, NULL);
			}
		}
		return(true);
	}
	return(false);
}

void C_MAIN::PushData(LPPACKET_BASE _pData)
{	// 여기로 온전한 패킷 한개가 들어온다.
	queueNetworkPackets.enqueue(_pData);
}

void C_MAIN::파이프전송(LPPACKET_BASE _pData)
{
	if (pPipe)
	{
		pPipe->Send(_pData);
	}
}

void C_MAIN::파이프전송(WORD _pData)
{
	if (pPipe)
	{
		pPipe->Send(_pData);
	}
}
