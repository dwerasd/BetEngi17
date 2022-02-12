
// OpenAPI.cpp: 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "DOpenAPI.h"
#include "DOpenAPIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// COpenAPI

BEGIN_MESSAGE_MAP(COpenAPI, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// COpenAPI 생성

COpenAPI::COpenAPI()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
	// 파이프 패킷 처리 스레드 생성
	this->ThreadStart();							// 키움 필수
}


// 유일한 COpenAPI 개체입니다.

COpenAPI theApp;


// COpenAPI 초기화

BOOL COpenAPI::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	COpenAPIDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}
	DSAFE_DELETE(키움OCX);
	// 위에서 만든 셸 관리자를 삭제합니다.
	DSAFE_DELETE(pShellManager)
#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif
	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}
/*
void COpenAPI::RequestListConditions()
{
	khOpenAPI.GetConditionLoad();
}

void COpenAPI::RequestListStocks()
{
	퍼포먼스타이머[0].Start();
	// [0]:장내, [3]: ELW, [4]: 뮤추얼펀드, [5]: 신주인수권, [6]: 리츠,	[8]: ETF, [9]: 하이일드펀드, [10]: 코스닥, [30]: K - OTC, [50]: 코넥스(KONEX)
	CStringA strStockCode0 = khOpenAPI.GetCodeListByMarket("0") + khOpenAPI.GetCodeListByMarket("10");
	// CStringA 문자열의 중간 바이너리를 0으로 만들면 어떻게 동작하는지 확인할 수 없으므로 데이터를 복사함.
	size_t nLengthStr = strStockCode0.GetLength() * sizeof(char);	// 유니코드 빌드라면 길이를 2배 증가
	char* pBuffer = new char[nLengthStr + sizeof(char)];		// 문자열의 끝인 0을 포함하기 위해 + sizeof(char)
	memcpy_s(pBuffer, nLengthStr, strStockCode0, nLengthStr);		// 결과를 pBuffer 에 복사

	std::list<std::string> listStockCodes;

	do
	{
		char* pResult = strrchr(pBuffer, ';');		// 우측부터 ;를 찾는다.
		if (nullptr == pResult) { break; }						// 찾지 못하면 반복문을 중지

		pResult += 1;											// ';' 를 찾았으면 그 다음 주소로 +1
		if (0 < strlen(pResult))
		{
			//DBGPRINT("%i / %s", strlen(pResult), pResult);
			listStockCodes.push_back(pResult);
		}
		*(char*)(pResult - 1) = 0;							// 문자열 앞에 ';' 이었던 바이너리를 0 으로 하면 그 곳이 문자열의 끝이 된다.
	} while (true);
	//DBGPRINT("마지막 %i / %s", strlen(pBuffer), pBuffer);
	listStockCodes.push_back(pBuffer);

	// 할당받은 메모리 제거
	DSAFE_DELETE_ARRAY(pBuffer);

	DBGPRINT("전체 종목 수: %i", listStockCodes.size());

	CStringA strStockCodeELW = khOpenAPI.GetCodeListByMarket("3");	// ELW 리스트
	CStringA strExclude1 = khOpenAPI.GetCodeListByMarket("4");	// 뮤추얼펀드 리스트
	CStringA strExclude2 = khOpenAPI.GetCodeListByMarket("5");	// 신주인수권 리스트
	CStringA strExclude3 = khOpenAPI.GetCodeListByMarket("6");	// 리츠 리스트
	CStringA strExclude4 = khOpenAPI.GetCodeListByMarket("8");	// ETF 리스트
	CStringA strExclude5 = khOpenAPI.GetCodeListByMarket("9");	// 하이일드펀드 리스트
	CStringA strExclude6 = khOpenAPI.GetCodeListByMarket("30");	// K- OTC 리스트
	CStringA strExclude7 = khOpenAPI.GetCodeListByMarket("50");	// 코넥스(KONEX) 리스트
	// 오름차순 정렬 000020 ~  
	listStockCodes.sort();
	size_t nCountSend = 0;
	for (auto& itr : listStockCodes)
	{
		PACKET_BASE netPacket =
		{
			sizeof(STOCK_INFO_KIWOOM)				// 데이터크기
			, _PKT_PIPE_RESULT_LOGIN_SUCCEED_		// 헤더
			, 1										// 클라 종류
			, 0										// 더미
			, _VERSION_OPENAPI_						// 클라 버전
			, { 0 }									// 보낼 내용
		};
		LPSTOCK_INFO_KIWOOM pStockInfo = (LPSTOCK_INFO_KIWOOM)&netPacket.bytBuffer[0];

		strcpy_s(pStockInfo->종목코드, itr.c_str());

		//pPipe->Send(&netPacket);
		if (-1 != strExclude1.Find(itr.c_str())
			|| -1 != strExclude2.Find(itr.c_str())
			|| -1 != strExclude3.Find(itr.c_str())
			|| -1 != strExclude4.Find(itr.c_str())
			|| -1 != strExclude5.Find(itr.c_str())
			|| -1 != strExclude6.Find(itr.c_str())
			|| -1 != strExclude7.Find(itr.c_str())
			)
		{
			continue;	// 아예 보내지 않는다.
		}
		// 종목이름
		CStringA strStockName = khOpenAPI.GetMasterCodeName(pStockInfo->szStockCode);
		strcpy_s(pStockInfo->szStockName, _countof(pStockInfo->szStockName), (LPCSTR)strStockName.GetBuffer());
		//if (-1 != strStockName.Find("ETN")
		if (0 != strstr(pStockInfo->szStockName, "ETN"))
		{
			continue;
		}
		// 종목상태 – 정상, 증거금100%, 거래정지, 관리종목, 감리종목, 투자유의종목,	담보대출, 액면분할, 신용가능
		// 복수의 상태가 |로 구분되어 하나의 문자열
		CStringA strStockState = khOpenAPI.GetMasterStockState(pStockInfo->szStockCode);
		strcpy_s(pStockInfo->szStockState, _countof(pStockInfo->szStockState), (LPCSTR)strStockState.GetBuffer());

		// 감리구분 – 정상, 투자주의, 투자경고, 투자위험, 투자주의환기종목
		CStringA strControlKind = khOpenAPI.GetMasterConstruction(pStockInfo->szStockCode);
		//DBGPRINT("감리구분: %s", strControlKind.GetBuffer());
		strcpy_s(pStockInfo->szControlKind, _countof(pStockInfo->szControlKind), (LPCSTR)strControlKind.GetBuffer());

		// 종목을 한개씩 보낸다.
		//pPipe->Send(_PIPE_KIWOOM_RECIVE_GET_STOCK_, (LPVOID)pStockInfo, sizeof(STOCK_BASE_INFO));

		pMessageBundle->dwHeader = _NET_MSG_KIWOOM_RECIVE_GET_STOCK_;
		pMessageBundle->nDataSize = sizeof(STOCK_BASE_INFO);
		MessageHandler.PushData(pMessageBundle);
		pMessageBundle = nullptr;
		nCountSend++;
	}
	// 할당받은 메모리 제거
	//DSAFE_DELETE(pStockInfo);
	DBGPRINT("걸러낸 종목: %i개, 완료까지 걸린 시간: %0.0f", nCountSend, 퍼포먼스타이머[0].Check());

	pMessageBundle = (LPCALLBACK_MESSAGE)MessageHandler.Alloc();
	pMessageBundle->dwHeader = _NET_MSG_KIWOOM_END_GET_STOCK_;
	MessageHandler.PushData(pMessageBundle);

	listStockCodes.clear();
	//pPipe->Send(_PIPE_KIWOOM_END_GET_STOCK_);
}
*/
