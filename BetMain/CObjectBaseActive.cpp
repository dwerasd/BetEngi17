#include "framework.h"
#include "CObjectBaseActive.h"



C_OBJECT_BASE_ACTIVE::C_OBJECT_BASE_ACTIVE()
{

}

C_OBJECT_BASE_ACTIVE::~C_OBJECT_BASE_ACTIVE()
{

}

void _OBJECT_CHANGED_RECORD_BASE::Init()
{
	날짜.clear();
	시간.clear();
	거래량.clear();
	시가.clear();
	고가.clear();
	저가.clear();
	종가.clear();
	고저종.clear();
	시고저종.clear();
}

void _OBJECT_CHANGED_RECORD_INDICATORS::Init()
{
	SWMA.clear();
#if defined(_사용_볼린저밴드_)
	볼밴_중단.clear();
	볼밴_상단.clear();
	볼밴_하단.clear();
#endif
#if defined(_사용_기본보조지표_)
	스토캐스틱빠른선.clear();
	스토캐스틱느린선.clear();
	상대강도지수14.clear();
	MACD빠른선.clear();
	MACD느린선.clear();
	HistLine.clear();
#endif
}

void _OBJECT_CHANGED_RECORD::Init()
{
	_OBJECT_CHANGED_RECORD_BASE::Init();
	_OBJECT_CHANGED_RECORD_INDICATORS::Init();

	매수대기플래그 = false;
	캔들카운트 = 캔들인덱스 = 0;
	매매카운트 = 매수카운트 = 0;
	매수량 = 매수금액 = 0;

	베팅플래그.clear();
	경로.clear();
}

long _OBJECT_CHANGED_RECORD::insert(ULONG _날짜, ULONG _시간, double _종가, ULONG _거래량, double _시가, double _고가, double _저가)
{	// 스틱추가를 하면 마지막에 deque 의 front 제거를 하면서 총 갯수를 맞추기 때문에 여기에서 시뮬레이션을 하도록 하자.
	long nResult = 0;
	날짜.push_back(_날짜);
	시간.push_back(_시간);
	종가.push_back(_종가);
	시가.push_back(_시가);
	고가.push_back(_고가);
	저가.push_back(_저가);
	거래량.push_back(_거래량);
	고저종.push_back((_고가 + _저가 + _종가) / 3);
	시고저종.push_back((_시가 + _고가 + _저가 + _종가) / 4);
	캔들인덱스 = 캔들카운트++;
	calculate();
	디뷰("_OBJECT_CHANGED_RECORD::스틱추가() - [%04d](%d/%d) %0.0f, %0.0f, %0.0f, %0.0f, %0.0f, %0.0f"
		, 캔들인덱스, 날짜[캔들인덱스], 시간[캔들인덱스]
		, 시가[캔들인덱스], 고가[캔들인덱스], 저가[캔들인덱스], 종가[캔들인덱스], 고저종[캔들인덱스], 시고저종[캔들인덱스]
	);
	pop_front();
	{
		if (캔들인덱스 > 20)
		{
			ULONG64 평균거래량 = 0, 평균종가 = 0;
#if 0
			for (size_t i = 0; i < 20; i++)
			{
				평균종가 += (ULONG64)종가[캔들인덱스 - i];
				평균거래량 += (ULONG64)(거래량[캔들인덱스 - i]);
			}
			평균종가 /= 20;
			평균거래량 /= 20;
			ULONG64 평균거래대금 = 평균종가 * 평균거래량;
			ULONG64 평균거래량배수 = (ULONG64)(평균거래량 * 1.5);
#else		
			for (size_t i = 0; i < 4; i++)
			{
				평균종가 += (ULONG64)종가[캔들인덱스 - i];
				평균거래량 += (ULONG64)(거래량[캔들인덱스 - i] * 0.5);
			}
			for (size_t i = 4; i < 7; i++)
			{
				평균종가 += (ULONG64)종가[캔들인덱스 - i];
				평균거래량 += (ULONG64)(거래량[캔들인덱스 - i] * 0.3);
			}
			for (size_t i = 7; i < 10; i++)
			{
				평균종가 += (ULONG64)종가[캔들인덱스 - i];
				평균거래량 += (ULONG64)(거래량[캔들인덱스 - i] * 0.2);
			}
			평균종가 /= 10;
			평균거래량 /= 10;
			ULONG64 평균거래대금 = 평균종가 * 평균거래량;
			ULONG64 평균거래량배수 = (ULONG64)(평균거래량 * 5);
#endif		
			if (평균거래대금 > 1000000000 && 평균거래량배수 < 거래량[캔들인덱스])
			{
				if (날짜[캔들인덱스] > 20211228)
				{
					디뷰(">>> 평균거래대금증가: %zd / %d / %zd / 전일거래량: %d / 평균거래대금: %zd", 평균거래량, 거래량[캔들인덱스], 평균거래량배수, 거래량[캔들인덱스 - 1], 평균거래대금);
				}
			}
		}
	}
	return(nResult);
}

long _OBJECT_CHANGED_RECORD::insert(ULONG _날짜, ULONG _시간, ULONG _거래량, double _시가, double _고가, double _저가, double _종가)
{	// 완성된 스틱
	//디뷰("_OBJECT_CHANGED_RECORD::추가(%d %d) %0.0f %0.0f %0.0f %0.0f %d", _날짜, _시간, _시가, _고가, _저가, _종가, _거래량);
	베팅플래그.push_back(0);
	날짜.push_back(_날짜);
	시간.push_back(_시간);
	종가.push_back(_종가);
	시가.push_back(_시가);
	고가.push_back(_고가);
	저가.push_back(_저가);
	거래량.push_back(_거래량);
	고저종.push_back((_고가 + _저가 + _종가) / 3);
	시고저종.push_back((_시가 + _고가 + _저가 + _종가) / 4);

	SWMA.push_back(0.0);
#if defined(_사용_볼린저밴드_)
	볼밴_중단.push_back(0.0);					// [0]: high, [1]: WMA 20, [2]: low
	볼밴_상단.push_back(0.0);					// [0]: high, [1]: WMA 20, [2]: low
	볼밴_하단.push_back(0.0);					// [0]: high, [1]: WMA 20, [2]: low
#endif
#if defined(_사용_기본보조지표_)
	스토캐스틱빠른선.push_back(0.0);
	스토캐스틱느린선.push_back(0.0);
	상대강도지수14.push_back(0.0);

	MACD빠른선.push_back(0.0);					// = (ema(close, 12) - ema(close, 26))
	MACD느린선.push_back(0.0);					// = ema(MACD빠른선, 9)
	HistLine.push_back(0.0);
#endif
	캔들인덱스 = 캔들카운트++;	// 캔들인덱스는 0부터 시작함.
	//디뷰("프론트제거 전 캔들인덱스: %d 캔들총갯수: %d", 캔들인덱스, 캔들카운트);
	pop_front();
	//디뷰("프론트제거 후 캔들인덱스: %d 캔들총갯수: %d", 캔들인덱스, 캔들카운트);
	return(0);
}

long _OBJECT_CHANGED_RECORD::push_back(ULONG _시간, ULONG _거래량, double _현재가, ULONG _날짜)
{
	베팅플래그.push_back(0);
	날짜.push_back(_날짜);
	시간.push_back(_시간);
	종가.push_back(_현재가);
	시가.push_back(_현재가);
	고가.push_back(_현재가);
	저가.push_back(_현재가);
	거래량.push_back(_거래량);
	고저종.push_back(_현재가);
	시고저종.push_back(_현재가);

	SWMA.push_back(0.0);
#if defined(_사용_볼린저밴드_)
	볼밴_중단.push_back(0.0);					// [0]: high, [1]: WMA 20, [2]: low
	볼밴_상단.push_back(0.0);					// [0]: high, [1]: WMA 20, [2]: low
	볼밴_하단.push_back(0.0);					// [0]: high, [1]: WMA 20, [2]: low
#endif
#if defined(_사용_기본보조지표_)
	스토캐스틱빠른선.push_back(0.0);
	스토캐스틱느린선.push_back(0.0);
	상대강도지수14.push_back(0.0);

	MACD빠른선.push_back(0.0);					// = (ema(close, 12) - ema(close, 26))
	MACD느린선.push_back(0.0);					// = ema(MACD빠른선, 9)
	HistLine.push_back(0.0);
#endif
	캔들인덱스 = 캔들카운트++;	// 캔들인덱스는 0부터 시작함.
	//디뷰("프론트제거 전 캔들인덱스: %d 캔들총갯수: %d", 캔들인덱스, 캔들카운트);
	pop_front();
	//디뷰("프론트제거 후 캔들인덱스: %d 캔들총갯수: %d", 캔들인덱스, 캔들카운트);
	return(0);
}

long _OBJECT_CHANGED_RECORD::update(ULONG _거래량, double _현재가, ULONG _시간)
{
	long nResult = 0;
	if (_시간) { 시간[캔들인덱스] = _시간; }	// 일봉, 주봉, 월봉은 시간을 갱신하지 않는다.
	거래량[캔들인덱스] += _거래량;				// 거래량은 기본적으로 누적한다.
	if (종가[캔들인덱스] != _현재가)
	{	// 현재가의 변동이 있으면 일봉의 시고저종을 갱신한다.
		종가[캔들인덱스] = _현재가;
		if (0.0 == 시가[캔들인덱스]) { 시가[캔들인덱스] = _현재가; }
		if (0.0 == 고가[캔들인덱스]) { 고가[캔들인덱스] = _현재가; }
		if (0.0 == 저가[캔들인덱스]) { 저가[캔들인덱스] = _현재가; }
		// 고가는 체결가가 더 높으면 체결가로 갱신한다.
		if (고가[캔들인덱스] < _현재가)
		{
			고가[캔들인덱스] = _현재가;
			nResult = 1;
		}
		// 저가는 체결가가 더 낮으면 체결가로 갱신한다.
		if (저가[캔들인덱스] > _현재가)
		{
			저가[캔들인덱스] = _현재가;
			nResult = 2;
		}
		고저종[캔들인덱스] = (고가[캔들인덱스] + 저가[캔들인덱스] + _현재가 / 3);
		시고저종[캔들인덱스] = (시가[캔들인덱스] + 고가[캔들인덱스] + 저가[캔들인덱스] + _현재가 / 4);
	}
	// 지표 연산을 다시 한다.
	//지표연산();
	//프론트제거();
	// 이제 매매 판단을 하고, 종목과 해당 봉의 직업, 

	return(nResult);
}

long _OBJECT_CHANGED_RECORD::update(ULONG _시간, ULONG _거래량, double _고가, double _저가, double _종가, double _시가, bool _누적)
{
	long nResult = 0;
	if (_시간) { 시간[캔들인덱스] = _시간; }	// 일봉, 주봉, 월봉은 시간을 갱신하지 않는다.
	if (!_누적) { 거래량[캔들인덱스] = _거래량; }
	else { 거래량[캔들인덱스] += _거래량; }
	//if (_시가) { 시가[캔들인덱스] = _시가;}	// 시가가 없을 시에만 넣는다.
	if (0.0 == 시가[캔들인덱스]) { 시가[캔들인덱스] = _시가; }
	if (0.0 == 고가[캔들인덱스]) { 고가[캔들인덱스] = _고가; }
	if (0.0 == 저가[캔들인덱스]) { 저가[캔들인덱스] = _저가; }
	if (종가[캔들인덱스] != _종가)
	{	// 현재가의 변동이 있으면 일봉의 시고저종을 갱신한다.
		종가[캔들인덱스] = _종가;
	}
	if (고가[캔들인덱스] < _고가)
	{
		고가[캔들인덱스] = _고가;
		nResult = 1;
	}
	if (저가[캔들인덱스] > _저가)
	{
		저가[캔들인덱스] = _저가;
		nResult = 2;
	}
	고저종[캔들인덱스] = (고가[캔들인덱스] + 저가[캔들인덱스] + 종가[캔들인덱스] / 3);
	시고저종[캔들인덱스] = (시가[캔들인덱스] + 고가[캔들인덱스] + 저가[캔들인덱스] + 종가[캔들인덱스] / 4);
	return(nResult);
}

long _OBJECT_CHANGED_RECORD::calculate()
{
	long 리턴 = 0;
	if (3 < 캔들카운트)
	{
		//디뷰("_OBJECT_CHANGED_RECORD::연산(start) 종가[%d]: %0.2f, %d, %d", 캔들인덱스 - 3, 종가[캔들인덱스 - 3], SWMA.size(), 캔들인덱스);
		SWMA[캔들인덱스] = (종가[캔들인덱스 - 3] * 1 / 6)
			+ (종가[캔들인덱스 - 2] * 2 / 6)
			+ (종가[캔들인덱스 - 1] * 2 / 6)
			+ (종가[캔들인덱스] * 1 / 6);
		//디뷰("SWMA: %0.2f", SWMA[캔들인덱스]);
	}

	int 소스배열길이 = 0;
	int 리턴_배열인덱스 = 0;
	int 리턴_배열개수 = 0;
#if defined(_사용_볼린저밴드_)
	int 볼밴길이 = _BB_LENGTH_DEFAULT_;
	소스배열길이 = 볼밴길이;
	if (캔들카운트 > 소스배열길이)
	{
		BufferPool[0].Init();
		BufferPool[1].Init();
		BufferPool[2].Init();
		BufferPool[3].Init();
		size_t 배열인덱스 = 캔들카운트 - 소스배열길이;
		for (size_t 버퍼배열인덱스 = 0; 버퍼배열인덱스 < 소스배열길이; 버퍼배열인덱스++)
		{
			BufferPool[0].pBuffer[버퍼배열인덱스] = 종가[배열인덱스++];
		}
		리턴_배열인덱스 = 0;
		리턴_배열개수 = 0;
		long res = TA_BBANDS(
			0
			, 소스배열길이 - 1
			, BufferPool[0].pBuffer
			, 볼밴길이
			, 2
			, 2
			, (TA_MAType)TA_MAType_WMA
			, &리턴_배열인덱스
			, &리턴_배열개수
			, BufferPool[1].pBuffer
			, BufferPool[2].pBuffer
			, BufferPool[3].pBuffer
		);
		if (TA_SUCCESS == res)
		{	// 마지막 것만 갱신한다.
			볼밴_상단[캔들인덱스] = BufferPool[1].pBuffer[리턴_배열개수 - 1];
			볼밴_중단[캔들인덱스] = BufferPool[2].pBuffer[리턴_배열개수 - 1];
			볼밴_하단[캔들인덱스] = BufferPool[3].pBuffer[리턴_배열개수 - 1];
		}
	}
	//디뷰("볼밴 %0.0f, %0.0f, %0.0f", 볼밴_상단[캔들인덱스], 볼밴_중단[캔들인덱스], 볼밴_하단[캔들인덱스]);
#endif
#if defined(_사용_기본보조지표_)
	int 스토캐스틱길이 = 22;
	소스배열길이 = (스토캐스틱길이 * 6);
	if (캔들카운트 > 소스배열길이)
	{
		BufferPool[0].Init();
		BufferPool[1].Init();
		BufferPool[2].Init();
		BufferPool[3].Init();
		BufferPool[4].Init();
		size_t 배열인덱스 = 캔들카운트 - 소스배열길이;
		for (size_t 버퍼배열인덱스 = 0; 버퍼배열인덱스 < 소스배열길이; 버퍼배열인덱스++)
		{
			BufferPool[0].pBuffer[버퍼배열인덱스] = 고가[배열인덱스];
			BufferPool[1].pBuffer[버퍼배열인덱스] = 저가[배열인덱스];
			BufferPool[2].pBuffer[버퍼배열인덱스] = 종가[배열인덱스];
			배열인덱스++;
		}
		리턴_배열인덱스 = 0;
		리턴_배열개수 = 0;
		long res = TA_STOCH(
			0
			, 소스배열길이 - 1
			, BufferPool[0].pBuffer
			, BufferPool[1].pBuffer
			, BufferPool[2].pBuffer
			, 스토캐스틱길이
			, 5
			, (TA_MAType)TA_MAType_WMA
			, 5
			, (TA_MAType)TA_MAType_WMA
			, &리턴_배열인덱스
			, &리턴_배열개수
			, BufferPool[3].pBuffer
			, BufferPool[4].pBuffer
		);
		if (TA_SUCCESS == res)
		{	// 마지막 것만 추가한다.
			스토캐스틱빠른선[캔들인덱스] = BufferPool[3].pBuffer[리턴_배열개수 - 1];
			스토캐스틱느린선[캔들인덱스] = BufferPool[4].pBuffer[리턴_배열개수 - 1];
		}
	}
	int RSI_14_길이 = 14;
	소스배열길이 = (RSI_14_길이 * 6);
	if (캔들카운트 > 소스배열길이)
	{
		BufferPool[0].Init();
		BufferPool[1].Init();
		size_t 배열인덱스 = (캔들카운트 - 소스배열길이);
		for (size_t 버퍼배열인덱스 = 0; 버퍼배열인덱스 < 소스배열길이; 버퍼배열인덱스++)
		{
			BufferPool[0].pBuffer[버퍼배열인덱스] = 종가[배열인덱스++];
		}
		리턴_배열인덱스 = 0;
		리턴_배열개수 = 0;
		long res = TA_RSI(0, 소스배열길이 - 1, BufferPool[0].pBuffer, RSI_14_길이, &리턴_배열인덱스, &리턴_배열개수, BufferPool[1].pBuffer);
		if (TA_SUCCESS == res)
		{
			상대강도지수14[캔들인덱스] = BufferPool[1].pBuffer[리턴_배열개수 - 1];
		}
	}
	int MACD길이 = 26;
	소스배열길이 = (MACD길이 * 6);
#if defined(_사용_기본MACD_)
	if (캔들카운트 > 소스배열길이)
	{
		BufferPool[0].Init();
		BufferPool[1].Init();
		BufferPool[2].Init();
		BufferPool[3].Init();
		size_t 배열인덱스 = 캔들카운트 - 소스배열길이;
		for (size_t 버퍼배열인덱스 = 0; 버퍼배열인덱스 < 소스배열길이; 버퍼배열인덱스++)
		{
			BufferPool[0].pBuffer[버퍼배열인덱스] = 종가[배열인덱스++];
		}
		리턴_배열인덱스 = 0;
		리턴_배열개수 = 0;
		long res = TA_MACD(0, 소스배열길이 - 1, BufferPool[0].pBuffer, 12, 26, 9, &리턴_배열인덱스, &리턴_배열개수, BufferPool[1].pBuffer, BufferPool[2].pBuffer, BufferPool[3].pBuffer);
		if (TA_SUCCESS == res)
		{	// 마지막 것만 추가한다.
			MACD빠른선[캔들인덱스] = BufferPool[1].pBuffer[리턴_배열개수 - 1];
			MACD느린선[캔들인덱스] = BufferPool[2].pBuffer[리턴_배열개수 - 1];
			HistLine[캔들인덱스] = BufferPool[3].pBuffer[리턴_배열개수 - 1];
		}
	}
#else
	if (캔들카운트 > 소스배열길이)
	{
		BufferPool[0].Init();
		BufferPool[1].Init();
		size_t 배열인덱스 = (캔들카운트 - 소스배열길이);
		for (size_t 버퍼배열인덱스 = 0; 버퍼배열인덱스 < 소스배열길이; 버퍼배열인덱스++)
		{
			BufferPool[0].pBuffer[버퍼배열인덱스] = 종가[배열인덱스++];
		}
		리턴_배열인덱스 = 0;
		리턴_배열개수 = 0;
		long res = TA_WMA(0, 소스배열길이 - 1, BufferPool[0].pBuffer, 12, &리턴_배열인덱스, &리턴_배열개수, BufferPool[1].pBuffer);
		if (TA_SUCCESS == res)
		{
			double 결과1 = BufferPool[1].pBuffer[리턴_배열개수 - 1];
			res = TA_WMA(0, 소스배열길이 - 1, BufferPool[0].pBuffer, 26, &리턴_배열인덱스, &리턴_배열개수, BufferPool[1].pBuffer);
			if (TA_SUCCESS == res)
			{
				MACD빠른선[캔들인덱스] = 결과1 - BufferPool[1].pBuffer[리턴_배열개수 - 1];
			}
		}
	}
	소스배열길이 += 9;
	if (캔들카운트 > 소스배열길이)
	{
		BufferPool[0].Init();
		BufferPool[1].Init();
		size_t 배열인덱스 = (캔들카운트 - 소스배열길이);
		for (size_t 버퍼배열인덱스 = 0; 버퍼배열인덱스 < 소스배열길이; 버퍼배열인덱스++)
		{
			BufferPool[0].pBuffer[버퍼배열인덱스] = MACD빠른선[배열인덱스++];
		}
		long res = TA_WMA(0, 소스배열길이 - 1, BufferPool[0].pBuffer, 9, &리턴_배열인덱스, &리턴_배열개수, BufferPool[1].pBuffer);
		if (TA_SUCCESS == res)
		{
			MACD느린선[캔들인덱스] = BufferPool[1].pBuffer[리턴_배열개수 - 1];
		}
	}
#endif
#endif
	return(리턴);
}

void _OBJECT_CHANGED_RECORD::pop_front()
{
	if (_MAX_ARRAY_BUFFER_SIZE_ < 캔들카운트)
	{
		베팅플래그.pop_front();
		날짜.pop_front();
		시간.pop_front();
		종가.pop_front();
		시가.pop_front();
		고가.pop_front();
		저가.pop_front();
		거래량.pop_front();
		고저종.pop_front();
		시고저종.pop_front();

		SWMA.pop_front();
#if defined(_사용_볼린저밴드_)
		볼밴_중단.pop_front();
		볼밴_상단.pop_front();
		볼밴_하단.pop_front();
#endif
#if defined(_사용_기본보조지표_)
		스토캐스틱빠른선.pop_front();
		스토캐스틱느린선.pop_front();
		상대강도지수14.pop_front();
		MACD빠른선.pop_front();
		MACD느린선.pop_front();
		HistLine.pop_front();
#endif
		캔들카운트--;
		캔들인덱스 = (캔들카운트 - 1);		// pop_front 를 했으면 nCount 를 하나 빼고 캔들인덱스도 감소.
	}
}

