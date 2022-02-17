#include "framework.h"
#include "CObjectMonster.h"



C_OBJECT_MONSTER::C_OBJECT_MONSTER(ULONG _nDateToday)
	: 누적매수거래횟수(0)
	, bControlKind(0)
	, nCurrentTime(0)
	, nDateToday(_nDateToday)
{
	//pEngine = fb::C_MAIN::GetInstance();
	//pFileManager = pEngine->GetFileMgr();

	ZeroMemory(&stickToday, sizeof(_STICK_DATA));
	ZeroMemory(szState, _countof(szState));
	ZeroMemory(szControlKind, _countof(szControlKind));

	for (size_t i = 0; i < _countof(bState); i++)
	{
		bState[i] = false;
	}
	C_OBJECT_BASE_ACTIVE::Init();
	/*
	vStickMinute1.resize(10000);
	DBGPRINT("vStickMinute1.size(): %d", vStickMinute1.size());
	for (size_t i = 0; i < vStickMinute1.size(); i++)
	{
		vStickMinute1[i] = nullptr;
	}
	*/
}

C_OBJECT_MONSTER::~C_OBJECT_MONSTER()
{

}

void C_OBJECT_MONSTER::Init(LPSTR _종목코드)
{
	C_OBJECT_BASE::SetItemCode(_종목코드);
	누적체결건수 = 누적매도체결건수 = 누적매수체결건수 = 0;
	누적매도체결량 = 누적매수체결량 = 누적체결량 = 0;
}

void C_OBJECT_MONSTER::Init_Creon(LPSTR _종목코드, LPSTR _크레온코드, LPSTR _풀코드, LPSTR _종목명)
{
	C_OBJECT_BASE::SetItemCode(_종목코드);
	C_OBJECT_BASE::SetItemCreonCode(_크레온코드);
	C_OBJECT_BASE::SetItemFullCode(_풀코드);
	C_OBJECT_BASE::SetItemName(_종목명);
}

void C_OBJECT_MONSTER::Init_Kiwoom(LPSTR _종목코드, LPSTR _종목명, LPSTR _종목상태, LPSTR _감리구분)
{
	C_OBJECT_BASE::SetItemCode(_종목코드);
	if (_종목명)
	{
		C_OBJECT_BASE::SetItemName(_종목명);
	}
	//::strcpy_s(종목코드, _종목코드);
	if (_종목상태)
	{
		// 어차피 순식간인데 그냥 복사만 해두자 나중에 확인을 하든. 지우든.
		::strcpy_s(szState, _종목상태);
		// 종목 상태는 복수 적용
		// 정상, 증거금100%, 거래정지, 관리종목, 감리종목, 투자유의종목,	담보대출, 액면분할, 신용가능
		if (::strstr(_종목상태, "정상")) { bState[0] = true; }
		if (::strstr(_종목상태, "거래정지")) { bState[1] = true; }
		if (::strstr(_종목상태, "관리종목")) { bState[2] = true; }
		if (::strstr(_종목상태, "감리종목")) { bState[3] = true; }
		if (::strstr(_종목상태, "투자유의종목")) { bState[4] = true; }
		if (::strstr(_종목상태, "담보대출")) { bState[5] = true; }
		if (::strstr(_종목상태, "액면분할")) { bState[6] = true; }
		if (::strstr(_종목상태, "신용가능")) { bState[7] = true; }

		//strState = _szState;
		// 증거금을 찾고
		LPCSTR strFind1 = ::strstr(_종목상태, "증거금");
		if (strFind1)
		{	// 있으면 증거금 뒤로 포인터 이동.
			strFind1 += 6;
			// % 검색
			LPCSTR strFind2 = ::strrchr(_종목상태, '%');
			// 증거금 얻기
			char szBuf[4] = { 0 };
			::strncpy_s(szBuf, strFind1, strFind2 - strFind1);
			nEvidence_kiw = (BYTE)::atoi(szBuf);
		}
	}
	if (_감리구분)
	{
		::strcpy_s(szControlKind, _감리구분);
		// 감리구분은 한가지다. 
		//szControlKind = _szControlKind;
		if (::strstr(_감리구분, "정상")) { bControlKind = 0; }
		else if (::strstr(_감리구분, "투자주의")) { bControlKind = 1; }
		else if (::strstr(_감리구분, "투자경고")) { bControlKind = 2; }
		else if (::strstr(_감리구분, "투자위험")) { bControlKind = 3; }
		else { bControlKind = 4; }

	}
	/*
	DBGPRINT("[ENGINE] [%s] 종목이름: %s / 감리구분: %s / 상태: %s"
		, _szStockCode
		, _szStockName
		, _szControlKind
		, _szState
	);
	*/
}

void C_OBJECT_MONSTER::InitSticks()
{

}

void C_OBJECT_MONSTER::PushTick(LPTICK_DATA _pTick)
{	// 여기는 파일에서 읽은게 들어오는거다.
	// 위에서 pStock 가 없으면 생성하므로 분기를 제거.
	//try
	{
		vTransactionTicks.push_back(_pTick);	// 

	///////////////////////////////////////////////////////////////////////////////
	// 일단 해당 종목의 일봉의 시고저종을 갱신한다.
	///////////////////////////////////////////////////////////////////////////////
		if (!일봉.size())
		{
			일봉.push_back(_pTick->nTime, _pTick->nTransVolume, (double)_pTick->fPrice);
		}
		else
		{
			long nResult = 일봉.update(_pTick->nTransVolume, (double)_pTick->fPrice);
			if (1 == nResult)
			{
				고가시간 = _pTick->nTime;
			}
			else if (2 == nResult)
			{
				저가시간 = _pTick->nTime;
			}
		}
		// 들어온 시간, 분을 구해서 초로 만든다.
		//char szBuffer[(1 << 4)] = { 0 };
		//sprintf_s(szBuffer, "%09d", _pTick->nTime);		// 9000000, 11034600, 12593100, 15300000
		//int nHour = ((((szBuffer[0] - 0x30) * 10) + ((szBuffer[1] - 0x30))) * 3600);	// 09 는 32400
		//int nMinute = ((((szBuffer[2] - 0x30) * 10) + ((szBuffer[3] - 0x30))) * 60);	// 30 은 1800
		//int nSecond = (((szBuffer[4] - 0x30) * 10) + ((szBuffer[5] - 0x30)));	// 30 은 1800
		//int nMicroSecond = (((szBuffer[6] - 0x30) * 100) + ((szBuffer[7] - 0x30) * 10) + (szBuffer[8] - 0x30));	// 32400, 32460, 32520, 32580

		//ULONG 현재_시 = (((nHour + nMinute) / 60) / 60);
		//ULONG 현재_분 = (((nHour + nMinute) / 60) % 60);

		//디뷰("[%s] (%02d:%02d:%02d.%03d) -%s- 가격: %0.0f \t\t\t거래량: (%d/%d)"
		//	, _pTick->szCode
		//	, 현재_시
		//	, 현재_분
		//	, nSecond
		//	, nMicroSecond
		//	, 1 == _pTick->nTransType ? "매수" : 2 == _pTick->nTransType ? "매도" : "단일"
		//	, _pTick->fPrice
		//	, _pTick->nTransVolume
		//	, _pTick->nAccrueVolume
		//	);
#if defined(_MAKE_MINITE_STICKS_)
	///////////////////////////////////////////////////////////////////////////////
	// 여기서는 분봉을 만든다
	///////////////////////////////////////////////////////////////////////////////


	// 시간하고 분을 합한다. 
		ULONG nTime1 = (nHour / 36) + ((nSecond1 / 60) % 60);	// 900, 901, 902, 903
		// 만약 900 이면, nStickSizeMinute1 + 0, 901 이면 nStickSizeMinute1 + 1
		// 959 라면 nStickSizeMinute1 + 59, 1000 이면, 60 이어야한다.
		// nSecond1 은 0 에서 59를 반복한다, nStickCountMinute1 는 봉 한개마다 1씩 증가한다.
		// [nStickSizeMinute1 + nStickCountMinute1];

		// 생성된 해당 분의 1분봉 구조체를 얻어온다.
		LPSTICK_DATA pStickData1 = umapStickMinute1[nTime1];
		if (!pStickData1)
		{	// 매 분마다 새로 생성하게 되니 이거슨 1분봉.
			//char szTime[(1 << 4)] = { 0 };
			//sprintf_s(szTime, "%d", nTime1);
			//DBGPRINT("%d / %s", nTime1, szTime);
			pStickData1 = umapStickMinute1[nTime1] = new STICK_DATA(_pTick->fPrice, nTime1, nDateToday);		// 시고저종을 모두 체결가로. ( 시가는 초기에 세팅한 후 변하지 않음 )
			// 새로 생성하면 순서대로 저장하기 위해서 벡터에도 보관한다.
			vStickMinute1.push_back(pStickData1);
			//DBGPRINT("[%s] %d 분봉 만든다", szStockCode, nTime1);
		}
		else
		{	// 있으면 고저종만 갱신한다. ( 시가는 생성할떄 들어감 )
			pStickData1->Update(_pTick->fPrice);
		}
#endif
		///////////////////////////////////////////////////////////////////////////////
			// 누적 및 업데이트
			///////////////////////////////////////////////////////////////////////////////
		__int64 ullTodayClose = (__int64)_pTick->fPrice;
		if (1 == _pTick->nTransType)	// 3항 반복해도 되지만, 분기는 한번만 하자.
		{	// 각 분봉의 매수 횟수 누적
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->nAccrueNumberofTimesBuy++;
#endif
			stickToday.nAccrueNumberofTimesBuy++;
			// 각 분봉의 매수 거래량 누적
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->누적매수체결량 += _pTick->nTransVolume;
#endif
			stickToday.누적매수체결량 += _pTick->nTransVolume;
			// 각 분봉의 매수거래대금 누적 ( 체결가 x 체결량 = 매수금액 )
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->nAccruePaymentBuy += (ullTodayClose * _pTick->nTransVolume);
#endif
			stickToday.nAccruePaymentBuy += (ullTodayClose * _pTick->nTransVolume);
			// 각 분봉의 매수 평균 가격 구하기 ( 매수금액 / 매수체결량 = 평균 매수 가격 )
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->nAveragePaymentBuy = (ULONG)(pStickData1->nAccruePaymentBuy / pStickData1->누적매수체결량);
#endif
			stickToday.nAveragePaymentBuy = (ULONG)(stickToday.nAccruePaymentBuy / stickToday.누적매수체결량);
			//DBGPRINT("[ENGINE] (%s) [%s:매수] 거래가: %d, 거래량: %d / 매수평균가: %d / 매수거래대금: %lld"
			//	, _szTime, szStockCode, _nClose, _nVolume, stickToday.nAveragePaymentBuy, stickToday.nAccruePaymentBuy);
			누적매수거래횟수++;
		}
		else// if (2 == _bytType)												// 매수가 아니면 전부 매도로 판단한다
		{	// 각 분봉의 매도 횟수 누적
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->nAccrueNumberofTimesSell++;
#endif
			stickToday.nAccrueNumberofTimesSell++;
			// 각 분봉의 매도 거래량 누적
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->누적매도체결량 += _pTick->nTransVolume;
#endif
			stickToday.누적매도체결량 += _pTick->nTransVolume;
			// 각 분봉의 매도거래대금 누적 ( 체결가 x 체결량 = 매도금액 )
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->nAccruePaymentSell += (ullTodayClose * _pTick->nTransVolume);
#endif
			stickToday.nAccruePaymentSell += (ullTodayClose * _pTick->nTransVolume);
			// 각 분봉의 매도 평균 가격 구하기 ( 매수금액 / 매도체결량 = 평균 매도 가격 )
#if defined(_MAKE_MINITE_STICKS_)
			pStickData1->nAveragePaymentSell = (ULONG)(pStickData1->nAccruePaymentSell / pStickData1->누적매도체결량);
#endif
			stickToday.nAveragePaymentSell = (ULONG)(stickToday.nAccruePaymentSell / stickToday.누적매도체결량);
			누적매도거래횟수++;
			//DBGPRINT("[ENGINE] (%s) [%s:매도] 거래가: %d, 거래량: %d / 매도평균가: %d / 매도거래대금: %lld"
			//	, _szTime, szStockCode, _nClose, _nVolume, stickToday.nAveragePaymentSell, stickToday.nAccruePaymentSell);
		}
		
		//else if (3 == _bytType)	// 여기선 무조건 매도로 간주한다.
		//{
		//	//DBGPRINT("[ENGINE] 단일가매매 종목: %s / 시간: %s", szStockCode, _szTime);
		//	nSingleAuction = true;
		//	//pSaveToFile->bForceWrite = true;
		//}
		//else
		//{	// 여기는 절대 오면 안된다.
		//	DBGPRINT("[ENGINE] 아 여기는 절대 오면 안된다니까");
		//}


		//info.누적매수체결량 = stickToday.누적매수체결량;
		//info.nAccruePaymentBuy = stickToday.nAccruePaymentBuy;
		//info.nAveragePaymentBuy = stickToday.nAveragePaymentBuy;

		//info.누적매도체결량 = stickToday.누적매도체결량;
		//info.nAccruePaymentSell = stickToday.nAccruePaymentSell;
		//info.nAveragePaymentSell = stickToday.nAveragePaymentSell;

		//info.nAccrueVolumeBuySell = (stickToday.누적매수체결량 - stickToday.누적매도체결량);		// 매수거래량 - 매도거래량 = 이건 거래대금보단 못할텐데 흠.
		//info.nAccruePaymentBuySell = (stickToday.nAccruePaymentBuy - stickToday.nAccruePaymentSell);	// 매수거래대금 - 매도거래대금 = 매수 강도 측정
		// 전광판에 줄 때는 거래대금에서 백단위는 버리고 준다
		//pNoticeBoardRowData->nAccruePaymentBuy = (__int64)(stickToday.nAccruePaymentBuy / 1000);
		// 매수거래량 - 매도거래량 = 이건 거래대금보단 못할텐데 흠.
		//pNoticeBoardRowData->nAccrueVolumeBuySell = (long)(stickToday.누적매수체결량 - stickToday.누적매도체결량);
		// 매수거래대금 - 매도거래대금 = 매수 강도 측정
		//pNoticeBoardRowData->nAccruePaymentBuySell = (long)((stickToday.nAccruePaymentBuy - stickToday.nAccruePaymentSell) / 1000);
		///////////////////////////////////////////////////////////////////////////////
		// 거래량을 확인해서 누적이 있었는지 확인하자.
		///////////////////////////////////////////////////////////////////////////////
		// 누적 거래대금은
		stickToday.nAccruePaymentTotal += (__int64)((DWORD)(_pTick->fPrice) * _pTick->nTransVolume);	// 현재 체결가 x 체결량
		///////////////////////////////////////////////////////////////////////////////
		// 누적 거래량 갱신.
		///////////////////////////////////////////////////////////////////////////////
		// 각 분봉에 체결량을 누적
#if defined(_MAKE_MINITE_STICKS_)
		pStickData1->nAccrueVolumeTotal += _pTick->nTransVolume;
#endif

		if (1 == _pTick->nTransType)
		{
			누적매수체결건수++;
			누적매수체결량 += _pTick->nTransVolume;

		}
		else if (2 == _pTick->nTransType)
		{
			누적매도체결건수++;
			누적매도체결량 += _pTick->nTransVolume;
		}
		if (0 < 누적체결량)
		{// 당일 누적 체결이 있고, 
			if ((누적체결량 + _pTick->nTransVolume) != _pTick->nAccrueVolume)
			{	// (이전틱누적 + 현재틱거래량)과 현재틱누적이 다르면
				if (누적매도체결건수 || 누적매수체결건수)
				{	// 매도나 매수가 발생한적이 있는 경우
					디뷰("일반 누락발생 %d / %d", 누적매도체결건수, 누적매수체결건수);
					디뷰("C_OBJECT_MONSTER::PushTick(LPTICK_DATA) - [누락/일반] 종목: %s / 시간: %d / 이전틱누적: %d + 현재거래량: %d = 현재틱누적: %d / 이전틱+현재거래량: %d / 차이: %d"
						, _pTick->szCode
						, _pTick->nTime
						, 누적체결량							// 이전까지의 누적
						, _pTick->nTransVolume					// 현재 거래량
						, _pTick->nAccrueVolume					// 현재틱에 포함되어 오는 누적 거래량
						, (누적체결량 + _pTick->nTransVolume)
						, (_pTick->nAccrueVolume - (누적체결량 + _pTick->nTransVolume))	// 차이 = 틱의누적량 - (이전틱의누적 + 현재틱거래량)
					);
				}
				else
				{	// 매도나 매수 체결이 발생한적이 없다면 단일가 매매만 진행하는 종목이다.
					디뷰("C_OBJECT_MONSTER::PushTick(LPTICK_DATA) - [누락/단일] 종목: %s / 시간: %d / 이전틱누적: %d + 현재틱거래량: %d = 현재틱누적: %d / 이전틱+현재거래량: %d / 차이: %d"
						, _pTick->szCode, _pTick->nTime, 누적체결량, _pTick->nTransVolume, _pTick->nAccrueVolume, (누적체결량 + _pTick->nTransVolume)
						, (_pTick->nAccrueVolume - (누적체결량 + _pTick->nTransVolume))	// 차이 = 현재틱누적 - (이전틱누적 + 현재틱거래량)
					);
				}
			}
		}
		누적체결량 = _pTick->nAccrueVolume;
		// 각 분봉에 호가 잔량 갱신
#if defined(_MAKE_MINITE_STICKS_)
		pStickData1->nRemainVolumeTotalBuy = 매수호가총잔량;
		pStickData1->nRemainVolumeTotalSell = 매도호가총잔량;
#endif
		//stickToday.nRemainVolumeTotalBuy = 매수호가총잔량;
		//stickToday.nRemainVolumeTotalSell = 매도호가총잔량;
		// 체결 강도 갱신
		float 매수체결합 = (float)누적매수체결량;
		float 매도체결합 = (float)누적매도체결량;
		체결강도 = ((매수체결합 - 매도체결합) / (매수체결합 + 매도체결합) * 100);
		//디뷰("[%s] 체강: %0.2f", _pTick->szCode, 체결강도);
		//체결강도 = (float)((누적매수체결량 / 누적매도체결량) * 100);
	}
	//catch (...)
	//{
	//	디뷰위치();
	//}
}

void C_OBJECT_MONSTER::Update(
	LPSTR _szTime			// 체결시간
	, BYTE _bytType			// 체결타입
	, double _dbPrice		// 체결가격
	, ULONG _nVolume		// 체결량
	, ULONG _nAccrueVolume	// 당일 누적 체결량
	, float _fStrength		// 당일 체결강도
	, LPSTR _szOpen
	, LPSTR _szHigh
	, LPSTR _szLow
)
{	// 여기는 실시간 체결 데이터가 들어오는거다.
	_szTime;
	_bytType;
	_nAccrueVolume;
	_fStrength;
	_szOpen;
	_szHigh;
	_szLow;

	try
	{
		///////////////////////////////////////////////////////////////////////////////
		// 일단 해당 종목의 시고저종을 갱신한다.
		///////////////////////////////////////////////////////////////////////////////
		일봉.update(_nVolume, _dbPrice);
		/*
		if (stickToday.nClose != _nClose)											// 체결가의 변동이 있으면? 처음엔 stickToday.nClose 가 0 이라 들어온다.
		{
			// 현재가는 변동이 있을때만 갱신한다.
			stickToday.nClose = _nClose;
			// 만약 장중에 프로그램을 가동할 경우 얻어온걸 써야한다.
			if (0 == stickToday.nOpen) { stickToday.nOpen = dk::atoi_s(_szOpen); }	// 시가는 변동이 없으니 한번만 들어온다.	(분기문이 atoi 반복 호출보다 싸게 먹힌다)
			if (0 == stickToday.nHigh) { stickToday.nHigh = dk::atoi_s(_szHigh); }
			if (0 == stickToday.nLow) { stickToday.nLow = dk::atoi_s(_szLow); }
			// 고가는 체결가가 더 높으면 체결가로 갱신한다.
			if (stickToday.nHigh < _nClose) { stickToday.nHigh = _nClose; }
			// 저가는 체결가가 더 낮으면 체결가로 갱신한다.
			if (stickToday.nLow > _nClose) { stickToday.nLow = _nClose; }
		}
		//info.nClose = stickToday.nClose;
		//info.nOpen = stickToday.nOpen;
		//info.nHigh = stickToday.nHigh;
		//info.nLow = stickToday.nLow;
		///////////////////////////////////////////////////////////////////////////////
		// 여기서는 분봉을 만든다
		///////////////////////////////////////////////////////////////////////////////
			// 들어온 시간, 분을 구해서 초로 만든다.
		int nHour = ((((_szTime[0] - 0x30) * 10) + ((_szTime[1] - 0x30))) * 3600);	// 09 는 32400
		int nMinute = ((((_szTime[2] - 0x30) * 10) + ((_szTime[3] - 0x30))) * 60);	// 30 은 1800
		int nSecond1 = (nHour + nMinute);	// 32400, 32460, 32520, 32580

		// 시간하고 분을 합한다. 
		ULONG nTime1 = (nHour / 36) + ((nSecond1 / 60) % 60);	// 900, 901, 902, 903
		// 만약 900 이면, nStickSizeMinute1 + 0, 901 이면 nStickSizeMinute1 + 1
		// 959 라면 nStickSizeMinute1 + 59, 1000 이면, 60 이어야한다.
		// nSecond1 은 0 에서 59를 반복한다, nStickCountMinute1 는 봉 한개마다 1씩 증가한다.
		// [nStickSizeMinute1 + nStickCountMinute1];

		// 생성된 해당 분의 1분봉 구조체를 얻어온다.
		LPSTICK_DATA pStickData1 = umapStickMinute1[nTime1];
		if (!pStickData1)
		{	// 매 분마다 새로 생성하게 되니 이거슨 1분봉.
			//char szTime[(1 << 4)] = { 0 };
			//sprintf_s(szTime, "%d", nTime1);
			//DBGPRINT("%d / %s", nTime1, szTime);
			pStickData1 = umapStickMinute1[nTime1] = new STICK_DATA(_nClose, nTime1, nDateToday);		// 시고저종을 모두 체결가로. ( 시가는 초기에 세팅한 후 변하지 않음 )
			// 새로 생성하면 순서대로 저장하기 위해서 벡터에도 보관한다.
			vStickMinute1.push_back(pStickData1);
			//DBGPRINT("[%s] %d 분봉 만든다", szStockCode, nTime1);
		}
		else
		{	// 있으면 고저종만 갱신한다. ( 시가는 생성할떄 들어감 )
			pStickData1->Update(_nClose);
		}
		///////////////////////////////////////////////////////////////////////////////
		// 3분봉
		///////////////////////////////////////////////////////////////////////////////
			// 초를 각 3분단위로 잘라낸다.
		int nSecond3 = (nSecond1 - (nSecond1 % 180));	// 32400 - 32400 % 0
		// 분으로 변환하고 시간이랑 합친다 0900 -> 900, 0938 -> 930
		ULONG nTime3 = (nHour / 36) + ((nSecond3 / 60) % 60);
		// 생성된 3분봉이 있나?
		LPSTICK_DATA pStickData3 = umapStickMinute3[nTime3];
		if (nullptr == pStickData3)
		{	// 3분마다 새로 생성하게 되니 이거슨 3분봉.
			pStickData3 = umapStickMinute3[nTime3] = new STICK_DATA(_nClose, nTime3, nDateToday);		// 시고저종을 pSaveToFile->nPrice 로 세팅
			vStickMinute3.push_back(pStickData3);
		}
		else
		{	// 있으면 고저종만 갱신한다. ( 시가는 생성할떄 들어감 )
			pStickData3->Update(_nClose);
		}
		///////////////////////////////////////////////////////////////////////////////
		// 5분봉
		///////////////////////////////////////////////////////////////////////////////
			// 초를 각 5분단위로 잘라낸다.
		int nSecond5 = (nSecond1 - (nSecond1 % 300));	// 32400 - 32400 % 0
		// 분으로 변환하고 시간이랑 합친다 0900 -> 900, 0938 -> 930
		ULONG nTime5 = (nHour / 36) + ((nSecond5 / 60) % 60);
		// 생성된 5분봉이 있나?
		LPSTICK_DATA pStickData5 = umapStickMinute5[nTime5];
		if (nullptr == pStickData5)
		{	// 5분마다 새로 생성하게 되니 이거슨 5분봉.
			pStickData5 = umapStickMinute5[nTime5] = new STICK_DATA(_nClose, nTime5, nDateToday);		// 시고저종을 pSaveToFile->nPrice 로 세팅
			vStickMinute5.push_back(pStickData5);
		}
		else
		{	// 있으면 고저종만 갱신한다. ( 시가는 생성할떄 들어감 )
			pStickData5->Update(_nClose);
		}
		///////////////////////////////////////////////////////////////////////////////
		// 15분봉
		///////////////////////////////////////////////////////////////////////////////
			// 초를 각 15분단위로 잘라낸다.
		int nSecond15 = (nSecond1 - (nSecond1 % 900));
		// 분으로 변환하고 시간이랑 합친다 0900 -> 900, 0938 -> 930
		ULONG nTime15 = (nHour / 36) + ((nSecond15 / 60) % 60);

		LPSTICK_DATA pStickData15 = umapStickMinute15[nTime15];
		if (nullptr == pStickData15)
		{	// 15분마다 새로 생성하게 되니 이거슨 15분봉.
			pStickData15 = umapStickMinute15[nTime15] = new STICK_DATA(_nClose, nTime15, nDateToday);	// 시고저종을 pSaveToFile->nPrice 로 세팅
			vStickMinute15.push_back(pStickData15);
		}
		else
		{	// 있으면 고저종만 갱신한다. ( 시가는 생성할떄 들어감 )
			pStickData15->Update(_nClose);
		}

		///////////////////////////////////////////////////////////////////////////////
		// 누적 및 업데이트
		///////////////////////////////////////////////////////////////////////////////
		__int64 ullTodayClose = (__int64)_nClose;
		if (1 == _bytType)	// 3항 반복해도 되지만, 분기는 한번만 하자.
		{
			// 각 분봉의 매수 횟수 누적
			pStickData1->nAccrueNumberofTimesBuy++;
			pStickData1->nAccrueNumberofTimesBuy++;
			pStickData1->nAccrueNumberofTimesBuy++;
			pStickData1->nAccrueNumberofTimesBuy++;
			stickToday.nAccrueNumberofTimesBuy++;
			// 각 분봉의 매수 거래량 누적
			pStickData1->누적매수체결량 += _nVolume;
			pStickData3->누적매수체결량 += _nVolume;
			pStickData5->누적매수체결량 += _nVolume;
			pStickData15->누적매수체결량 += _nVolume;
			stickToday.누적매수체결량 += _nVolume;
			// 각 분봉의 매수거래대금 누적 ( 체결가 x 체결량 = 매수금액 )
			pStickData1->nAccruePaymentBuy += (ullTodayClose * _nVolume);
			pStickData3->nAccruePaymentBuy += (ullTodayClose * _nVolume);
			pStickData5->nAccruePaymentBuy += (ullTodayClose * _nVolume);
			pStickData15->nAccruePaymentBuy += (ullTodayClose * _nVolume);
			stickToday.nAccruePaymentBuy += (ullTodayClose * _nVolume);
			// 각 분봉의 매수 평균 가격 구하기 ( 매수금액 / 매수체결량 = 평균 매수 가격 )
			pStickData1->nAveragePaymentBuy = (ULONG)(pStickData1->nAccruePaymentBuy / pStickData1->누적매수체결량);
			pStickData3->nAveragePaymentBuy = (ULONG)(pStickData3->nAccruePaymentBuy / pStickData3->누적매수체결량);
			pStickData5->nAveragePaymentBuy = (ULONG)(pStickData5->nAccruePaymentBuy / pStickData5->누적매수체결량);
			pStickData15->nAveragePaymentBuy = (ULONG)(pStickData15->nAccruePaymentBuy / pStickData15->누적매수체결량);
			stickToday.nAveragePaymentBuy = (ULONG)(stickToday.nAccruePaymentBuy / stickToday.누적매수체결량);
			//DBGPRINT("[ENGINE] (%s) [%s:매수] 거래가: %d, 거래량: %d / 매수평균가: %d / 매수거래대금: %lld"
			//	, _szTime, szStockCode, _nClose, _nVolume, stickToday.nAveragePaymentBuy, stickToday.nAccruePaymentBuy);
			누적매수거래횟수++;
		}
		else// if (2 == _bytType)												// 매수가 아니면 전부 매도로 판단한다
		{
			// 각 분봉의 매도 횟수 누적
			pStickData1->nAccrueNumberofTimesSell++;
			pStickData1->nAccrueNumberofTimesSell++;
			pStickData1->nAccrueNumberofTimesSell++;
			pStickData1->nAccrueNumberofTimesSell++;
			stickToday.nAccrueNumberofTimesSell++;
			// 각 분봉의 매도 거래량 누적
			pStickData1->누적매도체결량 += _nVolume;
			pStickData3->누적매도체결량 += _nVolume;
			pStickData5->누적매도체결량 += _nVolume;
			pStickData15->누적매도체결량 += _nVolume;
			stickToday.누적매도체결량 += _nVolume;
			// 각 분봉의 매도거래대금 누적 ( 체결가 x 체결량 = 매도금액 )
			pStickData1->nAccruePaymentSell += (ullTodayClose * _nVolume);
			pStickData3->nAccruePaymentSell += (ullTodayClose * _nVolume);
			pStickData5->nAccruePaymentSell += (ullTodayClose * _nVolume);
			pStickData15->nAccruePaymentSell += (ullTodayClose * _nVolume);
			stickToday.nAccruePaymentSell += (ullTodayClose * _nVolume);
			// 각 분봉의 매도 평균 가격 구하기 ( 매수금액 / 매도체결량 = 평균 매도 가격 )
			pStickData1->nAveragePaymentSell = (ULONG)(pStickData1->nAccruePaymentSell / pStickData1->누적매도체결량);
			pStickData3->nAveragePaymentSell = (ULONG)(pStickData3->nAccruePaymentSell / pStickData3->누적매도체결량);
			pStickData5->nAveragePaymentSell = (ULONG)(pStickData5->nAccruePaymentSell / pStickData5->누적매도체결량);
			pStickData15->nAveragePaymentSell = (ULONG)(pStickData15->nAccruePaymentSell / pStickData15->누적매도체결량);
			stickToday.nAveragePaymentSell = (ULONG)(stickToday.nAccruePaymentSell / stickToday.누적매도체결량);

			누적매도거래횟수++;
			//DBGPRINT("[ENGINE] (%s) [%s:매도] 거래가: %d, 거래량: %d / 매도평균가: %d / 매도거래대금: %lld"
			//	, _szTime, szStockCode, _nClose, _nVolume, stickToday.nAveragePaymentSell, stickToday.nAccruePaymentSell);
		}
		
		//else if (3 == _bytType)	// 여기선 무조건 매도로 간주한다.
		//{
		//	//DBGPRINT("[ENGINE] 단일가매매 종목: %s / 시간: %s", szStockCode, _szTime);
		//	nSingleAuction = true;
		//	//pSaveToFile->bForceWrite = true;
		//}
		//else
		//{	// 여기는 절대 오면 안된다.
		//	DBGPRINT("[ENGINE] 아 여기는 절대 오면 안된다니까");
		//}
		

		//info.누적매수체결량 = stickToday.누적매수체결량;
		//info.nAccruePaymentBuy = stickToday.nAccruePaymentBuy;
		//info.nAveragePaymentBuy = stickToday.nAveragePaymentBuy;

		//info.누적매도체결량 = stickToday.누적매도체결량;
		//info.nAccruePaymentSell = stickToday.nAccruePaymentSell;
		//info.nAveragePaymentSell = stickToday.nAveragePaymentSell;

		//info.nAccrueVolumeBuySell = (stickToday.누적매수체결량 - stickToday.누적매도체결량);		// 매수거래량 - 매도거래량 = 이건 거래대금보단 못할텐데 흠.
		//info.nAccruePaymentBuySell = (stickToday.nAccruePaymentBuy - stickToday.nAccruePaymentSell);	// 매수거래대금 - 매도거래대금 = 매수 강도 측정
		// 전광판에 줄 때는 거래대금에서 백단위는 버리고 준다
		//pNoticeBoardRowData->nAccruePaymentBuy = (__int64)(stickToday.nAccruePaymentBuy / 1000);
		// 매수거래량 - 매도거래량 = 이건 거래대금보단 못할텐데 흠.
		//pNoticeBoardRowData->nAccrueVolumeBuySell = (long)(stickToday.누적매수체결량 - stickToday.누적매도체결량);
		// 매수거래대금 - 매도거래대금 = 매수 강도 측정
		//pNoticeBoardRowData->nAccruePaymentBuySell = (long)((stickToday.nAccruePaymentBuy - stickToday.nAccruePaymentSell) / 1000);

		// 누적 거래대금은
		stickToday.nAccruePaymentTotal += (__int64)(_nClose * _nVolume);	// 현재 체결가 x 체결량
	///////////////////////////////////////////////////////////////////////////////
	// 누적 거래량 갱신.
	///////////////////////////////////////////////////////////////////////////////
		// 각 분봉에 체결량을 누적
		pStickData1->nAccrueVolumeTotal += _nVolume;
		pStickData3->nAccrueVolumeTotal += _nVolume;
		pStickData5->nAccrueVolumeTotal += _nVolume;
		pStickData15->nAccrueVolumeTotal += _nVolume;

		if (stickToday.nAccrueVolumeTotal)
		{	// (이전틱누적 + 현재틱거래량)과 현재틱누적이 다르면
			if ((stickToday.nAccrueVolumeTotal + _nVolume) != _nAccrueVolume)
			{	// 매수 거래가 발생한 적이 있는 경우
				if (0 < 누적매수거래횟수)
				{
					DBGPRINT("[ENGINE] [체결누락] 종목: %s / 시간: %s / 이전틱누적: %d + 현재틱거래량: %d = 현재틱누적: %d / 이전틱+현재거래량: %d / 차이: %d"
						, C_OBJECT_BASE::GetItemCode(), _szTime, stickToday.nAccrueVolumeTotal, _nVolume, _nAccrueVolume, (stickToday.nAccrueVolumeTotal + _nVolume)
						, (_nAccrueVolume - (stickToday.nAccrueVolumeTotal + _nVolume))	// 차이 = 현재틱누적 - (이전틱누적 + 현재틱거래량)
					);
				}
				else // 매수 거래가 발생한 적이 없다면 단일가 매매만 진행되는거임
				{
					DBGPRINT("[ENGINE] [체결누락/단일가] 종목: %s / 시간: %s / 이전틱누적: %d + 현재틱거래량: %d = 현재틱누적: %d / 이전틱+현재거래량: %d / 차이: %d"
						, C_OBJECT_BASE::GetItemCode(), _szTime, stickToday.nAccrueVolumeTotal, _nVolume, _nAccrueVolume, (stickToday.nAccrueVolumeTotal + _nVolume)
						, (_nAccrueVolume - (stickToday.nAccrueVolumeTotal + _nVolume))	// 차이 = 현재틱누적 - (이전틱누적 + 현재틱거래량)
					);
				}
			}
		}
		stickToday.nAccrueVolumeTotal = _nAccrueVolume;													// 누적 거래량 갱신
		// 각 분봉에 호가 잔량 갱신
		pStickData1->nRemainVolumeTotalBuy = 매수호가총잔량;
		pStickData1->nRemainVolumeTotalSell = 매도호가총잔량;

		pStickData3->nRemainVolumeTotalBuy = 매수호가총잔량;
		pStickData3->nRemainVolumeTotalSell = 매도호가총잔량;

		pStickData5->nRemainVolumeTotalBuy = 매수호가총잔량;
		pStickData5->nRemainVolumeTotalSell = 매도호가총잔량;

		pStickData15->nRemainVolumeTotalBuy = 매수호가총잔량;
		pStickData15->nRemainVolumeTotalSell = 매도호가총잔량;

		// 체결 강도 갱신
		if (fTodayStrength != _fStrength) { fTodayStrength = _fStrength; }

		//pEngine->pCallback(_UPDATE_TABLE_ROW_STOCK_INFO_, &info);
		*/
	}
	catch (...)
	{

	}
}

void C_OBJECT_MONSTER::Update(			// 포인터만 넘어오므로 복사가 발생하지 않은 상태.
	LPSTR _szCode						// 체결 된 종목코드
	, LPSTR _szTime						// 해당 종목 현재 체결시간
	, LPSTR _szOpen						// 해당 종목 당일시가
	, LPSTR _szHigh						// 해당 종목 당일고가
	, LPSTR _szLow						// 해당 종목 당일저가
	, LPSTR _szClose					// 해당 종목 현재 체결가
	, LPSTR _szVolume					// 해당 종목 현재 체결량
	, LPSTR _szAccrueVolume				// 해당 종목 당일누적체결량
	, LPSTR _szStrength					// 해당 종목 체결 강도
)
{
	// 이 함수는 메모리 적재 모드가 아닐때만 들어온다.
	try
	{
		//DBGPRINT("[ENGINE] info.szCode: %s", info.szCode);
		// 파일로 저장할 구조체 할당
		LPSAVE_FILE_DATA pSaveToFile = new SAVE_FILE_DATA();
		pSaveToFile->clear();

		strcpy_s(pSaveToFile->szCode, _szCode);

		// 순서 등록
		//pSaveToFile->nSequence = 누적체결건수;
		//DBGPRINT("[ENGINE] pSaveToFile->nSequence: %d -> nCountTransfer: %d", pSaveToFile->nSequence, nCountTransfer);
		// 구조체는 초기화 되고, 체결가는 변동이 있으면 전광판에 업데이트 하므로 변동이 없어도 넣어줘야한다.
		pSaveToFile->nPrice = dk::atoi_s(_szClose);

		//DBGPRINT("[ENGINE] pSaveToFile->nPrice: %d", pSaveToFile->nPrice);
		// 체결 시간을 16진수로 변경하고 endian 변환 ( 시분초에 밀리초를 더한다. )
		SYSTEMTIME cur_time;
		::GetLocalTime(&cur_time);

		char szTime[(1 << 4)] = { 0 };
		sprintf_s(szTime, "%08x", (::atoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pSaveToFile->nTime = ::ntohl(::strtol(szTime, NULL, 16));

		// 틱데이터 저장 구조체는 체결가만 기록한다.
		if (stickToday.nClose != pSaveToFile->nPrice)	// 체결가의 변동이 있으면? 처음엔 stickToday.nClose 가 0 이라 들어온다.
		{
			// 시가는 변동이 없으니 한번만, 고가 저가는 변동이 있을때만 갱신한다.
			if (0 == stickToday.nOpen) { stickToday.nOpen = dk::atoi_s(_szOpen); }						// 분기문이 atoi 반복 호출보다 싸게 먹힌다.
			if (stickToday.nHigh < pSaveToFile->nPrice) { stickToday.nHigh = dk::atoi_s(_szHigh); }		// 기록된 최고가보다 크면 갱신한다.
			if (stickToday.nLow > pSaveToFile->nPrice) { stickToday.nLow = dk::atoi_s(_szLow); }		// 기록된 최저가보다 작으면 갱신한다.
			stickToday.nClose = pSaveToFile->nPrice;
		}

		// 분봉은 스레드로 나눠봐야 어차피 컨테이너에 접근할때 락을 걸어야하므로 여기서 만든다.

		// 들어온 시간, 분을 구해서 초로 만든다.	0900 이 들어오면 32400, 0 이겠지.
		int nHour = ((((_szTime[0] - 0x30) * 10) + ((_szTime[1] - 0x30))) * 3600);
		int nMinute = ((((_szTime[2] - 0x30) * 10) + ((_szTime[3] - 0x30))) * 60);
		int nSecond1 = (nHour + nMinute);	// 32400, 32460, 32520, 32580

		// 시간하고 분을 합한다. 
		ULONG nTime1 = (nHour * 100) + ((nSecond1 / 60) % 60);	// 900, 901, 902, 903
		// 생성된 해당 분의 1분봉이 있나?
		LPSTICK_DATA pStickData1 = umapStickMinute1[nTime1];
		if (nullptr == pStickData1)
		{	// 매 분마다 새로 생성하게 되니 이거슨 1분봉.
			pStickData1 = umapStickMinute1[nTime1] = new STICK_DATA(pSaveToFile->nPrice, nTime1, nDateToday);		// 시고저종을 모두 체결가로. ( 시가는 초기에 세팅한 후 변하지 않음 )
		}
		else
		{
			//if (_nClose > nHigh) { nHigh = _nClose; }	// 이전에 저장된 가격보다 높으면 갱신
			//if (_nClose < nLow) { nLow = _nClose; }	// 이전에 저장된 가격보다 낮으면 갱신
			//nClose = _nClose;							// 종가는 갱신 반복 ( 비교하면 어차피 분기하니 mov 만 함 )
			pStickData1->Update(pSaveToFile->nPrice);	// 이미 있다면 고저종 갱신
		}

		// 초를 각 3분단위로 잘라낸다.
		int nSecond3 = (nSecond1 - (nSecond1 % 180));	// 32400 - 32400 % 0
		// 분으로 변환하고 시간이랑 합친다 0900 -> 900, 0938 -> 930
		ULONG nTime3 = (nHour * 100) + ((nSecond3 / 60) % 60);
		// 생성된 3분봉이 있나?
		LPSTICK_DATA pStickData3 = umapStickMinute3[nTime3];
		if (nullptr == pStickData3)
		{	// 3분마다 새로 생성하게 되니 이거슨 3분봉.
			pStickData3 = umapStickMinute3[nTime3] = new STICK_DATA(pSaveToFile->nPrice, nTime3, nDateToday);		// 시고저종을 pSaveToFile->nPrice 로 세팅
			//DBGPRINT("[ENGINE] [%s] 3분봉 만든다: %d / %x / %x", _szCode, nTime3, pStickData3, umapStickMinute3[nTime3]);
		}
		else { pStickData3->Update(pSaveToFile->nPrice); }	// 이미 있다면 고저종 갱신

		// 초를 각 15분단위로 잘라낸다.
		int nSecond15 = (nSecond1 - (nSecond1 % 900));
		// 분으로 변환하고 시간이랑 합친다 0900 -> 900, 0938 -> 930
		ULONG nTime15 = (nHour * 100) + ((nSecond15 / 60) % 60);

		LPSTICK_DATA pStickData15 = umapStickMinute15[nTime15];
		if (nullptr == pStickData15)
		{	// 15분마다 새로 생성하게 되니 이거슨 15분봉.
			pStickData15 = umapStickMinute15[nTime15] = new STICK_DATA(pSaveToFile->nPrice, nTime15, nDateToday);	// 시고저종을 pSaveToFile->nPrice 로 세팅
			//DBGPRINT("[ENGINE] [%s] 15분봉 만든다: %d / %x / %x", _szCode, nTime15, pStickData15, umapStickMinute15[nTime15]);
		}
		else { pStickData15->Update(pSaveToFile->nPrice); }	// 이미 있다면 고저종 갱신

		//pStickData1->nAccrueVolumeTotal
		// 부호를 무시하고 거래량을 일단 뽑음. (dk::atoi_s)
		pSaveToFile->nTransVolume = dk::atoi_s(_szVolume);
		pStickData1->nAccrueVolumeTotal += pSaveToFile->nTransVolume;		// 1분봉 1분동안 거래량 누적.
		pStickData3->nAccrueVolumeTotal += pSaveToFile->nTransVolume;		// 3분봉 3분동안 거래량 누적.
		pStickData15->nAccrueVolumeTotal += pSaveToFile->nTransVolume;		// 15분봉 15분동안 거래량 누적.
		// 해당 종목의 거래량 매수 매도로 나누기
		{
			ULONGLONG ullTodayClose = stickToday.nClose;
			if ('+' == _szVolume[0])																// 해당 종목의 체결이 매수인가?
			{
				// 1분봉 매수거래량 누적
				pStickData1->누적매수체결량 += pSaveToFile->nTransVolume;
				// 매수거래대금 = 거래가격 X 매수거래량 ( 1분봉 )
				pStickData1->nAccruePaymentBuy += (ullTodayClose * pSaveToFile->nTransVolume);
				// 매수평균가격 = 누적매수거래대금 / 누적매수거래량 ( 1분봉 )
				pStickData1->nAveragePaymentBuy = (ULONG)(pStickData1->nAccruePaymentBuy / pStickData1->누적매수체결량);
				// 3분봉 매수거래량 누적
				pStickData3->누적매수체결량 += pSaveToFile->nTransVolume;
				pStickData3->nAccruePaymentBuy += (ullTodayClose * pSaveToFile->nTransVolume);									// 거래가격 X 매수거래량 = 매수거래대금 누적. ( 3분봉 )
				pStickData3->nAveragePaymentBuy = (ULONG)(pStickData3->nAccruePaymentBuy / pStickData3->누적매수체결량);		// 누적매수거래대금 / 누적매수거래량 = 매수평균가격 ( 3분봉 )
				// 15분봉 매수거래량 누적
				pStickData15->누적매수체결량 += pSaveToFile->nTransVolume;
				pStickData15->nAccruePaymentBuy += (ullTodayClose * pSaveToFile->nTransVolume);									// 거래가격 X 매수거래량 = 매수거래대금 누적. ( 15분봉 )
				pStickData15->nAveragePaymentBuy = (ULONG)(pStickData15->nAccruePaymentBuy / pStickData15->누적매수체결량);	// 누적매수거래대금 / 누적매수거래량 = 매수평균가격 ( 15분봉 )
				// 종목별 당일 매수거래량 누적
				stickToday.누적매수체결량 += pSaveToFile->nTransVolume;														// 매수 거래량 누적
				stickToday.nAccruePaymentBuy += (ullTodayClose * pSaveToFile->nTransVolume);									// 매수대금누적 = (매수대금누적 + (매수가격 x 거래량))
				stickToday.nAveragePaymentBuy = (ULONG)(stickToday.nAccruePaymentBuy / stickToday.누적매수체결량);			// 매수 평균 가격 = 누적매수대금 / 누적매수거래량
				// 파일로 저장할 구조체에 매수 표시
				pSaveToFile->nTransType = 1;
				/*
				DBGPRINT("[ENGINE] (시:분:초:밀리초 = %09d) [%s:매수] 거래가: %lld, 거래량: %d / 매수평균가: %d / 매수거래대금: %lld"
					, ::ntohl(pSaveToFile->nTime)
					, pSaveToFile->szCode
					, ullTodayClose
					, pSaveToFile->nTransVolume
					, nAveragePaymentBuy
					, nAccruePaymentBuy
				);
				*/
				누적매수거래횟수++;
			}
			else if ('-' == _szVolume[0])														// 해당 종목의 체결이 매도인가?
			{
				// 1분봉 매도거래량 누적
				pStickData1->누적매도체결량 += pSaveToFile->nTransVolume;
				pStickData1->nAccruePaymentSell += (ullTodayClose * pSaveToFile->nTransVolume);										// 거래가격 X 매도거래량 = 매도거래대금 누적. ( 1분봉 )
				pStickData1->nAveragePaymentSell = (ULONG)(pStickData1->nAccruePaymentSell / pStickData1->누적매도체결량);		// 누적매도거래대금 / 누적매도거래량 = 매수평균가격 ( 1분봉 )
				// 3분봉 매도거래량 누적
				pStickData3->누적매도체결량 += pSaveToFile->nTransVolume;
				pStickData3->nAccruePaymentSell += (ullTodayClose * pSaveToFile->nTransVolume);										// 거래가격 X 매도거래량 = 매도거래대금 누적. ( 3분봉 )
				pStickData3->nAveragePaymentSell = (ULONG)(pStickData3->nAccruePaymentSell / pStickData3->누적매도체결량);		// 누적매도거래대금 / 누적매도거래량 = 매수평균가격 ( 3분봉 )
				// 15분봉 매도거래량 누적
				pStickData15->누적매도체결량 += pSaveToFile->nTransVolume;
				pStickData15->nAccruePaymentSell += (ullTodayClose * pSaveToFile->nTransVolume);									// 거래가격 X 매도거래량 = 매도거래대금 누적. ( 15분봉 )
				pStickData15->nAveragePaymentSell = (ULONG)(pStickData15->nAccruePaymentSell / pStickData15->누적매도체결량);	// 누적매도거래대금 / 누적매도거래량 = 매수평균가격 ( 15분봉 )
				// 종목별 당일 매도거래량 누적
				stickToday.누적매도체결량 += pSaveToFile->nTransVolume;																		// 매도 거래량 누적
				stickToday.nAccruePaymentSell += (ullTodayClose * pSaveToFile->nTransVolume);													// 누적매도대금 = (누적매도대금 + (매수가격 x 거래량))
				stickToday.nAveragePaymentSell = (ULONG)(stickToday.nAccruePaymentSell / stickToday.누적매도체결량);												// 매도 평균 가격 = 누적매도대금 / 누적매도거래량
				// 파일로 저장할 구조체에 매도 표시
				pSaveToFile->nTransType = 2;
				/*
				DBGPRINT("[ENGINE] (시:분:초:밀리초 = %09d) [%s:매도] 거래가: %lld, 거래량: %d / 매도평균가: %d / 매도거래대금: %lld"
					, ::ntohl(pSaveToFile->nTime)
					, pSaveToFile->szCode
					, ullTodayClose
					, pSaveToFile->nTransVolume
					, nAveragePaymentSell
					, nAccruePaymentSell
				);
				*/
			}
			else
			{
				// 단일가매매인거다.
				pSaveToFile->nTransType = 3;
				//DBGPRINT("[ENGINE] 단일가매매 종목: %s / 시간: %s", _szCode, _szTime);
				// 단일가 매매라면 종가의 부호로 매수 매도를 판단한다.

			}
		}
		// 해당 종목의 누적 거래량 갱신
		ULONG nAccrueVolume = ::atoi(_szAccrueVolume);												// 체결 데이터로 받아온 누적 거래량
		if (0 != stickToday.nAccrueVolumeTotal)																// 해당 종목에 이전 거래가 있었는가?
		{
			// (이전틱누적 + 현재틱거래량)과 현재틱누적이 다르면
			if ((stickToday.nAccrueVolumeTotal + pSaveToFile->nTransVolume) != nAccrueVolume)
			{
				// 누락 발생 로그를 남겨야한다.
				if (0 < 누적매수거래횟수)	// 단일가 매매 종목이 아닐 경우
				{
					DBGPRINT("[ENGINE] [체결누락] 종목: %s / 시간: %s / 이전틱누적: %d + 현재틱거래량: %d = 현재틱누적: %d / 이전틱+현재거래량: %d / 차이: %d"
						, _szCode
						, _szTime
						, stickToday.nAccrueVolumeTotal
						, pSaveToFile->nTransVolume
						, nAccrueVolume
						, (stickToday.nAccrueVolumeTotal + pSaveToFile->nTransVolume)
						, (nAccrueVolume - (stickToday.nAccrueVolumeTotal + pSaveToFile->nTransVolume))	// 차이 = 현재틱누적 - (이전틱누적 + 현재틱거래량)
					);
				}
				else
				{
					DBGPRINT("[ENGINE] [체결누락/단일가] 종목: %s / 시간: %s / 이전틱누적: %d + 현재틱거래량: %d = 현재틱누적: %d / 이전틱+현재거래량: %d / 차이: %d"
						, _szCode
						, _szTime
						, stickToday.nAccrueVolumeTotal
						, pSaveToFile->nTransVolume
						, nAccrueVolume
						, (stickToday.nAccrueVolumeTotal + pSaveToFile->nTransVolume)
						, (nAccrueVolume - (stickToday.nAccrueVolumeTotal + pSaveToFile->nTransVolume))	// 차이 = 현재틱누적 - (이전틱누적 + 현재틱거래량)
					);
				}
			}
		}
		stickToday.nAccrueVolumeTotal = nAccrueVolume;													// 누적 거래량 갱신
		if (_szStrength)
		{
			체결강도 = (float)atof(_szStrength);										// 체결강도 갱신
		}
		// 틱데이터 저장하라고 쏘고
		//pFileManager->PushData(pSaveToFile);


		// 지표 등 연산할게 있으면 여기에서 하든, 다른 작업큐에 넘기든가 하면 된다.


		//DBGPRINT("콜백간다");
		// 화면에 출력하라고 쏘고
		// 관심종목인지 여기서 판단해서 보낸다.
	}
	catch (...)
	{
		DBGPRINT("[ENGINE] 뻑남: C_OBJECT_MONSTER::Update()");
	}
}
