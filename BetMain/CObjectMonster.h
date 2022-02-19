#pragma once


#include <map>
#include <vector>
#include <unordered_map>

#include <DarkCore/DDef.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DString.h>
#include <DarkCore/DUtil.h>

#include <Defines/DataDef.h>

#include "GameStruct.h"
#include "CObjectBaseActive.h"

////////////////////////////////////////////////////////////////////////////////////////
// 이 클래스는 종목마다 한개씩 소유한다.
typedef std::unordered_map<ULONG, LPSTICK_DATA> UMAP_STICK_DATAS;		// 시간별 분봉.

class C_OBJECT_MONSTER
	: public C_OBJECT_BASE_ACTIVE
{
private:
	BYTE bControlKind{ 0 };								// [0]: 정상, [1]: 투자주의, [2]: 투자경고, [3]: 투자위험, [4]: 투자주의환기종목

	BYTE market_creon;
	BYTE section_creon;
	BYTE control_creon;
	BYTE supervision_creon;
	BYTE status_creon;
	BYTE spac_creon;
	double nPreClose{ 0.0 };							// 전일종가는 당일 만든 일봉의 종가로 디비에 업데이트 해둔다.
	BYTE nEvidence_kiw;									// 증거금 40%, 100% 등 숫자로

	
	float 최우선매도호가{ 0 }, 최우선매수호가{ 0 };
	long 누적매수거래횟수{ 0 }, 누적매도거래횟수{ 0 };	// 매수거래 횟수

	ULONG nDateToday{ 0 };								// 당일 일자를 숫자로 변환해서 가지고 있는다.
	long nCurrentTime{ 0 };							// 현재 들어온 데이터의 시간.

	bool bState[8] = { false };							// [0]: 정상, [1]: 거래정지, [2]: 관리종목. [3]: 감리종목, [4]: 투자유의종목, [5]: 담보대출, [6]: 액면분할, [7]: 신용가능
	char szState[(1 << 6)] = { 0 };						// 종목상태, 길수도 있으니까 넉넉하게 64

	char szControlKind[(1 << 5)] = { 0 };				// 감리구분, 투자주의환기종목 때문에 16바이트 초과

public:
	C_OBJECT_MONSTER(ULONG _nDateToday = 0);
	~C_OBJECT_MONSTER();

	ULONG 누적체결건수{ 0 };							// 해당 종목의 체결 데이터의 카운트
	ULONG 누적매도체결건수{ 0 }, 누적매수체결건수{ 0 };
	ULONG 누적매도체결량{ 0 }, 누적매수체결량{ 0 }, 누적체결량{ 0 };
	
	ULONG 고가시간{ 0 };								// 종목별 당일 고가가 갱신된 시간.
	ULONG 저가시간{ 0 };								// 종목별 당일 저가가 갱신된 시간.

	float 체결강도{ 0.0f };

	bool 상태{ false };
	bool 관리종목{ false };					// 0 이 아니면 거름
	BYTE 상태_크레온{ 0 };				// 위험 경고 등 0 이 아니면 거름
	bool 스팩주{ false };
	BYTE 섹션{ 0 };							// ETF, ELW 등의 직업
	BYTE 장구분{ 0 };						// [1]: 코스피, [2]: 코스닥
	double 전일종가{ 0.0 };
	// 키움
	bool 키움_거래정지{ false };		// 거래정지
	bool 키움_관리종목 { false };		// 관리종목
	bool 키움_담보대출{ false };		// 담보대출
	bool 키움_신용가능{ false };		// 신용가능
	bool 키움_뮤추얼펀드{ false };		// 뮤추얼 펀드	( 구려 )
	bool 키움_신주인수권{ false };		// 신주인수권	( 작전주 같음 )
	bool 키움_리츠{ false };			// 리츠			( 좋아 )
	bool 키움_ETF{ false };				// ETF			( 코스피 코스닥에 많음 )
	bool 키움_ELW{ false };				// ETF			( 코스피 코스닥에 많음 )
	bool 키움_하이일드펀드{ false };				// ETF			( 코스피 코스닥에 많음 )
	bool 키움_K_OTC{ false };			// ETF			( 코스피 코스닥에 많음 )
	bool 키움_코넥스{ false };			// ETF			( 코스피 코스닥에 많음 )
	BYTE 키움_감리구분{ 0 };			// [0]: 정상, [1]: 투자주의, [2]: 투자경고, [3]: 투자위험, [4]: 투자주의환기종목
	WORD 키움_증거금{ 0 };				// 증거금

	ULONG 시가총액{ 0 };
	// 종목별로 체결데이터 포인터를 가지고 있는다. 이건 종목별로 백테스팅 할 때 필요함.
	std::vector<LPTICK_DATA>	vTransactionTicks;

	void Init(LPSTR _종목코드);
	void Init_Creon(LPSTR _종목코드, LPSTR _크레온코드, LPSTR _풀코드, LPSTR _종목명);
	void Init_Kiwoom(LPSTR _종목코드, LPSTR _종목명 = nullptr, LPSTR _종목상태 = nullptr, LPSTR _감리구분 = nullptr);

	// 해당 종목의 당일 데이터, 이건 아래 로드한 배열의 뒤에 계속 붙이기로 하자.

	// 당일의 스틱 및 보조지표 데이터를 가지고 있어야하는데, std::deque 를 사용해서 며칠간의 연속된 데이터를 가지고 있도록 하자.
	ARRAY_STICK_BASE 월봉;
	ARRAY_STICK_BASE 주봉;
	ARRAY_STICK_BASE 일봉;
	// 각 분봉
	ARRAY_STICK_BASE 분봉[_MAX_USE_MINUTE_];
	void InitSticks();

	_STICK_DATA stickToday;

	ULONG 매도호가총잔량{ 0 };
	ULONG 매수호가총잔량{ 0 };
	/*
	ULONG nTodayOpen;
	ULONG nTodayHigh;
	ULONG nTodayLow;
	ULONG nTodayClose;
	ULONG nTodayVolume;
	ULONG nTodayAccrueVolume;						// 누적 거래량

	ULONG nAveragePaymentBuy;						// 평균 매수가격 = 대금 / 거래량
	ULONG nAveragePaymentSell;						// 평균 매도가격 = 대금 / 거래량

	ULONG 누적매수체결량;							// 누적 매수 거래량
	ULONG 누적매도체결량;						// 누적 매도 거래량

	unsigned __int64 nAccruePaymentBuy;						// 누적 매수 대금
	unsigned __int64 nAccruePaymentSell;					// 누적 매도 대금
	*/
	// 해당 종목의 당일 데이터, 이건 아래 로드한 배열의 뒤에 계속 붙이기로 하자.

	// 매매 판단을 하기 위해 각 시간대의 봉들이 필요하다.
	UMAP_STICK_DATAS umapStickMinute1;				// 1분봉		장 시작전에 1일치정도 로드한다.
	UMAP_STICK_DATAS umapStickMinute3;				// 3분봉		장 시작전에 3일치정도 로드한다.
	UMAP_STICK_DATAS umapStickMinute5;				// 5분봉		장 시작전에 4일치정도 로드한다.
	UMAP_STICK_DATAS umapStickMinute15;				// 15분봉		장 시작전에 12일치정도 로드한다.
	UMAP_STICK_DATAS umapStickDay;					// 일봉			장 시작전에 320일치정도 로드한다.
	
	std::vector<LPSTICK_DATA> vStickMinute1;
	ULONG nStickSizeMinute1{ 0 };						// 이전 봉을 채워놨다면 그 개수
	std::vector<LPSTICK_DATA> vStickMinute3;
	ULONG nStickSizeMinute3{ 0 };						// 이전 봉을 채워놨다면 그 개수
	std::vector<LPSTICK_DATA> vStickMinute5;
	ULONG nStickSizeMinute5{ 0 };						// 이전 봉을 채워놨다면 그 개수
	std::vector<LPSTICK_DATA> vStickMinute15;
	ULONG nStickSizeMinute15{ 0 };						// 이전 봉을 채워놨다면 그 개수
	std::vector<LPSTICK_DATA> vStickDay;
	ULONG nStickSizeDay{ 0 };						// 이전 봉을 채워놨다면 그 개수

	void PushTick(LPTICK_DATA _pTick);
	void Update(
		LPSTR _szTime			// 체결시간
		, BYTE _bytType			// 체결 타입
		, double _dbPrice			// 체결가격
		, ULONG _nVolume		// 체결량
		, ULONG _nAccrueVolume	// 당일 누적 체결량
		, float _fStrength		// 당일 체결강도
		, LPSTR _szOpen
		, LPSTR _szHigh
		, LPSTR _sznLow
	);
	void Update(
		LPSTR _szCode
		, LPSTR _szTime
		, LPSTR _szOpen
		, LPSTR _szHigh
		, LPSTR _szLow
		, LPSTR _szClose
		, LPSTR _szVolume
		, LPSTR _szAccrueVolume
		, LPSTR _szStrength
	);
};