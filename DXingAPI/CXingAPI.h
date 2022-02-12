#pragma once


#include <DarkCore/DUtil.h>

#include "XingAPIDef.h"



class C_XING_API
{
private:
	HWND hWnd;
	HMODULE hXingAPI;

	FP_CONNECT					m_fpConnect;
	FP_ISCONNECTED				m_fpIsConnected;
	FP_DISCONNECT				m_fpDisconnect;
	FP_LOGIN					m_fpLogin;
	FP_LOGOUT					m_fpLogout;

	FP_GETLASTERROR				m_fpGetLastError;
	FP_GETERRORMESSAGE			m_fpGetErrorMessage;

	FP_REQUEST					m_fpRequest;
	FP_ADVISEREALDATA			m_fpAdviseRealData;
	FP_UNADVISEREALDATA			m_fpUnadviseRealData;
	FP_UNADVISEWINDOW			m_fpUnadviseWindow;
	FP_RELEASEREQUESTDATA		m_fpReleaseRequestData;
	FP_RELEASEMESSAGEDATA		m_fpReleaseMessageData;

	FP_GETACCOUNTLISTCOUNT		m_fpGetAccountListCount;
	FP_GETACCOUNTLIST			m_fpGetAccountList;
	FP_GETACCOUNTNAME			m_fpGetAccountName;
	FP_GETACCTDETAILNAME		m_fpGetAcctDetailName;
	FP_GETACCTNICKNAME			m_fpGetAcctNickname;

	FP_GETCOMMMEDIA				m_fpGetCommMedia;
	FP_GETETKMEDIA				m_fpGetETKMedia;
	FP_GETCLIENTIP				m_fpGetClientIP;
	FP_GETSERVERNAME			m_fpGetServerName;
	FP_GETAPIPATH				m_fpGetAPIPath;

	FP_SETHEADERINFO			m_fpSetHeaderInfo;
	FP_SETUSEAPIVER				m_fpSetUseAPIVer;
	FP_SETMODE					m_fpSetMode;

	FP_GETPROCBRANCHNO			m_fpGetProcBranchNo;
	FP_GETUSEOVERFUTURE			m_fpGetUseOverFuture;
	FP_GETUSEFX					m_fpGetUseFX;

	FP_GETTRCOUNTPERSEC			m_fpGetTRCountPerSec;
	FP_GETTRCOUNTBASESEC		m_fpGetTRCountBaseSec;
	FP_GETTRCOUNTREQUEST		m_fpGetTRCountRequest;
	FP_GETTRCOUNTLIMIT			m_fpGetTRCountLimit;

	FP_SETNOTIFYFLAG			m_fpSetNotifyFlag;

	FP_REQUESTSERVICE			m_fpRequestService;
	FP_REMOVESERVICE			m_fpRemoveService;

	FP_REQUESTLINKTOHTS			m_fpRequestLinkToHTS;
	FP_ADVISELINKFROMHTS		m_fpAdviseLinkFromHTS;
	FP_UNADVISELINKFROMHTS		m_fpUnAdviseLinkFromHTS;

	FP_DECOMPRESS				m_fpDecompress;
	FP_ISCHARTLIB				m_fpIsChartLib;

	FP_SETPROGRAMORDER			m_fpSetProgramOrder;
	FP_GETPROGRAMORDER			m_fpGetProgramOrder;

public:
	C_XING_API();
	~C_XING_API();

	// 콜백
	bool OnWindowMessage(UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

	// 초기화 관련
	bool	Init(HWND _hWnd);
	void	Destroy();

	// 서버연결
	BOOL	Connect(LPCSTR pszSvrIP, int nPort, int nStartMsgID, int nTimeOut = -1, int nSendMaxPacketSize = -1);
	BOOL	IsConnected();
	BOOL	Disconnect();

	// 로그인
	BOOL	Login(LPCSTR pszID, LPCSTR pszPwd, LPCSTR pszCertPwd, int nType, BOOL bShowCertErrDlg = TRUE);
	BOOL	Logout();
	
	int		GetLastError();
	std::string	GetErrorMessage(int nErrorCode);
	
	// 조회성TR 관련
	int		Request(LPCSTR pszCode, LPVOID lpData, int nDataSize, BOOL bNext = FALSE, LPCSTR pszNextKey = "", int nTimeOut = 30);
	BOOL	ReleaseRequestData(int nRequestID);
	BOOL	ReleaseMessageData(LPARAM lp);

	// 실시간TR 관련
	BOOL	AdviseRealData(LPCSTR pszTrNo, LPCSTR pszData, int nDataUnitLen);
	BOOL	UnadviseRealData(LPCSTR pszTrNo, LPCSTR pszData, int nDataUnitLen);
	BOOL	UnadviseWindow();

	// 계좌 관련
	int		GetAccountListCount();
	BOOL	GetAccountList(int nIndex, LPSTR pszAcc, int nAccSize);
	BOOL	GetAccountName(LPCSTR pszAcc, LPSTR pszAccName, int nAccNameSize);
	BOOL	GetAcctDetailName(LPCSTR pszAcc, LPSTR pszAcctDetailName, int nAcctDetailNameSize);
	BOOL	GetAcctNickname(LPCSTR pszAcc, LPSTR pszAcctNickname, int nAcctNicknameSize);

	// 정보얻기
	std::string	GetCommMedia();			// 통신매체를 구한다.
	std::string	GetETKMedia();			// 당사매체를 구한다.
	std::string	GetClientIP();			// 공인IP를 구한다.
	std::string	GetServerName();			// 서버명을 구한다.
	std::string GetAPIPath();			// 실행중인 xingAPI의 경로

	void	SetHeaderInfo(LPCSTR szType, LPCSTR szValue);
	void	SetUseAPIVer(LPCSTR szUserAPIVer);
	BOOL	SetMode(LPCSTR pszMode, LPCSTR pszValue);

	std::string	GetProcBranchNo();			// 처리점을 구한다.
	BOOL	GetUseOverFuture();			// 해외선물 사용권한	
	BOOL	GetUseFX();			// FX 사용권한
	BOOL	IsChartLib();			// 차트라이브러리 연결

	int		GetTRCountPerSec(LPCSTR pszCode);				// N초당 전송 가능 횟수 : N초내 가능 횟수
	int		GetTRCountBaseSec(LPCSTR pszCode);				// N초당 전송 가능 횟수 : N초
	int		GetTRCountRequest(LPCSTR pszCode);				// TR 호출 횟수
	int		GetTRCountLimit(LPCSTR pszCode);				// 10분당 전송 가능 횟수

	void	SetNotifyFlag(BOOL bNotifyFlag);	// 긴급메시지, 서버접속 단절통지 등의 통보 설정 (지원 예정)

	void	SetProgramOrder(BOOL bProgramOrder);				// 프로그램매매 설정
	BOOL	GetProgramOrder();									// 프로그램매매 설정여부 확인

	//------------------------------------------------------------------------------
	// 부가 서비스 TR 조회
	//     반환값       - Request() 함수와 이용 방식 동일
	//     _hWnd			- 조회 결과를 받을 윈도우의 핸들
	//
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [1] 종목 검색        
	// ★★★★★
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//     pszCode		- "t1833" (HTS '[1807] 종목검색' 에서 'API 로 내보내기' 저장한 조건의 종목을 검색하는 TR)
	//     lpData		- 'API로 내보내기' 한 파일의 전체 경로 지정, NULL 입력시 파일다이얼로그 표시
	//
	//     ex) HTS '[1807] 종목검색' 에서 'API 로 내보내기' 저장한 파일이 "D:\test.adf"
	//			pszCode = "t1833", pszData = "D:\test.adf"   
	//			int nReqID = RequestService( _hWnd, "t1833", "D:\test.adf" );	
	// 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [2] ~ [3] 차트 기초데이터를 이용해 지표데이터를 제공
	// ★★★★★
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//	지표데이터는 챠트 기초데이터를 수신받아 API내부에서 가공하여 제공하는 것으로 조회 및 실시간 응답에 
	//  다소 시간이 걸릴 수 있습니다.
	//  자료실게시판 내에 "ChartApiSample(VC++ 2012)" 샘플을 참고하시기 바랍니다.
	//
	//  ※ 조회시 실시간 자동등록을 하면, 실시간 지표데이터 발생시 XM_RECEIVE_REAL_DATA_CHART 로 메시지가 수신
	//	  WPARAM = 해당 없음 
	//	  LPARAM = LPRECV_REAL_PACKET (RECV_REAL_PACKET 의 pszData  = 조회 TR의 outblock1과 동일)
	//
	//
	//  [2] 차트 지표데이터 조회 (HTS '[4201]xing차트1'의 수식관리자내 지표와 동일하며, DevCenter 메뉴 [부가서비스-수식관리자] 를 통해 지표 저장)
	//
	//     pszCode		- "ChartIndex" (차트 지표데이터 조회용 TR)
	//     lpData		- "ChartIndex" TR내 Inblock의 데이터 구조체
	//
	//     ex) "MACD" 지표데이터 조회
	//         ChartIndexInBlock sInBlock;
	//         sInBlock.indexid		= 지표ID	 // 최초 조회시 공백, '동일 종목 - 동일 지표' 조회시 이전 조회 ChartIndexOutBlock의 indexid
	//         sInBlock.indexname	= "MACD" 
	//         sInBlock.market		= "1"		 // 주식
	//         ...생략.. 
	//		   RemoveService( _hWnd, "ChartIndex", sInBlock.indexid );				
	//         int nReqID = RequestService( _hWnd, "ChartIndex", &sInBlock );	
	// 
	// 
	// [3] 차트 엑셀데이터 조회 (HTS '[4201]xing차트1'의 수식관리자내 지표와 동일하며, DevCenter 메뉴 [부가서비스-수식관리자] 를 통해 지표 저장)
	//     직접 저장한 차트 기초데이터를 엑셀 포맷으로 변경한 후, RequestService() 호출 시 지표데이터로 가공하여 제공  
	//     ("xingAPI 설치폴더/엑셀샘플/ChartExcelData.xls" 참고)
	//
	//     pszCode		- "ChartExcel" (차트 지표데이터 조회용 TR)
	//     lpData		- "ChartExcel" TR내 Inblock의 데이터 구조체
	//
	//     ex) 직접 쌓은 시고저종 데이터를 엑셀 포맷으로 변환하여 저장한 파일이 "C:\ebest\xingAPI\엑셀샘플\ChartExcelData.xls"
	//         ChartExcelInBlock sInBlock;
	//          sInBlock.indexid		= 지표ID	 // 최초 조회시 공백, '동일 종목 - 동일 지표' 조회시 이전 조회 ChartIndexOutBlock의 indexid
	//         sInBlock.indexname		= "MACD"		
	//         sInBlock.excelfilename	= "C:\ebest\xingAPI\엑셀샘플\ChartExcelData.xls"
	//         ...생략.. 
	//         RemoveService( _hWnd, "ChartExcel", sInBlock.indexid );				
	//         int nReqID = RequestService( _hWnd, "ChartExcel", &sInBlock );	
	// 
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [4] ~ [5] e종목검색 실시간 데이터 제공
	// ★★★★★
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//  자료실게시판 내에 "XingAPI_Sample_eSearch" 샘플을 참고하시기 바랍니다.
	//
	//  ※ 조회시 실시간 자동등록을 하면, 실시간 데이터 발생시 XM_RECEIVE_REAL_DATA_SEARCH 로 메시지가 수신
	//	  WPARAM = 해당 없음 
	//	  LPARAM = LPRECV_REAL_PACKET (RECV_REAL_PACKET 의 pszData  = 조회 TR의 outblock1과 동일)
	//
	//
	//  [4] 종목검색 데이터 조회
	//
	//     pszCode		- "t1857" (HTS '[1892] e종목검색' 에서 'API 로 내보내기' 혹은 '전략관리 -> 서버저장' 으로  저장한 조건의 종목을 검색하는 TR)
	//     lpData		- "t1857" TR내 Inblock의 데이터 구조체
	//
	//     ex) 종목검색 
	//         t1857InBlock	pckInBlock;
	//		   TCHAR		szTrNo[]	= "t1857";
	//		   char			szNextKey[]	= "";
	//         ...생략.. 
	//		   SetPacketData( pckInBlock.sRealFlag		, sizeof( pckInBlock.sRealFlag		), str_Real	   , DATA_TYPE_STRING );	// 실시간 여부 1:등록 0:조회만
	//		   SetPacketData( pckInBlock.sSearchFlag	, sizeof( pckInBlock.sSearchFlag	), str_Flag	   , DATA_TYPE_STRING );	// 조회구분값 S:서버 F:파일
	//		   SetPacketData( pckInBlock.query_index	, sizeof( pckInBlock.query_index	), str_Index   , DATA_TYPE_STRING );	// 종목검색입력값
	//         int nReqID = RequestService( _hWnd, szTrNo, (LPCSTR)&pckInBlock );	
	// 
	// 
	// [5] 종목검색 실시간 데이터 발생 시 XM_RECEIVE_REAL_DATA_SEARCH로 메세지 수신
	//
	//	   WPARAM = 해당 없음 
	//	   LPARAM = LPRECV_REAL_PACKET (RECV_REAL_PACKET 의 pszData  = 조회 TR의 outblock1과 동일)
	//
	//     ex) LPRECV_REAL_PACKET pRealPacket = (LPRECV_REAL_PACKET)lParam;
	//		   LPt1857OutBlock1 pOutBlock = (LPt1857OutBlock1)pRealPacket->pszData;
	// 
	// 	
	int		RequestService(LPCSTR pszCode, LPCSTR pszData);


	//------------------------------------------------------------------------------
	// 부가 서비스 조회 TR 해제
	//     반환값       - 부가서비스에 따라 달라짐
	//     _hWnd			- 조회 결과를 받을 윈도우의 핸들
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [1] 종목 검색
	// ★★★★★ 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//		해당 없음
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [2] ~ [3] 차트데이터 조회 
	// ★★★★★ 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//		지표데이터는 챠트 기초데이터를 수신받아 API내부에서 가공하여 제공하는 것으로
	//      많이 조회할수록 API에 부하가 갈 수 있으니, 사용하지 않는 지표는 해제하는 것이 좋습니다.
	//
	//
	//		※ 조회 시 자동등록한 실시간을 해제할 때도 호출함
	//
	//     pszCode		- "ChartIndex" or "ChartExcel" 
	//     lpData		- 각 TR의  OutBlock의 indexid
	//
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	// ★★★★★ 
	// ★★★★★  [4] ~ [5] e종목검색 조회 
	// ★★★★★ 
	// ★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★
	//
	//		실시간 종목검색은 등록 갯수에 제한이 있기때문에, 사용하지 않는 경우 해제하는 것이 좋습니다(기본 2개)
	//
	//
	//
	//     pszCode		- "t1857"
	//     lpData		- t1857 TR의 OutBlock의 AlertNum 값
	//
	int		RemoveService(LPCSTR pszCode, LPCSTR pszData);

	//------------------------------------------------------------------------------
	// API 에서 HTS로 연동 (API -> HTS) : 연동을 원할때마다 요청한다
	//     반환값       - 연동 성공 여부
	//     _hWnd			- 연동을 요청한 윈도우의 핸들
	//     
	// [1] 종목 연동
	//     pszLinkKey	- &STOCK_CODE				: 주식 종목코드
	//                    &ETF_CODE					: ETF 종목코드
	//                    &ELW_CODE					: ELW 종목코드
	//                    &KONEX_CODE				: 코넥스 종목코드
	//                    &FREEBOARD_CODE			: 프리보드 종목코드
	//                    &KSPI_CODE				: 코스피 업종 코드
	//                    &KSQI_CODE				: 코스닥 업종 코드
	//                    &FUTURE_CODE				: 선물종목코드
	//                    &OPTION_CODE				: 옵션종목코드
	//                    &FUTOPT_CODE				: 선물/옵션 종목코드 
	//                    &FUTSP_CODE				: 선물스프레드 종목코드
	//                    &STOCK_FUTURE_CODE		: 주식 선물 종목코드
	//                    &STOCK_OPTION_CODE		: 주식 옵션 종목코드
	//                    &STOCK_FUTOPT_CODE		: 주식 선물옵션 종목코드 
	//                    &STOCK_FUTSP_CODE			: 주식 선물스프레드 종목코드
	//                    &FUTOPT_STOCK_FUTOPT_CODE : 선물옵션 & 주식 선물옵션 종목코드
	//                    &US_CODE					: 해외종목코드
	//                    &COMMODITY_FUTOPT_CODE	: 상품선물/선물옵션
	//                    &COMMODITY_FUTURE_CODE	: 상품선물
	//                    &COMMODITY_STAR_CODE		: 스타선물
	//                    &CME_FUTURE_CODE			: CME야간선물
	//                    &EUREX_OPTION_CODE		: EUREX야간옵션
	//                    &NIGHT_FUTOPT_CODE		: 야간선물옵션
	//     pszData    	- 상품별 종목코드  
	//
	//     ex) 주식 종목 연동 : pszLinkKey = "&STOCK_CODE", pszData = "078020"
	//         선물 종목 연동 : pszLinkKey = "&FUTURE_CODE", pszData = "101HC000"
	//     
	// [2] HTS 화면 열기 
	//     pszLinkKey	- &OPEN_SCREEN : 화면 열기
	//     pszData    	- 열고 싶은 화면 번호
	//
	//     ex) HTS의 '[6602]선옵원장 미결제잔고' 열기
	//         pszLinkKey = &OPEN_SCREEN, pszData = "6602"	

	int		RequestLinkToHTS(LPCSTR pszLinkKey, LPCSTR pszData, LPCSTR pszFiller = NULL);

	//------------------------------------------------------------------------------
	// HTS 에서 API로 연동 (HTS -> API) : 등록하면 해제할때까지 연동된다
	//     _hWnd			- 연동을 원하는 윈도우의 핸들
	// 
	// ※ HTS에서 연동 발생 시, XM_RECEIVE_LINK_DATA 로 메시지가 발생
	//	  WPARAM = LINK_DATA, 
	//	  LPARAM = LINKDATA_RECV_MSG 구조체 데이터 
	//    ★★★  사용방식은 Real 수신과 동일, LPARAM 메모리 수신 후 반드시 해제 필요

	void	AdviseLinkFromHTS();
	void	UnAdviseLinkFromHTS();

	//------------------------------------------------------------------------------
	// 데이터의 압축을 해제한다 : 틱챠트 데이터 등에서 압축상태로 수신받은 경우 사용
	//     반환값       - 압축을 해제한 데이터(pszDest)의 길이
	//
	//     pszSrc		- 압축상태 데이터
	//     pszDest		- 압축을 해제한 데이터를 저장할 메모리 (Outblock 구조체 사이즈 최대 2000건)
	//	   nSrcLen	    - pszSrc 데이터의 길이
	// 
	// 사용 방법 
	//     ex) t8411 TR 이용시, InBlock의 comp_yn(압축여부) 필드에 "Y" 입력 후 조회
	//          ReceiveData() 에서 Occurs 블럭(t8411OutBlock1)이 압축되어 수신되므로, 해당 블럭 압축을 해제
	// 
	//			LRESULT t8411_Wnd::OnXMReceiveData( WPARAM wParam, LPARAM lParam )
	//			{
	//				//-------------------------------------------------------------------------------------
	//				// Data를 받음
	//				if( wParam == REQUEST_DATA )
	//				{
	//					LPRECV_PACKET pRpData = (LPRECV_PACKET)lParam;
	//
	//					if( strcmp( pRpData->szBlockName, NAME_t8411OutBlock ) == 0 )
	//					{
	//					}
	//					else if( strcmp( pRpData->szBlockName, NAME_t8411OutBlock1 ) == 0 )
	//					{
	//						LPt8411OutBlock1 pOutBlock1 = (LPt8411OutBlock1)pRpData->lpData;
	//
	//						t8411OutBlock1 szOutBlock1[2000];		// 압축 해제시 최대 2000건 수신
	//						int nDestSize = g_iXingAPI.Decompress((char *)pOutBlock1, (char *)&szOutBlock1[0], pRpData->nDataLength);
	//
	//						// Occurs 일 경우
	//						// Header가 'A' 이면 전체길이에서 OutBlock의 길이를 나눠서 갯수를 구한다.
	//						if (nDestSize > 0)
	//						{
	//							int nCount = nDestSize / sizeof( t8411OutBlock1 );
	//
	//							for( int i=0; i<nCount; i++ )
	//							{
	//								데이터 표시 
	//							}
	//						}
	//					}
	//				}
	//			}
	// 
	int		Decompress(LPCSTR pszSrc, LPCSTR pszDest, int nSrcLen);

	//------------------------------------------------------------------------------
};
