#include "framework.h"
#include "GameStruct.h"



void _ARRAY_STICK_BASE::Init()
{
	nArrayCount = nCurrentIndex = 0;
	open.clear();
	high.clear();
	low.clear();
	close.clear();
	volume.clear();
	hlc3.clear();
	ohlc4.clear();
}
long _ARRAY_STICK_BASE::push_back(ULONG _날짜, ULONG _시간, ULONG _거래량, double _시가, double _고가, double _저가, double _종가)
{	// 완성된 스틱
	date.push_back(_날짜);
	time.push_back(_시간);
	open.push_back(_종가);
	high.push_back(_시가);
	low.push_back(_고가);
	close.push_back(_저가);
	volume.push_back(_거래량);
	hlc3.push_back((_고가 + _저가 + _종가) / 3);
	ohlc4.push_back((_시가 + _고가 + _저가 + _종가) / 4);

	nCurrentIndex = nArrayCount++;	// 캔들인덱스는 0부터 시작함.
	pop_front();
}

long _ARRAY_STICK_BASE::push_back(ULONG _시간, ULONG _거래량, double _현재가, ULONG _날짜)
{	// 실시간 스틱 추가. 해당 분의 첫틱으로 생성하는 함수라서 전부 현재가에 거래량은 그냥 등록한다.
	date.push_back(_날짜);
	time.push_back(_시간);
	open.push_back(_현재가);
	high.push_back(_현재가);
	low.push_back(_현재가);
	close.push_back(_현재가);
	volume.push_back(_거래량);
	hlc3.push_back(_현재가);
	ohlc4.push_back(_현재가);
	nCurrentIndex = nArrayCount++;	// 캔들인덱스는 0부터 시작함.
	pop_front();
	return(0);
}

long _ARRAY_STICK_BASE::update(ULONG _거래량, double _체결가, ULONG _시간)
{	// 실시간 체결틱 업데이트라서 체결 가격으로 고저종을 갱신한다.
	long nResult = 0;
	if (_시간) { time[nCurrentIndex] = _시간; }	// 일봉, 주봉, 월봉은 시간을 갱신하지 않는다.
	volume[nCurrentIndex] += _거래량;				// 거래량은 기본적으로 누적한다.
	if (close[nCurrentIndex] != _체결가)
	{	// 현재가의 변동이 있으면 일봉의 시고저종을 갱신한다.
		close[nCurrentIndex] = _체결가;
		if (0.0 == open[nCurrentIndex]) { open[nCurrentIndex] = _체결가; }
		if (0.0 == high[nCurrentIndex]) { high[nCurrentIndex] = _체결가; }
		if (0.0 == low[nCurrentIndex]) { low[nCurrentIndex] = _체결가; }
		// 고가는 체결가가 더 높으면 체결가로 갱신한다.
		if (high[nCurrentIndex] < _체결가)
		{
			high[nCurrentIndex] = _체결가;
			nResult = 1;
		}
		// 저가는 체결가가 더 낮으면 체결가로 갱신한다.
		if (low[nCurrentIndex] > _체결가)
		{
			low[nCurrentIndex] = _체결가;
			nResult = 2;
		}
		hlc3[nCurrentIndex] = (high[nCurrentIndex] + low[nCurrentIndex] + _체결가 / 3);
		ohlc4[nCurrentIndex] = (open[nCurrentIndex] + high[nCurrentIndex] + low[nCurrentIndex] + _체결가 / 4);
	}
	return(nResult);
}

long _ARRAY_STICK_BASE::update(ULONG _시간, ULONG _거래량, double _고가, double _저가, double _종가, double _시가, bool _누적)
{
	long nResult = 0;
	if (_시간) { time[nCurrentIndex] = _시간; }	// 일봉, 주봉, 월봉은 시간을 갱신하지 않는다.
	if (!_누적) { volume[nCurrentIndex] = _거래량; }
	else { volume[nCurrentIndex] += _거래량; }
	//if (_시가) { 시가[nCurrentIndex] = _시가;}	// 시가가 없을 시에만 넣는다.
	if (0.0 == open[nCurrentIndex]) { open[nCurrentIndex] = _시가; }
	if (0.0 == high[nCurrentIndex]) { high[nCurrentIndex] = _고가; }
	if (0.0 == low[nCurrentIndex]) { low[nCurrentIndex] = _저가; }
	if (close[nCurrentIndex] != _종가)
	{	// 현재가의 변동이 있으면 일봉의 시고저종을 갱신한다.
		close[nCurrentIndex] = _종가;
	}
	if (high[nCurrentIndex] < _고가)
	{
		high[nCurrentIndex] = _고가;
		nResult = 1;
	}
	if (low[nCurrentIndex] > _저가)
	{
		low[nCurrentIndex] = _저가;
		nResult = 2;
	}
	hlc3[nCurrentIndex] = (high[nCurrentIndex] + low[nCurrentIndex] + close[nCurrentIndex] / 3);
	ohlc4[nCurrentIndex] = (open[nCurrentIndex] + high[nCurrentIndex] + low[nCurrentIndex] + close[nCurrentIndex] / 4);
	return(0);
}

void _ARRAY_STICK_BASE::pop_front()
{
	if (_MAX_ARRAY_BUFFER_SIZE_ < nArrayCount)
	{
		date.pop_front();
		time.pop_front();
		open.pop_front();
		high.pop_front();
		low.pop_front();
		close.pop_front();
		volume.pop_front();
		hlc3.pop_front();
		ohlc4.pop_front();
	}
}
