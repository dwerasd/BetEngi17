#pragma once


#include <DarkCore/DMemory.h>
#include <DarkCore/DSocket.h>

#include <DNetwork/NetworkDef.h>



#if __cplusplus > 201703L
enum
#else
enum _PKT_INDEX_
#endif
{
	_PKT_PACKET_INDEX_NONE_ = 0
	, _PKT_STOCK_PACKET_INDEX_NONE_

	, _PKT_PIPE_DESTROY_							// 증권사 중계기 프로그램 종료
	, _PKT_PIPE_CONNECTED_
	, _PKT_PIPE_DISCONNECTED_
	// 여긴 크레온
	, _PKT_PIPE_CREON_MIN_

	, _PKT_PIPE_CONNECTED_CREON_
	, _PKT_PIPE_DISCONNECTED_CREON_
	, _PKT_BRIDGE_REQUEST_INIT_CREON_								// 장 시작시간, 종료시간, 종목리스트를 가져온다.
	, _PKT_BRIDGE_SUCCESSED_INIT_CREON_						// 장 시작시간, 종료시간 날라옴.
	, _PKT_BRIDGE_FAILED_INIT_CREON_						//
	, _PKT_BRIDGE_REQUEST_STOCK_INFO_CREON_						// 종목 요청함.
	, _PKT_BRIDGE_RECEIVE_STOCK_INFO_CREON_						// STOCK_INFO 날라옴.
	, _PKT_BRIDGE_SUCCEEDED_STOCK_INFO_CREON_					// 종목 다 받음.

	, _PKT_PIPE_GET_STOCK_OHLCV_						// 해당 종목의 과거 데이터 요청.
	, _PKT_PIPE_INFO_STOCK_OHLCV_						// 과거 데이터 날라옴.

	, _PKT_PIPE_CREON_MAX_
	// 여긴 키움
	, _PKT_PIPE_KIWOOM_MIN_

	, _브릿지패킷_키움_클라이언트_접속_
	, _브릿지패킷_키움_클라이언트_접속해제_
	, _브릿지패킷_키움_로그인요청_						// 초기화 요청.
	, _브릿지패킷_키움_로그인성공_						// 초기화 완료 날라옴.
	, _브릿지패킷_키움_초기화실패_						// 초기화 실패 날라옴.
	// 키움 조건식
	, _브릿지패킷_키움_조건식요청_						// 조건식을 요청한다. HTS에서 조건식을 수정했을 경우 다시 호출해야 API에 반영된다.
	, _브릿지패킷_키움_조건식성공_					// 조건식을 얻어옴.
	, _브릿지패킷_키움_조건식실패_						// 조건식을 얻어오지 못함.
	, _브릿지패킷_키움_조건검색받음_						// 조건검색 데이터 받음.
	, _PKT_PIPE_REALTIME_CONDITION_						// 실시간 조건검색 데이터 받음.

	, _PKT_PIPE_KIWOOM_REQUEST_STOCK_INFO_					// STOCK_INFO 리스트 요청.
	, _PKT_PIPE_KIWOOM_RECEIVE_STOCK_INFO_						// STOCK_INFO 날라옴.
	, _PKT_PIPE_KIWOOM_SUCCEEDED_STOCK_INFO_					// STOCK_INFO 다 날라옴.

	, _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_					// 키움 실시간 체결 데이터 요청
	, _PKT_PIPE_KIWOOM_RECEIVE_TRANSACTION_			// 키움 실시간 체결 데이터 날라옴
	, _브릿지패킷_키움_주식호가잔량_						// 키움 실시간 체결 데이터 날라옴
	, _브릿지패킷_키움_장시작시간_						// 키움 실시간 체결 데이터 날라옴

	, _PKT_PIPE_RECIVE_REALTIME_REMAIN_DATA_			// 실시간 호가창 데이터 요청
	, _브릿지패킷_키움_예수금상세현황요청_					// 예수금상세현황요청
	, _브릿지패킷_키움_예수금상세현황받음_					// 예수금상세현황받음.

	, _PKT_PIPE_KIWOOM_MAX_
	// 이제 이베스트
	, _PKT_PIPE_XINGAPI_MIN_

	, _PKT_PIPE_CONNECTED_XINGAPI_
	, _PKT_PIPE_DISCONNECTED_XINGAPI_
	, _PKT_PIPE_INIT_XINGAPI_
	, _PKT_PIPE_INIT_COMPLETED_
	, _PKT_PIPE_INIT_FAILED_
	, _PKT_PIPE_INFO_STOCKS_XINGAPI_					// STOCK_INFO 날라옴.
	, _PKT_PIPE_INIT_FINISH_XINGAPI_					// 초기화 완료 날라옴.

	, _PKT_PIPE_REALTIME_DATA_XINGAPI_					// 이베스트 해당 종목의 실시간 데이터를 요청.
	, _PKT_PIPE_RECEIVE_REALTIME_DATA_					// 실시간 데이터를 받음.
	, _PKT_PIPE_RECEIVE_TR_DATA_						// tr 데이터를 받음. (RequestData 요청에 대한 응답)

	, _PKT_PIPE_XINGAPI_MAX_
	//, _PKT_PIPE_REQUEST_LOGIN_						// 증권사 로그인 요청 서버에서 보낸다.
	//, _PKT_PIPE_RESULT_LOGIN_SUCCEED_					// 증권사 로그인 성공
	//, _PKT_PIPE_RESULT_LOGIN_FAILED_					// 증권사 로그인 실패

	, _PKT_PIPE_KOREA_MIN_

	, _PKT_PIPE_KOREA_MAX_

	, _PKT_STOCK_PACKET_INDEX_MAX_

	// 여긴 네트워크 TCP 패킷
	, _PKT_NET_PACKET_NONE_ = 1000
	, _PKT_NET_CONNECTED_								// 접속 직후 보냄
	, _PKT_NET_DISCONNECTED_							// 프로그램이 종료될 때 보냄.
	, _PKT_NET_RECEIVE_STOCKINFO_KIWOOM_				// 키움 종목 패킷이다.
	, _네트워크패킷_키움_주식체결_						// 키움 체결 데이터다
	, _네트워크패킷_키움_주식호가잔량_					// 키움 체결 데이터다
	, _PKT_NET_RECEIVE_TRANSACTION_EBEST_				// 이베스트 체결데이터다.
	, _PKT_NET_REQUEST_LOGIN_							// 유저 로그인
	, _PKT_NET_RESULT_LOGIN_SUCCEED_					// 로그인 성공
	, _PKT_NET_RESULT_LOGIN_FAILED_						// 로그인 실패

	, _PKT_NET_GET_ALL_STOCKS_LIST_						// 모든 STOCK_INFO 리스트를 얻어온다. (백테스트용)
	// 프로그램이 연결되면 자동으로 종목리스트를 얻어와서 서버에 모인다.
	// 각 증권사의 종목들을 하나의 리스트로 만들고 걸러낼 종목을 디비에 저장한다.
	// 그렇게 걸러내진 종목을 클라이언트가 받는거다.
	, _PKT_NET_GET_CLEAN_STOCKS_LIST_					// 걸러진 STOCK_INFO 리스트를 얻어온다, (실시간 감시용)
	// 장 시작전 준비.
	, _PKT_NET_LOAD_STOCK_DATA_							// 걸러진 종목들의 과거 데이터를 로드하고, 각 봉의 보조지표를 계산한다.
	// 장 끝나고 백업, 크레온의 모든 종목수만큼 반복.
	, _PKT_NET_FILL_STOCK_DATA_							// 크레온에 해당 종목의 1분봉, 45분봉, 135분봉, 1일봉, 1주봉, 1월봉을 얻어와서 채우고 기록한다


	// 관리자 프로그램에서만 사용한다.
	, _PKT_NET_EXECUTE_
	, _PKT_NET_RUNNING_									// 프로그램이 연결되면 자동으로 종목리스트를 얻어와서 서버로 보냄.
	, _PKT_NET_PACKET_MAX_

	, _PKT_PACKET_INDEX_MAX_
};

enum
{
	_키움_예외처리_ = 0
	, _키움_주식시세_
	, _키움_주식체결_
	, _키움_주식우선호가_
	, _키움_주식호가잔량_
	, _키움_주식시간외호가_
	, _키움_장시작시간_
};

// 이 아래는 바이트 단위로 통신하는 패킷이다.
#pragma pack(push,1)
typedef struct _PIPE_GET_STOCK_OHLCV
{
	ULONG nCountSticks;				// 봉 갯수 ex) 99999999 
	USHORT nLength;					// [1]: 1분봉, [15]: 15분봉
	char cType;						// [D]: 일봉, [W]: 주봉, [M]: 월봉, [m]: 분봉, [T]: 틱
	char cModifyPrice;				// [0]: 수정주가 적용안함, [1]: 수정주가 적용
	char cVolumeType;				// [1]: 시간외거래량포함, [2]: 장종료시간외거래량만포함, [3]: 시간외거래량모두제외, [4]: 장전시간외거래량만포함
	char szCodeCreon[(1 << 3)];
} PIPE_GET_STOCK_OHLCV, *LPPIPE_GET_STOCK_OHLCV;

typedef struct _NET_MESSAGE_HEADER
{
	BYTE bytClientType;			// [0]: 유저 프로그램, [1]: 중계기, [2]: 관리자
	BYTE bytLocale;				// [0]: 멀티바이트, [1]: 유니코드. [2]: UTF-8
	WORD nClientVersion;		// 
} NET_MESSAGE_HEADER, *LPNET_MESSAGE_HEADER;

typedef struct _NET_MESSAGE_LOGIN
	: public NET_MESSAGE_HEADER
{
	char szAccount[(1 << 5)];
	wchar_t wszAccount[(1 << 5)];
	char szPassword[(1 << 5)];
	wchar_t wszPassword[(1 << 5)];
	void clear()
	{
		ZeroMemory(this, sizeof(_NET_MESSAGE_LOGIN));
	}
} NET_MESSAGE_LOGIN, *LPNET_MESSAGE_LOGIN;

typedef struct _NET_MESSAGE_EXECUTE_API
	: public NET_MESSAGE_HEADER
{
	BYTE nAPIType;
	void clear()
	{
		ZeroMemory(this, sizeof(_NET_MESSAGE_EXECUTE_API));
	}
} NET_MESSAGE_EXECUTE_API, *LPNET_MESSAGE_EXECUTE_API;

typedef struct _NET_PACKET_HEADER
{
	WORD nPacketSize;						// 패킷 사이즈 일단 큐로 쓰고 링버퍼는 나중에?
} NET_PACKET_HEADER, * LPNET_PACKET_HEADER;



typedef struct _PACKET_BUNDLE		// 패킷 뭉치
{
	// 최대 1024
	DWORD	dwSize1;				// 패킷 끝에도 사이즈를 박는다
	DWORD	dwSignature;
	WORD	nEncKey;				// 암호화 키
	WORD	nEncSize;				// 암호화 사이즈
	char	szBuffer[_MAX_PACKET_SIZE_ - _ALIGNMENT_];
	DWORD	dwSize2;

	void clear()
	{
		ZeroMemory(this, sizeof(_PACKET_BUNDLE));
	}
} PACKET_BUNDLE, *LPPACKET_BUNDLE;

typedef struct _PACKET_BUNCH		// 패킷 조각
{
	DWORD nTotalSize;
	BYTE bytBuffer[(1 << 13) - _ALIGNMENT_];	// 현재는 -12, -52까지 여유가 있다.

	DWORD nCurrentSize;
	DWORD nCurrentPos;
	DWORD bHeader;

	void clear()
	{
		ZeroMemory(this, sizeof(_PACKET_BUNCH));
	}
} PACKET_BUNCH, *LPPACKET_BUNCH;


// 로그인정보
typedef struct _LOGIN_INFO_KIWOOM
{	// 접속이 되면 작성해서 BetMain 으로 보낸다.
	bool 모의서버{ false };	// [0]: 실서버, [1]: 모의서버
	BYTE 계좌갯수{ 0 };
	char 아이디[(1 << 4)] = { 0 };
	char 사용자명[(1 << 5)] = { 0 };
	char 계좌번호[10][20] = { 0 };	// 최대20자, 계좌는 10개까지.
} LOGIN_INFO_KIWOOM, * LPLOGIN_INFO_KIWOOM;
// 종목정보

// 예수금상세현황요청
typedef struct _REQUEST_TR_OPW00001
{
	char 조회구분[2] = { 0 };
	char szScrNum[5] = { 0 };
	char 계좌번호[20] = { 0 };
	//char 계좌비밀번호[5] = { 0 };
} REQUEST_TR_OPW00001, *LPREQUEST_TR_OPW00001;

typedef struct _STOCK_INFO_CREON
{
	BYTE 장구분;
	BYTE nKseSectionKind;
	BYTE 상태;				// [0]: 정상, [1]: 거래정지, [2]: 거래중단
	BYTE nSupervisionType;
	BYTE nControlType;
	BYTE 스팩주;			// 스팩
	WORD nDummy;
	long 전일종가;		// 전일종가

	char 크레온코드[(1 << 3)];
	char 종목코드[(1 << 3)];
	char 풀코드[(1 << 4)];
	char 종목명[(1 << 6)];

	void clear()
	{
		ZeroMemory(this, sizeof(_STOCK_INFO_CREON));
	}
} STOCK_INFO_CREON, *LPSTOCK_INFO_CREON;

typedef struct _STOCK_INFO_KIWOOM
{
	bool 거래정지;		// 거래정지
	bool 관리종목;		// 관리종목
	bool 담보대출;	// 담보대출
	bool 신용가능;	// 신용가능
	bool bELW;			// ELW
	bool 뮤추얼펀드;		// 뮤추얼 펀드	( 구려 )
	bool 신주인수권;		// 신주인수권	( 작전주 같음 )
	bool 리츠;		// 리츠			( 좋아 )
	bool bETF;			// ETF			( 코스피 코스닥에 많음 )
	BYTE 감리구분;	// [0]: 정상, [1]: 투자주의, [2]: 투자경고, [3]: 투자위험, [4]: 투자주의환기종목
	WORD 증거금;		// 증거금
	long nPreClose;		// 전일종가		( 모두 있음 )

	bool 하이일드펀드;			// ETF			( 코스피 코스닥에 많음 )
	bool K_OTC;			// ETF			( 코스피 코스닥에 많음 )
	bool 코넥스;			// ETF			( 코스피 코스닥에 많음 )

	char szStockCode[(1 << 3)];
	char szStockName[(1 << 6)];

	void clear() { ZeroMemory(this, sizeof(_STOCK_INFO_KIWOOM)); }
} STOCK_INFO_KIWOOM, *LPSTOCK_INFO_KIWOOM;

typedef struct _REQUEST_REALTIME_DATA
{
	BYTE nStockCount{ 0 };
	char szScr[(1 << 3)];			// szScrNum
	char szStocks[(1 << 10)];		// ';'로 구분된 종목코드, 화면당 최대 100개
	void clear() { ZeroMemory(this, sizeof(_REQUEST_REALTIME_DATA)); }
} REQUEST_REALTIME_DATA, *LPREQUEST_REALTIME_DATA;

typedef struct _예수금요청
{
	BYTE nStockCount{ 0 };
	char szScr[(1 << 3)];			// szScrNum
	char szStocks[(1 << 10)];		// ';'로 구분된 종목코드, 화면당 최대 100개
	void clear() { ZeroMemory(this, sizeof(_예수금요청)); }
} 예수금요청, * 예수금요청포;

#pragma pack(pop)

enum
{
	_KIWOOM_MAIN_x86_ = 1		// 키움에 연결되는 mfc 프로그램
	, _KIWOOM_MAIN_x64_			// mfc 프로그램이랑 연결되는 64비트 프로그램
	, _EBEST_MAIN_x86_
	, _EBEST_MAIN_x64_
	, _DAESHIN_MAIN_x86_
	, _DAESHIN_MAIN_x64_
};

enum
{
	_NET_REQUEST_LOGIN_ = 100
	, _NET_RECIVE_LOGIN_ = 200

};

typedef struct _PKT_NET_LOGIN
{
	DWORD dwHeader;
	DWORD dwClientType;
	char szUserID[1 << 5];
	char szUserPW[1 << 5];
} PKT_NET_LOGIN, *LPPKT_NET_LOGIN;

enum
{
	_NET_MSG_KIWOOM_LOGIN_SUCCEED_
	, _NET_MSG_KIWOOM_LOGIN_FAILED_

	, _NET_MSG_KIWOOM_RECIVE_REALTIME_DATA_
	, _NET_MSG_KIWOOM_RECIVE_REALTIME_REMAIN_DATA_

	, _NET_MSG_KIWOOM_RECIVE_GET_STOCK_
	, _NET_MSG_KIWOOM_END_GET_STOCK_

	, _NET_MSG_KIWOOM_RECIVE_CONDITION_SUCCEED_
	, _NET_MSG_KIWOOM_RECIVE_CONDITION_FAILED_

	, _NET_MSG_KIWOOM_RECIVE_TR_CONDITION_
	, _NET_MSG_KIWOOM_RECIVE_REALTIME_CONDITION_
	,
};

typedef struct _STOCK_BASE_INFO
{
	char szStockCode[(1 << 4)];		// 길어야 6자지만 혹시 모르니
	char szStockName[(1 << 6)];		// 모르니까 64
	char szStockState[(1 << 6)];	// 모르니까 64
	char szControlKind[(1 << 5)];	// "투자주의환기종목" 유니코드에선 16바이트로 에러 안났는데, 버그였나보다.
	void clear() { ZeroMemory(this, sizeof(_STOCK_BASE_INFO)); }
} STOCK_BASE_INFO, *LPSTOCK_BASE_INFO;

typedef struct _KIWOOM_REALDATA_HEADER
{
	BYTE nRealType;						// 주식시세, 주식체결 등
	char 종목코드[(1 << 3)];			// 종목코드
} KIWOOM_REALDATA_HEADER, *LPKIWOOM_REALDATA_HEADER;

typedef struct _KIWOOM_REALDATA_TRANSACTION
	: public _KIWOOM_REALDATA_HEADER
{
	char 체결시간[(1 << 5)];			// 091259
	char szOpen[(1 << 4)];				// 시 10000000
	char szHigh[(1 << 4)];				// 고 10000000
	char szLow[(1 << 4)];				// 저 10000000
	char 체결가[(1 << 4)];				// 종 10000000
	char 최우선매도호가[(1 << 4)];		// 최우선매도호가
	char 최우선매수호가[(1 << 4)];		// 최우선매수호가
	char 체결량[(1 << 4)];			// 거래량 9999999999999
	char 누적거래량[(1 << 4)];		// 누적거래량 9999999999999
	char 누적거래대금[(1 << 4)];		// 누적거래대금 9999999999999
	char 체결강도[(1 << 4)];			// 체결강도
	char 등락율[(1 << 4)];
	char 전일거래량대비[(1 << 4)];		// 전일거래량대비(비율)
	char 거래회전율[(1 << 4)];
	char 시가총액[(1 << 4)];
	char 전일동시간거래량비율[(1 << 4)];
	void clear() { ::memset(this, 0, sizeof(_KIWOOM_REALDATA_TRANSACTION)); }
} KIWOOM_REALDATA_TRANSACTION, *LPKIWOOM_REALDATA_TRANSACTION;

typedef struct _키움_예수금상세현황
{
	ULONG64 예수금{ 0 };
	ULONG64 출금가능금액;
	ULONG64 주문가능금액;
} 키움_예수금상세현황, * 키움_예수금상세현황포;

typedef struct _키움_주식호가잔량
	: public _KIWOOM_REALDATA_HEADER
{
	char 호가시간[(1 << 5)];				// 091259
	char 매도호가총잔량[(1 << 4)];
	char 매수호가총잔량[(1 << 4)];
	char 매도비율[(1 << 4)];
} 키움_주식호가잔량, * 키움_주식호가잔량포;

typedef struct _ORDERBOOK_KIWOOM
{
	char 종목코드[(1 << 3)] = { 0 };				// 091259
	ULONG 매도호가총잔량{ 0 };
	ULONG 매수호가총잔량{ 0 };
	float 매도비율{ 0.0f };
} ORDERBOOK_KIWOOM, *LPORDERBOOK_KIWOOM;

typedef struct _키움_장시작시간
	: public _KIWOOM_REALDATA_HEADER
{
	char 체결시간[(1 << 5)];					// 091259
	char 장운영구분[(1 << 5)];					// 091259
	char 장시작예상잔여시간[(1 << 5)];			// 091259
} 키움_장시작시간, * 키움_장시작시간포;

typedef struct _RECIVE_REALTIME_REMAIN_DATA
{
	char szMessageType[(1 << 5)];
	char szStockCode[(1 << 4)];
	char szTime[(1 << 5)];
	char szWaitingVolumeTotalSell[(1 << 5)];
	char szWaitingVolumeTotalBuy[(1 << 5)];
} RECIVE_REALTIME_REMAIN_DATA, *LPRECIVE_REALTIME_REMAIN_DATA;

typedef struct _RECIVE_TR_CONDITION_DATA
{
	int nConditionIndex;
	int nCountNextData;
	char szScreenNumber[(1 << 5)];
	char szConditionName[(1 << 5)];
	char szCodeList[(1 << 5)];
} RECIVE_TR_CONDITION_DATA, *LPRECIVE_TR_CONDITION_DATA;

typedef struct _RECIVE_REALTIME_CONDITION_DATA
{
	BYTE bInput;
	int nConditionIndex;
	char szStockCode[(1 << 5)];
	char szConditionName[(1 << 5)];
} RECIVE_REALTIME_CONDITION_DATA, *LPRECIVE_REALTIME_CONDITION_DATA;

enum
{
	_이베_예외처리_ = 0
	, _이베_주식체결_
	, _이베_주식호가잔량_
	, _이베_주식시세_
	, _이베_주식우선호가_
	, _이베_주식시간외호가_
	, _이베_장시작시간_
	, _이베_만들어야함_
};

typedef struct _요청_실시간_체결_이베스트
{
	BYTE 장구분{ 0 };
	char 종목코드[(1 << 3)] = { 0 };
} 요청_실시간_체결_이베스트, * 요청_실시간_체결_이베스트포;

typedef struct _실시간_등록_이베스트
	: public _요청_실시간_체결_이베스트
{
	bool 등록여부{ false };

	void 초기화() { ZeroMemory(this, sizeof(_실시간_등록_이베스트)); }
} 실시간_등록_이베스트, * 실시간_등록_이베스트포;

typedef struct _실시간_데이터_이베스트
{
	BYTE 실시간타입{ 0 };
	char 데이터[(1 << 12)];
} 실시간_데이터_이베스트, * 실시간_데이터_이베스트포;
//#pragma pack(pop)