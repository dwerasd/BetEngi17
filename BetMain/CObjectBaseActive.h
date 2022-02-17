#pragma once


#include <math.h>
#include <stdlib.h>

#include <cmath>

#include <Defines/DataDef.h>
#include <Defines/XingDef.h>

#include <ta-lib/ta_libc.h>

#include "CObjectBase.h"



//#define _사용_틱봉_
#if defined(_사용_틱봉_)
enum
{
	_USE_TICK_10_ = 0
	, _USE_TICK_20_
	, _USE_TICK_30_
	, _USE_TICK_40_
	, _USE_TICK_60_
	, _USE_TICK_100_
	, _USE_TICK_120_
	, _USE_TICK_150_
	, _USE_TICK_180_
	, _USE_TICK_200_
	, _USE_TICK_240_
	, _MAX_USE_TICK_
};
constexpr BYTE nTickLengths[_MAX_USE_TICK_] = { 10, 20, 30, 40, 60, 100, 120, 150, 180, 200, 240 };
#endif

//#define _사용_초봉_
#if defined(_사용_초봉_)
enum
{
	_USE_SECOND_1_ = 0
	, _USE_SECOND_5_
	, _USE_SECOND_10_
	, _USE_SECOND_15_
	, _USE_SECOND_30_
	, _USE_SECOND_45_
	, _USE_SECOND_135_
	, _MAX_USE_SECOND_
};
constexpr WORD nSecondLengths[_MAX_USE_SECOND_] = { 1, 5, 10, 15, 30, 45, 135 };
#endif

#define _사용_분봉_
#if defined(_사용_분봉_)
enum
{
	_USE_MINUTE_1_ = 0
	, _USE_MINUTE_5_
	, _USE_MINUTE_15_
	, _USE_MINUTE_30_
	, _USE_MINUTE_45_
	, _USE_MINUTE_135_
	, _MAX_USE_MINUTE_
};
constexpr WORD nMinuteLengths[_MAX_USE_MINUTE_] = { 1, 5, 15, 30, 45, 135 };
//constexpr WORD nSecondOfMinute[_MAX_USE_MINUTE_] = { 60, 300, 900, 1800, 2700, 8100 };
#endif

#define _사용_볼린저밴드_
#define _사용_기본보조지표_
#if defined(_사용_기본보조지표_)
//#define _사용_기본MACD_
#endif

typedef struct _OBJECT_CHANGED_RECORD_BASE
{	// 추가할때 다같이 추가하고 제거해서 갯수를 유지한다.
	std::deque<ULONG> 날짜;
	std::deque<ULONG> 시간;
	std::deque<ULONG> 거래량;
	std::deque<double> 종가;
	std::deque<double> 시가;
	std::deque<double> 고가;
	std::deque<double> 저가;
	std::deque<double> 고저종;
	std::deque<double> 시고저종;
	virtual void Init();
} OBJECT_CHANGED_RECORD_BASE, *LPOBJECT_CHANGED_RECORD_BASE;

typedef struct _OBJECT_CHANGED_RECORD_INDICATORS
//: public _캔들_데이터_기본
{
	std::deque<double> SWMA;
#if defined(_사용_볼린저밴드_)
	std::deque<double> 볼밴_중단;					// [0]: high, [1]: WMA 20, [2]: low
	std::deque<double> 볼밴_상단;					// [0]: high, [1]: WMA 20, [2]: low
	std::deque<double> 볼밴_하단;					// [0]: high, [1]: WMA 20, [2]: low
#endif
#if defined(_사용_기본보조지표_)
	std::deque<double> 스토캐스틱빠른선;
	std::deque<double> 스토캐스틱느린선;
	std::deque<double> 상대강도지수14;

	std::deque<double> MACD빠른선;					// = (ema(close, 12) - ema(close, 26))
	std::deque<double> MACD느린선;					// = ema(MACD빠른선, 9)
	std::deque<double> HistLine;
#endif
	virtual void Init();
} OBJECT_CHANGED_RECORD_INDICATORS, *LPOBJECT_CHANGED_RECORD_INDICATORS;

typedef struct _OBJECT_CHANGED_RECORD
	: public _OBJECT_CHANGED_RECORD_BASE
	, public _OBJECT_CHANGED_RECORD_INDICATORS
{
	bool 매수대기플래그{ false };
	WORD 캔들카운트{ 0 };
	WORD 캔들인덱스{ 0 };

	WORD 매매카운트{ 0 };
	BYTE 매수카운트{ 0 };

	ULONG 매수량{ 0 };
	ULONG 매수금액{ 0 };

	std::deque<BYTE> 베팅플래그;
	std::string 경로;

	void Init();
	size_t size() { return(캔들카운트); }
	// 완성된 스틱
	long insert(ULONG _날짜, ULONG _시간, double _종가, ULONG _거래량 = 0, double _시가 = 0.0, double _고가 = 0.0, double _저가 = 0.0);
	long insert(ULONG _날짜, ULONG _시간, ULONG _거래량 = 0, double _시가 = 0.0, double _고가 = 0.0, double _저가 = 0.0, double _종가 = 0.0);
	// 실시간 스틱
	long push_back(ULONG _시간, ULONG _거래량, double _현재가, ULONG _날짜 = 0);
	long update(ULONG _거래량, double _종가, ULONG _시간 = 0);
	long update(ULONG _시간, ULONG _거래량, double _고가, double _저가, double _종가, double _시가 = 0.0, bool _누적 = true);
	long calculate();
	void pop_front();

	ULONG lastdate() { return(날짜[캔들인덱스]); }
	ULONG lasttime() { return(시간[캔들인덱스]); }
} OBJECT_CHANGED_RECORD, *LPOBJECT_CHANGED_RECORD;

///////////////////////////////////////////////////////////////////////////////
// 보조 연산을 위한 버퍼풀
typedef struct _DYNAMIC_BUFFER_DOUBLE
{
	double* pBuffer{ nullptr };
	size_t nSize{ 0 };

	_DYNAMIC_BUFFER_DOUBLE()
	{
		Alloc(_MAX_ARRAY_BUFFER_SIZE_);
	}
	~_DYNAMIC_BUFFER_DOUBLE()
	{
		Free();
	}
	void Free()
	{
		힙해제(pBuffer);
		nSize = 0;
	}
	void Alloc(size_t _버퍼크기)
	{
		//DBGPRINT("_동적버퍼_더블::할당(%d)", _버퍼크기);
		pBuffer = (double*)힙할당(_버퍼크기);
		nSize = _버퍼크기;
	}
	void Init() { ::memset(pBuffer, 0, nSize); }
} DYNAMIC_BUFFER_DOUBLE, *LPDYNAMIC_BUFFER_DOUBLE;
static DYNAMIC_BUFFER_DOUBLE BufferPool[_보조지표버퍼수_];
///////////////////////////////////////////////////////////////////////////////

class C_OBJECT_BASE_ACTIVE
	: public C_OBJECT_BASE
{
public:
	C_OBJECT_BASE_ACTIVE();
	~C_OBJECT_BASE_ACTIVE();

#if defined(_사용_틱봉_)

#endif
#if defined(_사용_초봉_)

#endif
#if defined(_사용_분봉_)
	OBJECT_CHANGED_RECORD 분봉[_MAX_USE_MINUTE_];
#endif
	OBJECT_CHANGED_RECORD 일봉;
	OBJECT_CHANGED_RECORD 주봉;
	OBJECT_CHANGED_RECORD 월봉;

	virtual void Init()
	{
		C_OBJECT_BASE::Init();
#if defined(_사용_분봉_)
		for (size_t i = 0; i < _MAX_USE_MINUTE_; i++)
		{
			분봉[i].Init();
		}
#endif
		일봉.Init();
		주봉.Init();
		월봉.Init();
	}
};
