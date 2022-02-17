#pragma once


#define _WINSOCKAPI_
#include <tchar.h>
#include <Windows.h>

#include <deque>
#include <vector>
#include <unordered_map>



#if (_MSC_VER > 1600) && (__cplusplus >= 201103L)
constexpr size_t _MAX_STOCK_ITEMS_ = 3000;				// 주식 STOCK_INFO 총 갯수, 키움이 3550개가 넘기 때문에 4천개로 한다
constexpr size_t _MAX_STICK_ITEMS_MIN_ = 2000000;
constexpr WORD _BB_LENGTH_DEFAULT_ = 20;
constexpr WORD _DB_LENGTH_DEFAULT_ = 22;
constexpr WORD _DAY_LENGTH_ = 381;
constexpr WORD _MAX_ARRAY_LENGTH_ = 1440;
//constexpr WORD _MAX_ARRAY_BUFFER_SIZE_ = (_MAX_ARRAY_LENGTH_ + (_DAY_LENGTH_ * 2));	// 2202 인데 줄이자.
//constexpr WORD _MAX_ARRAY_BUFFER_SIZE_ = DMIN((_MAX_ARRAY_LENGTH_ / 2), (_DAY_LENGTH_ * 2));	// 720, 762
constexpr WORD _MAX_ARRAY_BUFFER_SIZE_ = 500;
constexpr BYTE _보조지표버퍼수_ = 8;
constexpr WORD _MAX_KIWOOM_SCREEN_ = 2000;
#else
#define _MAX_STOCK_ITEMS_			3000
#define _MAX_STICK_ITEMS_MIN_		2000000
#define _BB_LENGTH_DEFAULT_			20
#define _DAY_LENGTH_				381
#define _MAX_ARRAY_LENGTH_			1440
#define _MAX_ARRAY_BUFFER_SIZE_		(_MAX_ARRAY_LENGTH_ + (_DAY_LENGTH_ * 2))
#define _보조지표버퍼수_			8
#define _MAX_KIWOOM_SCREEN_	(WORD)2000
#endif

typedef struct _KIWOOM_SCREEN
{
	BYTE nCount;						// 한 화면당 100개가 최고.
	char szScrNum[(1 << 3)];
	void Init()
	{
		ZeroMemory(this, sizeof(_KIWOOM_SCREEN));
	}
	_KIWOOM_SCREEN()
		: nCount(0)
	{
		ZeroMemory(&szScrNum, _countof(szScrNum));
	}
} KIWOOM_SCREEN, *LPKIWOOM_SCREEN;


enum
{
	_TICK_CODE_				// 각 STOCK_INFO 폴더에 하루치 몰아서 저장.		"data_stocks/005930/Tick/20201215.dat" 
	, _STICK_MINUTE_1_		// 각 STOCK_INFO 폴더에 1분봉 폴더에 하루치 저장. "data_stocks/005930/Minute1/20201215.dat" 
	, _STICK_MINUTE_3_		// 
	, _STICK_MINUTE_5_
	, _STICK_MINUTE_10_
	, _STICK_MINUTE_15_		// 각 STOCK_INFO 폴더에 15분봉 폴더에 하루치 저장. "data_stocks/005930/Minute15/20201215.dat" 
	, _STICK_MINUTE_30_
	, _STICK_MINUTE_45_
	, _STICK_MINUTE_DAY_
};

#pragma pack(push, 1)    // 1바이트 크기로 정렬

//typedef struct _TICK_DATA
//{
//	ULONG nSequence;			// 받은 순서 ( 키움으로부터 )
//	char 종목코드[7];					// "005930"
//	ULONG nTime;				// 체결시간, 파일 이름이 11시59분31초, 여기 뒤에 밀리초 3자리 붙인다.
//	BYTE nTransType;			// 1 = 매수, 2 = 매도, 3 = 단일가매매(동시호가)
//	ULONG nPrice;				// 체결가격
//	ULONG nTransVolume;			// 체결량
//} TICK_DATA, *LPTICK_DATA;

typedef struct _REMAIN_VOLUME_DATA
{
	ULONG nSequence;
	char 종목코드[8];						// 마지막 1바이트는 더미
	ULONG nTime;
	ULONG nRemainVolumeBuy;
	ULONG nRemainVolumeSell;

} REMAIN_VOLUME_DATA, *LPREMAIN_VOLUME_DATA;
// 이건 틱데이터, 들어오는대로 작업 큐와 파일매니저 큐에 넣는다
typedef struct _TRANSFER_DATA
{
	ULONG nSequence;		// 받은 순서 ( 키움에서 준 + 키움으로부터 받은 밀리초 )
	//ULONG nMilliSecond;	// 받은 시간 ( 키움으로부터 ) 11시59분31초593밀리초
	ULONG nTransType;		// 1 = 매수, 2 = 매도, 3 = 단일가매매(동시호가)
	ULONG nTime;			// 체결시간, 파일 이름이 11시59분31초
	ULONG nPrice;			// 체결가격
	ULONG nTransVolume;		// 체결량
} TRANSFER_DATA, *LPTRANSFER_DATA;
// 이건 1분봉 데이터
typedef struct _STICK_DATA
{	// 1분봉의 경우 시작과 끝의 날짜 시간이 동일하다.
	ULONG nStartDate;						// 이전 데이터 가져와서 박아넣을꺼라 날짜가 필요함.
	ULONG nStartTime;						// 0900~1530, 초를 잘라서 이렇게 저장된다.
	//ULONG nDataType;						// [1]: 1분봉, [2]: 1일봉,
	ULONG nEndDate;							// 분봉 일봉 날짜 다 같을테니 주석한다
	ULONG nEndTime;							// 일봉은 1530 이고, 표시하면 될텐데 굳이 필요할까?

	ULONG nOpen;							// 시작가
	ULONG nHigh;							// 고가
	ULONG nLow;								// 저가
	ULONG nClose;							// 종가
	ULONG nAccrueVolumeTotal;				// 누적거래량 (매수매도 + 장시작 단일가 및 장마감 동시호가 포함)
	//ULONG nDummy1;							// 더미
	// 원래대로라면 여기까지 40바이트, - 4 해서 36 바이트
	ULONG nRemainVolumeTotalBuy;			// 매수 호가 잔량
	ULONG nRemainVolumeTotalSell;			// 매도 호가 잔량
	//ULONG nAccrueNumberofTimes;			// 누적 거래의 횟수
	ULONG nAccrueNumberofTimesBuy;			// 누적 매수체결 횟수
	ULONG nAccrueNumberofTimesSell;			// 누적 매도체결 횟수
	ULONG 누적매수체결량;					// 누적매수거래량, 체결 데이터에서 얻을 수 있다.
	ULONG 누적매도체결량;				// 누적매도거래량, 체결 데이터에서 얻을 수 있다.
	//ULONG nPrevDayClose;					// 전일종가
	__int64 nAccruePaymentTotal;					// 누적 거래대금(매수매도 + 장시작 단일가 및 장마감 동시호가 포함, 천원 미만은 자른다.)
	// 원래대로라면 총거래대금 + 8바이트 해서 80 바이트, -8 더해서 36+32 = 68 바이트
	// 여기에 내 데이터(24바이트)를 추가하면 92 바이트
	long nAveragePaymentBuy;				// 평균 매수 거래가
	long nAveragePaymentSell;				// 평균 매도 거래가
	__int64 nAccruePaymentBuy;						// 누적 매수거래대금
	__int64 nAccruePaymentSell;						// 누적 매도거래대금

	//long nAccrueVolumeBuySell;						// 순매수량은 매수거래량 - 매도거래량 이므로 저장 하지 않는다.
	//__int64 nAccruePaymentBuySell;					// 순매수대금은 매수대금 - 매도대금 이므로 저장 하지 않는다.

	_STICK_DATA()
		: nStartDate(0)								//
		, nStartTime(0)								// 처음이라 시간을 넣는데 0900~1530 이 들어온다.
		, nEndDate(0)								//
		, nEndTime(0)								// 

		, nOpen(0)									// 처음이라 체결가격이 시작가격
		, nHigh(0)									// 처음이라 고가도
		, nLow(0)									// 저가도 없다
		, nClose(0)									// 체결가만 있을뿐
		, nAccrueVolumeTotal(0)						// 첫거래량은 갱신

		, nAccrueNumberofTimesBuy(0)
		, nAccrueNumberofTimesSell(0)

		, 누적매수체결량(0)
		, 누적매도체결량(0)

		, nAccruePaymentTotal(0)

		, nAveragePaymentBuy(0)
		, nAveragePaymentSell(0)

		, nAccruePaymentBuy(0)
		, nAccruePaymentSell(0)

	{
	}
	_STICK_DATA(ULONG _nClose, ULONG _nStartTime, ULONG _nStartDate, ULONG _nEndTime = 0, ULONG _nEndDate = 0)
		: nStartDate(_nStartDate)							// 오늘 날짜를 넣어주도록 하자.
		, nStartTime(_nStartTime)							// 일봉의 경우 090000000
		, nEndDate(_nEndDate ? _nEndDate : _nStartDate)		// 안넣으면 오늘 일봉까지는 당일인거다.
		, nEndTime(_nEndTime ? _nEndTime : _nStartTime)		// 일봉의 경우 15300000, 없으면 1분봉인거다.
		, nOpen(_nClose)									// 처음이라 체결가격이 시작가격
		, nHigh(_nClose)									// 처음이라 고가도
		, nLow(_nClose)										// 저가도 없다
		, nClose(_nClose)									// 체결가만 있을뿐
		, nAccrueVolumeTotal(0)								// 첫거래량은 갱신

		, nAccrueNumberofTimesBuy(0)
		, nAccrueNumberofTimesSell(0)

		, 누적매수체결량(0)
		, 누적매도체결량(0)
		, nAveragePaymentBuy(0)
		, nAveragePaymentSell(0)

		, nAccruePaymentBuy(0)
		, nAccruePaymentSell(0)
		, nAccruePaymentTotal(0)
	{
	}
	void Update(ULONG _nClose)
	{
		if (_nClose > nHigh) { nHigh = _nClose; }	// 이전에 저장된 가격보다 높으면 갱신
		if (_nClose < nLow) { nLow = _nClose; }		// 이전에 저장된 가격보다 낮으면 갱신
		nClose = _nClose;							// 체결가는 갱신 반복
	}
} STICK_DATA, *LPSTICK_DATA;

typedef struct _CANDLE_DATA_TYPE1
{
	int nDateTime;					// 이게 일봉 저장할때는 Date, 분봉은 Time
	int nOpen;
	int nHigh;
	int nLow;
	int nClose;
	ULONG nTotalVolume;				// 전체 거래량
} CANDLE_DATA_TYPE1, *LPCANDLE_DATA_TYPE1;

typedef struct _CANDLE_DATA_TYPE2
{
	ULONG nDate;								// 날짜
	ULONG nTime;								// 시간
	ULONG nOpen;
	ULONG nHigh;
	ULONG nLow;
	ULONG nClose;
	ULONG nTotalVolume;						// 거래량
	__int64 nTransferPayment;				// 거래대금
	ULONG nRemainVolumeTotalBuy;			// 매수 호가 잔량
	ULONG nRemainVolumeTotalSell;			// 매도 호가 잔량
	ULONG nAccrueNumberofTimesBuy;			// 누적 매수체결 횟수
	ULONG nAccrueNumberofTimesSell;			// 누적 매도체결 횟수
	ULONG 누적매수체결량;					// 누적매수거래량, 체결 데이터에서 얻을 수 있다.
	ULONG 누적매도체결량;				// 누적매도거래량, 체결 데이터에서 얻을 수 있다.
	ULONG nAveragePaymentBuy;				// 평균 매수 거래가
	ULONG nAveragePaymentSell;				// 평균 매도 거래가
	__int64 nAccruePaymentBuy;				// 누적 매수거래대금
	__int64 nAccruePaymentSell;				// 누적 매도거래대금
} CANDLE_DATA_TYPE2, *LPCANDLE_DATA_TYPE2;

typedef struct _CANDLE_DATA_TYPEF
{
	ULONG nDate;							// 날짜
	ULONG nTime;							// 시간
	float fOpen;
	float fHigh;
	float fLow;
	float fClose;
	ULONG nTotalVolume;						// 거래량
	__int64 nTransferPayment;				// 거래대금
	ULONG nRemainVolumeTotalBuy;			// 매수 호가 잔량
	ULONG nRemainVolumeTotalSell;			// 매도 호가 잔량
	ULONG nAccrueNumberofTimesBuy;			// 누적 매수체결 횟수
	ULONG nAccrueNumberofTimesSell;			// 누적 매도체결 횟수
	ULONG 누적매수체결량;					// 누적매수거래량, 체결 데이터에서 얻을 수 있다.
	ULONG 누적매도체결량;				// 누적매도거래량, 체결 데이터에서 얻을 수 있다.
	ULONG nAveragePaymentBuy;				// 평균 매수 거래가
	ULONG nAveragePaymentSell;				// 평균 매도 거래가
	__int64 nAccruePaymentBuy;				// 누적 매수거래대금
	__int64 nAccruePaymentSell;				// 누적 매도거래대금
} CANDLE_DATA_TYPEF, *LPCANDLE_DATA_TYPEF;

typedef struct _BET_HEADER
{
	ULONG nFileType;			// 베트 메인의 데이터 파일. ex) 0x0000E90C
	ULONG nFileStyle;			// 스틱 데이터				ex) 0x00000001
	ULONG nStockExchange;		// [0]: none, [1]: 크레온, [2]: 키움, [3]: 이베스트
	char 종목코드[8];				// 키움, 이베스트
	char 크레온코드[8];		// 크레온
	char 풀코드[16];		// 풀코드
	ULONG nStickType;			// [0]: 분봉, [1]: 일봉, [2]: 주봉, [3]: 월봉, [4]: 틱봉
	ULONG nStickLength;			// [1]: 1분봉, [3]: 3분봉
	ULONG nStickSize;			// 스틱 한개의 크기: 기본캔들=28바이트, 확장캔들=128바이트
	ULONG nCountSticks;			// 분봉의 개수
	ULONG nLastDate;			// 가장 최근 일
	ULONG nLastTime;
} BET_HEADER, *LPBET_HEADER;

typedef struct _BET_FILE_HEADER
	: public _BET_HEADER
{
	char szDummy[((1 << 12) - sizeof(_BET_HEADER))];	// 헤더 크기는 0x1000
} BET_FILE_HEADER, *LPBET_FILE_HEADER, 베트파일헤더, *베트파일헤더포;

typedef struct _BET_STICK_DATAF
{
	ULONG nDate;							// 날짜
	ULONG nTime;							// 시간
	float fOpen;							// 시가
	float fHigh;							// 고가
	float fLow;								// 저가
	float fClose;							// 종가(현재가)
	ULONG nTotalVolume;						// 거래량
	_BET_STICK_DATAF()
	{
		nDate = nTime = nTotalVolume = 0;
		fOpen = fHigh = fLow = fClose = 0.0f;
	}
} BET_STICK_DATAF, *LPBET_STICK_DATAF;		// 28 바이트

typedef struct _BET_STICK_EX
	: public _BET_STICK_DATAF
{
	__int64 nTransferPayment;				// 거래대금, 예스차트 또는 체결 데이터에서 받아야함
	ULONG nRemainVolumeTotalBuy;			// 매수 호가 잔량, 예스차트 또는 체결 데이터에서 받아야함
	ULONG nRemainVolumeTotalSell;			// 매도 호가 잔량, 예스차트 또는 체결 데이터에서 받아야함
	ULONG nAccrueNumberofTimesBuy;			// 누적 매수체결 횟수, 예스차트 또는 체결 데이터에서 합산해야함
	ULONG nAccrueNumberofTimesSell;			// 누적 매도체결 횟수, 예스차트 또는 체결 데이터에서 합산해야함
	ULONG 누적매수체결량;					// 누적매수거래량, 체결 데이터에서 합산하면 되고, 분봉은 크레온에서 준다.ㅏ
	ULONG 누적매도체결량;				// 누적매도거래량, 체결 데이터에서 합산하면 되고, 분봉은 크레온에서 준다.ㅏ
	ULONG nAveragePaymentBuy;				// 평균 매수 거래가, 체결 데이터에서 합산해야함.
	ULONG nAveragePaymentSell;				// 평균 매도 거래가, 체결 데이터에서 합산해야함.
	__int64 nAccruePaymentBuy;				// 누적 매수거래대금, 체결 데이터에서 합산해야함.
	__int64 nAccruePaymentSell;				// 누적 매도거래대금, 체결 데이터에서 합산해야함.
	ULONG nDummy[11];						// 88, 16 바이트씩 묶기 위해 더미 추가. ( 현재 44 바이트 )
	void clear() { ZeroMemory(this, sizeof(_BET_STICK_EX)); }
#if _MSC_VER > 1900 // vs2017 이상
	void 초기화() { ZeroMemory(this, sizeof(_BET_STICK_EX)); }
#endif
} BET_STICK_EX, *LPBET_STICK_EX, 확장캔들, *확장캔들포;			// 28 + 56 = 84 바이트, 더미포함하면 128바이트


typedef struct _STICKS_HEADER
{
	char 크레온코드[8];			// 크레온코드
	char 플코드[13];			// 풀코드
	ULONG nStickType;		// [0]: 분봉, [1]: 일봉, [2]: 주봉, [3]: 월봉, [4]: 틱봉
	ULONG nStickLength;		// [1]: 1분봉, [3]: 3분봉
	ULONG nMaxSticks;		// 캔들 최대 갯수
	ULONG nCountSticks;		// 기록된 캔들의 갯수
} STICKS_HEADER, *LPSTICKS_HEADER;

typedef struct _STICK_DATAF
{
	ULONG nDate;							// 날짜
	ULONG nTime;							// 시간
	float fOpen;							// 시가
	float fHigh;							// 고가
	float fLow;							// 저가
	float fClose;							// 종가(현재가)
	ULONG nTotalVolume;						// 거래량
	ULONG 누적매수체결량;					// 누적매수거래량, 체결 데이터에서 합산하면 되고, 분봉은 크레온에서 준다.ㅏ
	ULONG 누적매도체결량;				// 누적매도거래량, 체결 데이터에서 합산하면 되고, 분봉은 크레온에서 준다.ㅏ
	ULONG 고가시간;
	ULONG 저가시간;
	float 매수평균가;
	float 매도평균가;
} STICK_DATAF, *LPSTICK_DATAF;		// 28 바이트


typedef struct _BET_INDICATOR
{
	//pine_swma(x) => x[3] * 1 / 6 + x[2] * 2 / 6 + x[1] * 2 / 6 + x[0] * 1 / 6
	float fSymmetryWeightMovingAverage;	// close[3] * 1 / 6 + close[2] * 2 / 6 + close[1] * 2 / 6 + close[0] * 1 / 6
	float fDummy;

} BET_INDICATOR, *LPBET_INDICATOR;

typedef struct _BET_STICK_DAY
	: public _BET_STICK_DATAF
{
	__int64 nTransferPayment;				// 거래대금
	ULONG nRemainVolumeTotalBuy;			// 매수 호가 잔량
	ULONG nRemainVolumeTotalSell;			// 매도 호가 잔량
	ULONG nAccrueNumberofTimesBuy;			// 누적 매수체결 횟수
	ULONG nAccrueNumberofTimesSell;			// 누적 매도체결 횟수
	ULONG 누적매수체결량;					// 누적매수거래량, 체결 데이터에서 얻을 수 있다.
	ULONG 누적매도체결량;				// 누적매도거래량, 체결 데이터에서 얻을 수 있다.
	ULONG nAveragePaymentBuy;				// 평균 매수 거래가
	ULONG nAveragePaymentSell;				// 평균 매도 거래가
	__int64 nAccruePaymentBuy;				// 누적 매수거래대금
	__int64 nAccruePaymentSell;				// 누적 매도거래대금
} BET_STICK_DAY, *LPBET_STICK_DAY;

///////////////////////////////////////////////////////////////////////////////
// 아래건 안쓰일듯하다.
///////////////////////////////////////////////////////////////////////////////
struct _SAVE_DATA_HEADER
{
	bool bForceWrite;			// 강제 쓰기
	long nIndex;				// 오더
	char szCode[8];				// 종목코드 ( 폴더명 작성을 위해 )
};

typedef struct _SAVE_FILE_DATA
	: public _SAVE_DATA_HEADER
	, public TRANSFER_DATA
{
	void clear() { ZeroMemory(this, sizeof(_SAVE_FILE_DATA)); }
} SAVE_FILE_DATA, *LPSAVE_FILE_DATA;
///////////////////////////////////////////////////////////////////////////////
#pragma pack(pop)			// 정렬 설정을 이전 상태(기본값)로 되돌림

//typedef struct _CALCULATE_WORK
//{
//
//} CALCULATE_WORK, * LPCALCULATE_WORK;

// ta-lib 는 파라미터로 배열을 받기 때문에 각각의 배열을 가져야한다
typedef struct _STICK_DATAS
{
	std::vector<ULONG>	vStartDates;
	std::vector<ULONG>	vStartTimes;
	//std::vector<ULONG>	vEndDates;
	//std::vector<ULONG>	vEndTimes;

	// ta-lib  때문에 double 으로 적재한다. 저장할땐 4바이트씩.
	std::vector<double>	vOpens;					// 시가
	std::vector<double>	vHighs;					// 고가
	std::vector<double>	vLows;					// 저가
	std::vector<double>	vCloses;				// 종가
	// 4바이트에 42억까지 가능한데, 삼성전자 한달 거래량이 8억정도인듯 하다. 4바이트로 하자.
	std::vector<ULONG>	vVolumes;		// 거래량

	// 저장할 때 날짜 시간은 8바이트, 봉데이터는 20바이트 = 28바이트

	//std::vector<double> vSma5;					// 이평선5
	//std::vector<double> vSma10;					// 이평선10
	//std::vector<double> vSma20;					// 이평선20
	//std::vector<double> vSma60;					// 이평선60
	//std::vector<double> vSma120;				// 이평선120

	//std::vector<double> vEma5;					// 지수이평선5
	//std::vector<double> vEma10;					// 지수이평선10
	std::vector<double> vEma20;					// 지수이평선20
	//std::vector<double> vEma60;					// 지수이평선60
	//std::vector<double> vEma120;				// 지수이평선120

	//std::vector<double> vMacd;
	//std::vector<double> vMacdSignal;
	//std::vector<double> vMacdHist;

} STICK_DATAS, *LPSTICK_DATAS;

typedef struct _RECIVE_REALTIME_DATA_CONVERTED
{
	BYTE bytMessageType;			// 주식시세, 주식체결 등
	BYTE bytTransType;				// [1]: 매수, [2]: 매도, [3]: 전일종가와 동일할 경우

	ULONG nTime;					// 091259
	ULONG nOpen;					// 시 10000000
	ULONG nHigh;					// 고 10000000
	ULONG nLow;					// 저 10000000
	ULONG nClose;					// 종 10000000
	ULONG nVolume;					// 거래량 9999999999999
	__int64 nAccrueVolume;			// 누적거래량 9999999999999
	__int64 nAccruePayment;		// 누적거래대금 9999999999999

	TCHAR szStockCode[(1 << 3)];	// 종목코드
	//float szStrength[1 << 4];		// 체결강도

	//DWORD nMessageSize;									// 원본 메시지 크기
	//_TCHAR szAllMessage[(1 << 14) - sizeof(DWORD)];		// 메시지 원본
	void clear() { ZeroMemory(this, sizeof(_RECIVE_REALTIME_DATA_CONVERTED)); }
} RECIVE_REALTIME_DATA_CONVERTED, *LPRECIVE_REALTIME_DATA_CONVERTED;

typedef struct _CANDLE_DATA_BASE
{
	ULONG nDate;								// 날짜
	ULONG nTime;								// 시간
	ULONG nOpen;
	ULONG nHigh;
	ULONG nLow;
	ULONG nClose;
	ULONG nTotalVolume;						// 거래량
} CANDLE_DATA_BASE, *LPCANDLE_DATA_BASE;

#pragma pack(push, 1)    // 1바이트 크기로 정렬
// 이건 틱데이터, 들어오는대로 작업 큐와 파일매니저 큐에 넣는다
typedef struct _주식체결변환
{
	ULONG 총시퀀스{ 0 };					// 받은 순서 ( 키움에서 준 + 키움으로부터 받은 밀리초 )
	ULONG 체결시간{ 0 };					// 체결시간, 파일 이름이 11시59분31초
	// 분봉 또는 초봉을 위해
	ULONG 초_시{ 0 };						// 09 는 32400
	WORD 초_분{ 0 };						// 30 은 1800
	BYTE 초{ 0 };							// 27 은 27
	//BYTE 전일대비구분;			// 
	//ULONG 전일대비;				// 
	//float 등락율;					// 
	double 현재가{ 0.0 };					// 
	//ULONG 시가시간;				// 
	//float 시가;					// 
	//ULONG 고가시간;				// 
	//float 고가;					// 
	//ULONG 저가시간;				// 
	//float 저가;					// 
	char 체결구분;					// 
	ULONG 거래량;					// 
	UINT64 누적거래량;				// 
	//ULONG64 누적거래대금;			// 
	//ULONG64 매도누적체결량;		// 
	//ULONG 매도누적체결건수;		// 
	//ULONG64 매수누적체결량;		// 
	//ULONG 매수누적체결건수;		// 
	//float 체결강도;				// 
	//float 가중평균가_이베스트;		// 
	float 최우선매도호가;					// 
	float 최우선매수호가;					// 
	//BYTE 장정보;					// 
	//ULONG64 전일동시간대거래량;	// 
	char 종목코드[7];				// 
} 주식체결변환, * 주식체결변환포;

typedef struct _TICK_DATA
{	// 저장할 버퍼
	ULONG nSequence;			// 받은 순서 ( 키움으로부터 )
	char szCode[7];				// "005930"
	char nTransType;			// 1 = 매수, 2 = 매도, 3 = 단일가매매(동시호가)
	ULONG nTime;				// 체결시간, 파일 이름이 11시59분31초, 여기 뒤에 밀리초 3자리 붙인다.
	float fPrice;				// 체결가격(선물 때문에 float 으로 저장하자)
	ULONG nTransVolume;			// 체결량
	ULONG nAccrueVolume;		// 누적거래량
	float 최우선매도호가;		// 
	float 최우선매수호가;		// 체결량
} TICK_DATA, *LPTICK_DATA;

//typedef struct _TICK_DATAF
//{	// 저장할 버퍼
//	ULONG nSequence;			// 받은 순서 ( 키움으로부터 )
//	char szCode[7];				// "005930"
//	char nTransType;			// 1 = 매수, 2 = 매도, 3 = 단일가매매(동시호가)
//	ULONG nTime;				// 체결시간, 파일 이름이 11시59분31초, 여기 뒤에 밀리초 3자리 붙인다.
//	float fPrice;				// 체결가격(선물 때문에 float 으로 저장하자)
//	float nTransVolume;			// 체결량
//	float nAccrueVolume;		// 누적거래량
//	float 최우선매도호가;		// 체결량
//	float 최우선매수호가;		// 체결량
//} TICK_DATAF, *LPTICK_DATAF;

typedef struct _TICK_TRANSACTIONF
{	// 저장할 버퍼
	ULONG nSequence;			// 받은 순서 ( 키움으로부터 )
	char szCode[7];				// "005930"
	char nTransType;			// 1 = 매수, 2 = 매도, 3 = 단일가매매(동시호가)
	ULONG nTime;				// 체결시간, 파일 이름이 11시59분31초, 여기 뒤에 밀리초 3자리 붙인다.
	float fPrice;				// 체결가격(선물 때문에 float 으로 저장하자)
	float nTransVolume;			// 체결량
	float nAccrueVolume;		// 누적거래량
	float 최우선매도호가;		// 체결량
	float 최우선매수호가;		// 체결량
} TICK_TRANSACTIONF, *LPTICK_TRANSACTIONF;
#pragma pack(pop)			// 정렬 설정을 이전 상태(기본값)로 되돌림

typedef struct _주식호가잔량변환
{
	ULONG 총시퀀스;			// 받은 순서 ( 키움에서 준 + 키움으로부터 받은 밀리초 )
	//ULONG nMilliSecond;	// 받은 시간 ( 키움으로부터 ) 11시59분31초593밀리초
	BYTE 동시호가구분;		// [1]: 장중, 장후 동시호가 체결, [2]: 장마감, [3]: 장전 동시호가, 장후 동시호가, 
	ULONG 호가시간;			// 체결시간, 파일 이름이 11시59분31초

	BYTE 변동호가;

	ULONG 매도호가_1;
	ULONG 매수호가_1;
	ULONG 매도호가잔량_1;
	ULONG 매수호가잔량_1;
	ULONG 매도호가_2;
	ULONG 매수호가_2;
	ULONG 매도호가잔량_2;
	ULONG 매수호가잔량_2;
	ULONG 매도호가_3;
	ULONG 매수호가_3;
	ULONG 매도호가잔량_3;
	ULONG 매수호가잔량_3;
	//ULONG 매도호가_4;
	//ULONG 매수호가_4;
	//ULONG 매도호가잔량_4;
	//ULONG 매수호가잔량_4;
	//ULONG 매도호가_5;
	//ULONG 매수호가_5;
	//ULONG 매도호가잔량_5;
	//ULONG 매수호가잔량_5;
	//ULONG 매도호가_6;
	//ULONG 매수호가_6;
	//ULONG 매도호가잔량_6;
	//ULONG 매수호가잔량_6;
	//ULONG 매도호가_7;
	//ULONG 매수호가_7;
	//ULONG 매도호가잔량_7;
	//ULONG 매수호가잔량_7;
	//ULONG 매도호가_8;
	//ULONG 매수호가_8;
	//ULONG 매도호가잔량_8;
	//ULONG 매수호가잔량_8;
	//ULONG 매도호가_9;
	//ULONG 매수호가_9;
	//ULONG 매도호가잔량_9;
	//ULONG 매수호가잔량_9;
	//ULONG 매도호가_10;
	//ULONG 매수호가_10;
	//ULONG 매도호가잔량_10;
	//ULONG 매수호가잔량_10;
	ULONG 매도호가총잔량;
	ULONG 매수호가총잔량;
	//ULONG64 누적거래량;
	//BYTE 배분적용구분;
	char 종목코드[7];	// "005930"
} 주식호가잔량변환, * 주식호가잔량변환포;
