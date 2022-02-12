#pragma once


#include <Windows.h>

/*
//------------------------------------------------------------------------------
// 메시지 정의
// 메시지의 ID값은 Connect시에 설정한 nStartMsgID와 더하여 사용하면 된다.
#define		XM_DEFAULT_VALUE			WM_USER

#define 	XM_DISCONNECT				XM_DEFAULT_VALUE + 1		// 서버와의 연결이 끊어졌을 경우 발생
#define 	XM_RECEIVE_DATA				XM_DEFAULT_VALUE + 3		// RequestData로 요청한 데이터가 서버로부터 받았을 때 발생
#define 	XM_RECEIVE_REAL_DATA		XM_DEFAULT_VALUE + 4		// AdviseData로 요청한 데이터가 서버로부터 받았을 때 발생
#define 	XM_LOGIN					XM_DEFAULT_VALUE + 5		// 서버로부터 로그인 결과 받았을때 발생
#define 	XM_LOGOUT					XM_DEFAULT_VALUE + 6		// 서버로부터 로그아웃 결과 받았을때 발생
#define		XM_TIMEOUT_DATA				XM_DEFAULT_VALUE + 7		// RequestData로 요청한 데이터가 Timeout 이 발생했을때
#define 	XM_RECEIVE_LINK_DATA		XM_DEFAULT_VALUE + 8		// HTS 에서 연동 데이터가 발생했을 때	: by zzin 2013.11.11  
#define 	XM_RECEIVE_REAL_DATA_CHART	XM_DEFAULT_VALUE + 10		// 실시간 자동 등록한 후 차트 조회 시, 지표실시간 데이터를 받았을 때  : by zzin 2013.08.14  
#define 	XM_RECEIVE_REAL_DATA_SEARCH	XM_DEFAULT_VALUE + 11		// 종목검색 실시간 데이터를 받았을 때 			: by 2017.11.24 LSW  

#define		XM_MAX						XM_DEFAULT_VALUE + 12
*/
enum
{
	XM_DEFAULT_VALUE = WM_USER
	, XM_DISCONNECT

	, XM_RECEIVE_DATA = (WM_USER + 3)
	, XM_RECEIVE_REAL_DATA
	, XM_LOGIN
	, XM_LOGOUT
	, XM_TIMEOUT_DATA
	, XM_RECEIVE_LINK_DATA

	, XM_RECEIVE_REAL_DATA_CHART = (WM_USER + 10)
	, XM_RECEIVE_REAL_DATA_SEARCH

	, XM_MAX
};


//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Receive Flag
#define		REQUEST_DATA			1
#define		MESSAGE_DATA			2
#define		SYSTEM_ERROR_DATA		3
#define		RELEASE_DATA			4
#define		LINK_DATA				10			// XM_RECEIVE_LINK_DATA 메시지의 구분 플래그
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------


// Structure 정의
#pragma pack( push, 1 )

// 조회TR 수신 Packet
typedef struct _RECV_PACKET
{
	int					nRqID;						// Request ID
	int					nDataLength;				// 받은 데이터 크기
	int					nTotalDataBufferSize;		// lpData에 할당된 크기
	int					nElapsedTime;				// 전송에서 수신까지 걸린시간(1/1000초)
	int					nDataMode;					// 1:BLOCK MODE, 2:NON-BLOCK MODE
	char				szTrCode[10 + 1];			// AP Code
	char				cCont[1];					// '0' : 다음조회 없음, '1' : 다음조회 있음
	char				szContKey[18 + 1];			// 연속키, Data Header가 B 인 경우에만 사용
	char				szUserData[30 + 1];			// 사용자 데이터
	char				szBlockName[17];			// Block 명, Block Mode 일때 사용
	unsigned char* lpData;
} RECV_PACKET, * LPRECV_PACKET;

// 메시지 수신 Packet
typedef struct _MSG_PACKET
{
	int					nRqID;						// Request ID
	int					nIsSystemError;				// 0:일반메시지, 1:System Error 메시지
	char				szMsgCode[5 + 1];			// 메시지 코드
	int					nMsgLength;					// Message 길이
	unsigned char* lpszMessageData;					// Message Data
} MSG_PACKET, * LPMSG_PACKET;

// 실시간TR 수신 Packet
typedef struct _REAL_RECV_PACKET
{
	char				szTrCode[3 + 1];

	int					nKeyLength;
	char				szKeyData[32 + 1];
	char				szRegKey[32 + 1];

	int					nDataLength;
	char* pszData;
} RECV_REAL_PACKET, * LPRECV_REAL_PACKET;


// HTS에서 API로 연동되어 수신되는 Packet
typedef struct _LINKDATA_RECV_MSG
{
	char				sLinkName[32];
	char				sLinkData[32];
	char				sFiller[64];
}LINKDATA_RECV_MSG, * LPLINKDATA_RECV_MSG;
//------------------------------------------------------------------------------
#pragma pack( pop )


typedef BOOL(__stdcall *FP_CONNECT) (HWND, LPCSTR, int, int, int, int);
typedef BOOL(__stdcall *FP_ISCONNECTED) ();
typedef BOOL(__stdcall *FP_DISCONNECT) ();
typedef BOOL(__stdcall *FP_LOGIN) (HWND, LPCSTR, LPCSTR, LPCSTR, int, BOOL);
typedef BOOL(__stdcall *FP_LOGOUT) (HWND);

typedef int(__stdcall *FP_GETLASTERROR) ();
typedef int(__stdcall *FP_GETERRORMESSAGE) (int, LPSTR, int);

typedef int(__stdcall *FP_REQUEST) (HWND, LPCSTR, LPVOID, int, BOOL, LPCSTR, int);
typedef void(__stdcall *FP_RELEASEREQUESTDATA) (int);
typedef void(__stdcall *FP_RELEASEMESSAGEDATA) (LPARAM);

typedef BOOL(__stdcall *FP_ADVISEREALDATA) (HWND, LPCSTR, LPCSTR, int);
typedef BOOL(__stdcall *FP_UNADVISEREALDATA) (HWND, LPCSTR, LPCSTR, int);
typedef BOOL(__stdcall *FP_UNADVISEWINDOW) (HWND);

typedef int(__stdcall *FP_GETACCOUNTLISTCOUNT) ();
typedef BOOL(__stdcall *FP_GETACCOUNTLIST) (int, LPSTR, int);
typedef BOOL(__stdcall *FP_GETACCOUNTNAME) (LPCSTR, LPSTR, int);
typedef BOOL(__stdcall *FP_GETACCTDETAILNAME) (LPCSTR, LPSTR, int);
typedef BOOL(__stdcall *FP_GETACCTNICKNAME) (LPCSTR, LPSTR, int);

typedef void(__stdcall *FP_GETCOMMMEDIA) (LPSTR);
typedef void(__stdcall *FP_GETETKMEDIA) (LPSTR);
typedef void(__stdcall *FP_GETCLIENTIP) (LPSTR);
typedef void(__stdcall *FP_GETSERVERNAME) (LPSTR);
typedef void(__stdcall *FP_GETAPIPATH) (LPSTR);

typedef void(__stdcall *FP_SETHEADERINFO) (LPCSTR, LPCSTR);
typedef void(__stdcall *FP_SETUSEAPIVER) (LPCSTR);
typedef void(__stdcall *FP_SETMODE) (LPCSTR, LPCSTR);

typedef void(__stdcall *FP_GETPROCBRANCHNO) (LPSTR);
typedef BOOL(__stdcall *FP_GETUSEOVERFUTURE) ();
typedef BOOL(__stdcall *FP_GETUSEFX) ();

typedef int(__stdcall *FP_GETTRCOUNTPERSEC) (LPCSTR);
typedef int(__stdcall *FP_GETTRCOUNTBASESEC) (LPCSTR);
typedef int(__stdcall *FP_GETTRCOUNTREQUEST) (LPCSTR);
typedef int(__stdcall *FP_GETTRCOUNTLIMIT) (LPCSTR);

typedef void(__stdcall *FP_SETNOTIFYFLAG) (BOOL);

typedef int(__stdcall *FP_REQUESTSERVICE) (HWND, LPCSTR, LPCSTR);
typedef int(__stdcall *FP_REMOVESERVICE) (HWND, LPCSTR, LPCSTR);

typedef int(__stdcall *FP_REQUESTLINKTOHTS) (HWND, LPCSTR, LPCSTR, LPCSTR);
typedef void(__stdcall *FP_ADVISELINKFROMHTS) (HWND);
typedef void(__stdcall *FP_UNADVISELINKFROMHTS) (HWND);

typedef int(__stdcall *FP_DECOMPRESS) (LPCSTR, LPCSTR, int);
typedef BOOL(__stdcall *FP_ISCHARTLIB) ();

typedef void(__stdcall *FP_SETPROGRAMORDER) (BOOL);
typedef BOOL(__stdcall *FP_GETPROGRAMORDER) ();


#pragma pack(push, 1)
typedef struct _S3_InBlock
{
	char shcode[6];
} _K3_InBlock;

typedef struct _체결_데이터_이베스트
{
	char 체결시간[6];
	char 전일대비구분[1];
	char 전일대비[8];
	char 등락율[6];
	char 현재가[8];
	char 시가시간[6];
	char 시가[8];
	char 고가시간[6];
	char 고가[8];
	char 저가시간[6];
	char 저가[8];
	char 체결구분[1];
	char 체결량[8];
	char 누적거래량[12];
	char 누적거래대금[12];
	char 매도누적체결량[12];
	char 매도누적체결건수[8];
	char 매수누적체결량[12];
	char 매수누적체결건수[8];
	char 체결강도[9];
	char 가중평균가[8];
	char 매도호가[8];
	char 매수호가[8];
	char 장정보[2];
	char 전일동시간대거래량[12];
	char 단축코드[6];
} 체결_데이터_이베스트, * 체결_데이터_이베스트포;
#pragma pack(pop)