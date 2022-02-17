#pragma once


#include <Defines/DataDef.h>



typedef struct _ARRAY_STICK_BASE
{	// 추가할때 다같이 추가하고 제거해서 갯수를 유지한다.
	WORD nArrayCount{ 0 };
	WORD nCurrentIndex{ 0 };

	std::string path;

	std::deque<ULONG> date;
	std::deque<ULONG> time;
	std::deque<double> open;
	std::deque<double> high;
	std::deque<double> low;
	std::deque<double> close;
	std::deque<ULONG> volume;
	std::deque<double> hlc3;
	std::deque<double> ohlc4;

	virtual void Init();
	long push_back(ULONG _날짜, ULONG _시간, ULONG _거래량 = 0, double _시가 = 0.0, double _고가 = 0.0, double _저가 = 0.0, double _종가 = 0.0);
	long push_back(ULONG _시간, ULONG _거래량, double _현재가, ULONG _날짜 = 0);
	long update(ULONG _거래량, double _체결가, ULONG _시간 = 0);
	long update(ULONG _시간, ULONG _거래량, double _고가, double _저가, double _종가, double _시가 = 0.0, bool _누적 = true);
	void pop_front();
	size_t size() { return(nArrayCount); }

	void Update();
} ARRAY_STICK_BASE, *LPARRAY_STICK_BASE;

typedef struct _DEQUE_BOLINGER_BAND
{
	std::deque<double> BBLineHigh;					// [0]: high, [1]: WMA 20, [2]: low
	std::deque<double> BBLineMA;					// [0]: high, [1]: WMA 20, [2]: low
	std::deque<double> BBLineLow;					// [0]: high, [1]: WMA 20, [2]: low
	virtual void Init()
	{
		BBLineHigh.clear();
		BBLineMA.clear();
		BBLineLow.clear();
	}
} DEQUE_BOLINGER_BAND, *LPDEQUE_BOLINGER_BAND;

typedef struct _DEQUE_STOCH
{
	std::deque<double> StochFast;
	std::deque<double> StochSlow;
	virtual void Init()
	{
		StochFast.clear();
		StochSlow.clear();
	}
} DEQUE_STOCH, *LPDEQUE_STOCH;

typedef struct _DEQUE_RSI
{
	std::deque<double> RSI_14;
	virtual void Init()
	{
		RSI_14.clear();
	}
} DEQUE_RSI, *LPDEQUE_RSI;

typedef struct _DEQUE_MACD
{
	std::deque<double> MACD_FAST;					// = (ema(close, 12) - ema(close, 26))
	std::deque<double> MACD_SLOW;					// = ema(MACD빠른선, 9)
	std::deque<double> MACD_HistLine;
	virtual void Init()
	{
		MACD_FAST.clear();
		MACD_SLOW.clear();
		MACD_HistLine.clear();
	}
} DEQUE_MACD, *LPDEQUE_MACD;

typedef struct _ARRAY_BASE
	: public _ARRAY_STICK_BASE
	//, public _DEQUE_BOLINGER_BAND
{	// 추가할때 다같이 추가하고 제거해서 갯수를 유지한다.

	virtual void Init()
	{
		_ARRAY_STICK_BASE::Init();
		//_DEQUE_BOLINGER_BAND::Init();
	}
} ARRAY_STICK, *LPARRAY_STICK;
