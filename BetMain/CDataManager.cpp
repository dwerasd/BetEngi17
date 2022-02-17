#include "framework.h"
#include "CDataManager.h"



C_DATA_MANAGER::C_DATA_MANAGER()
	: pTickBuffer(nullptr)
	, pTickBufferPtr(nullptr)
	, nAccrueTickCount(0)

	, pStocksTickBuffer(nullptr)
	, pStocksTickBufferPtr(nullptr)
	, nAccrueStocksTickCount(0)

	, pRemainVolumeBuffer(nullptr)
	, pRemainVolumeufferPtr(nullptr)
	, nAccrueRemainVolumeCount(0)

	, pStocksRemainVolumeBuffer(nullptr)
	, pStocksRemainVolumeBufferPtr(nullptr)
	, nAccrueStocksRemainVolumeCount(0)
{
	::time_t tToday = ::time(0);		// 오늘 날짜를 구함

	::tm t;
	::localtime_s(&t, &tToday);
	::_tcsftime(tszDateToday, _countof(tszDateToday), __TEXT("%Y%m%d"), &t);		// "20201215", 오늘 날짜 저장용
	nDateToday = ::_ttoi(tszDateToday);									// 20201215 가 그대로 들어간다.

	DBGPRINT(__TEXT("[BetMain] C_DATA_MANAGER(): %d"), nDateToday);

	::ZeroMemory(tszCurrentPath, _countof(tszCurrentPath));
	dk::GetCurrentDirectory(tszCurrentPath);

	///////////////////////////////////////////////////////////////////////////////
	// 틱데이터 모아둘 버퍼
	///////////////////////////////////////////////////////////////////////////////
	pTickBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(TICK_DATA) * 40000000));	// 4천만건이면 960메가, 어차피 기록만 할거니까
	if (pTickBuffer)
	{
		DBGPRINT("[BetMain] ticks::HeapSize1 = %d", DHEAP_SIZE(pTickBuffer));
		pTickBufferPtr = pTickBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] ticks 힙 할당 실패 1, 프로그램을 종료해야함.");
	}
	///////////////////////////////////////////////////////////////////////////////
	// 틱데이터를 저장할 떄 복사용으로 반복해 사용할 버퍼
	///////////////////////////////////////////////////////////////////////////////
	pStocksTickBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(TICK_DATA) * 10000000));	// 클리어는 안하고 덮어쓰기로 써도 될듯
	if (pStocksTickBuffer)
	{
		DBGPRINT("[BetMain] stocks::HeapSize2 = %d", DHEAP_SIZE(pStocksTickBuffer));
		pStocksTickBufferPtr = pStocksTickBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] stocks 힙 할당 실패 2, 프로그램을 종료해야함.");
	}
	///////////////////////////////////////////////////////////////////////////////
	// 호가 잔량을 모아둘 버퍼
	///////////////////////////////////////////////////////////////////////////////
	pRemainVolumeBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(REMAIN_VOLUME_DATA) * 40000000));	// 4천만건이면 960메가, 어차피 기록만 할거니까
	if (pRemainVolumeBuffer)
	{
		DBGPRINT("[BetMain] ticks::HeapSize3 = %d", DHEAP_SIZE(pRemainVolumeBuffer));
		pRemainVolumeufferPtr = pRemainVolumeBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] ticks 힙 할당 실패 3, 프로그램을 종료해야함.");
	}
	///////////////////////////////////////////////////////////////////////////////
	// 호가 잔량을 저장할 때 복사용으로 반복해 사용할 버퍼
	///////////////////////////////////////////////////////////////////////////////
	pStocksRemainVolumeBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(REMAIN_VOLUME_DATA) * 10000000));	// 클리어는 안하고 덮어쓰기로 써도 될듯
	if (pStocksRemainVolumeBuffer)
	{
		DBGPRINT("[BetMain] stocks::HeapSize4 = %d", DHEAP_SIZE(pStocksRemainVolumeBuffer));
		pStocksRemainVolumeBufferPtr = pStocksRemainVolumeBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] stocks 힙 할당 실패 4, 프로그램을 종료해야함.");
	}
}

C_DATA_MANAGER::~C_DATA_MANAGER()
{
	DHEAP_FREE(pTickBuffer);
	DHEAP_FREE(pStocksTickBuffer);
	DHEAP_FREE(pRemainVolumeBuffer);
	DHEAP_FREE(pStocksRemainVolumeBuffer);
}

void C_DATA_MANAGER::PushTickDataOfFile(LPRECIVE_REALTIME_DATA_CONVERTED _pTickData)
{
	try
	{
		// 미리 생성해둔 회사 포인터를 얻어와서 사용할 수 있게 한다.
		C_COMPANY_DATA* pCompanyData = umapStockCompanys[_pTickData->szStockCode];
		if (nullptr == pCompanyData)
		{	// 여기는 들어오는 일이 없어야 하지만 혹시 모르니까 작성해둔다.
			DBGPRINT(__TEXT("생성: %s"), _pTickData->szStockCode);
			pCompanyData = umapStockCompanys[_pTickData->szStockCode] = new C_COMPANY_DATA(nDateToday);
			pCompanyData->Init(_pTickData->szStockCode);
		}
		///////////////////////////////////////////////////////////////////////////////
		// 일단 틱 데이터를 연속된 메모리에 변환해서 쌓는다.
		///////////////////////////////////////////////////////////////////////////////

		// 들어온 데이터에서 기록할껀 (코드, 시간, 가격, 체결량, 매수매도여부);
		LPTICK_DATA pTickData = (LPTICK_DATA)pTickBufferPtr;

		pTickData->nSequence = nAccrueTickCount++;	// 더하고 증가, 순서는 0부터 기록한다.
		// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
		_tcsncpy_s(pTickData->szCode, _countof(pTickData->szCode) * sizeof(_TCHAR), _pTickData->szStockCode, 6);
		pTickData->szCode[6] = 0;

		// 이 함수가 호출된 시스템 시간을 얻어온다.
		SYSTEMTIME cur_time;
		::GetLocalTime(&cur_time);

		// 아직 저장할게 아니니까 endian 변환은 하지 않고, 대충 현재의 밀리초를 붙인다. ( 시간 변환은 여기서 저장용으로 한번만 한다.
		_TCHAR szTime[(1 << 4)] = { 0 };
		_stprintf_s(szTime, __TEXT("%d"), (_pTickData->nTime * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pTickData->nTime = ::_ttoi(szTime);

		// 판단해서 변환 후에 넘겨받도록 하자.
		pTickData->nTransType = _pTickData->bytTransType;

		// 부호가 붙어 있으면 무시하고 숫자만 변환
		pTickData->nPrice = _pTickData->nClose;
		pTickData->nTransVolume = _pTickData->nVolume;

		pTickBufferPtr += sizeof(TICK_DATA);	// 채운만큼 포인터 이동
		///////////////////////////////////////////////////////////////////////////////
		// 여기까지 연속된 메모리에 적재 완료, 분봉 등의 보조지표를 위한 업데이트
		///////////////////////////////////////////////////////////////////////////////
		pCompanyData->UpdateConverted(
			_pTickData->nTime
			, _pTickData->bytTransType
			, _pTickData->nClose
			, _pTickData->nVolume
			, _pTickData->nOpen
			, _pTickData->nHigh
			, _pTickData->nLow
			, _pTickData->nAccrueVolume
		);
		// 관심종목이면 정보를 채운 구조체 포인터를 를 리턴받는다
		/*
		if (!bDirectCallback)
		{
			if (pInfo) { CallbackQueue.enqueue(pInfo); }
		}
		*/
	}
	catch (...)
	{

	}
}

void C_DATA_MANAGER::PushRemainVolumeData(LPTSTR _szCode, LPTSTR _szTime, LPTSTR _szRemainVolumeBuy, LPTSTR _szRemainVolumeSell)
{
	C_COMPANY_DATA* pCompanyData = umapStockCompanys[_szCode];
	if (nullptr == pCompanyData)
	{	// 여기는 들어오는 일이 없어야 하지만 혹시 모르니까 작성해둔다.
		pCompanyData = umapStockCompanys[_szCode] = new C_COMPANY_DATA(nDateToday);
		pCompanyData->Init(_szCode);
	}
	// 들어온 데이터에서 기록할껀 (코드, 시간, 가격, 체결량, 매수매도여부);
	LPREMAIN_VOLUME_DATA pRemainVolumeData = (LPREMAIN_VOLUME_DATA)pRemainVolumeufferPtr;

	pRemainVolumeData->nSequence = nAccrueRemainVolumeCount++;	// 더하고 증가, 순서는 0부터 기록한다.
	// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
	memcpy_s(pRemainVolumeData->szCode, 6, _szCode, 6);
	pRemainVolumeData->szCode[6] = pRemainVolumeData->szCode[7] = 0;

	// 이 함수가 호출된 시스템 시간을 얻어온다.
	SYSTEMTIME cur_time;
	::GetLocalTime(&cur_time);
	// 밀리초를 더한다. ( 시간 변환은 여기서 저장용으로 한번만 한다.
	_TCHAR szTime[(1 << 4)] = { 0 };
	// 아직 저장할게 아니니까 endian 변환은 하지 않는다.
	_stprintf_s(szTime, __TEXT("%d"), (::_ttoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
	pRemainVolumeData->nTime = ::_ttoi(szTime);

	pRemainVolumeData->nRemainVolumeBuy = dk::_ttoi_s(_szRemainVolumeBuy);
	pRemainVolumeData->nRemainVolumeSell = dk::_ttoi_s(_szRemainVolumeSell);

	pRemainVolumeufferPtr += sizeof(REMAIN_VOLUME_DATA);	// 채운만큼 포인터 이동
///////////////////////////////////////////////////////////////////////////////
// 여기까지 연속된 메모리에 적재 완료, 여기에서 틱 처리 하자.
///////////////////////////////////////////////////////////////////////////////
	// 종목 데이터에 계속 최신걸로 갱신하는거다.
	pCompanyData->nRemainVolumeBuy = pRemainVolumeData->nRemainVolumeBuy;
	pCompanyData->nRemainVolumeSell = pRemainVolumeData->nRemainVolumeSell;

}
/*
void C_DATA_MANAGER::SetConditions(_TCHAR* _pConditionList)
{
	//DBGPRINT("[BetMain] SetConditions: %s", _pConditionList);
	try
	{
#if defined(_CHECK_PERFORMANCE_)
		timerPerformance[0].Start();
#endif
		//_TCHAR szBuffer[(1 << 8)] = { 0 };
		_CALLBACK_SET_CONDITION info;
		do
		{
			// "014^나뭇잎의연화는두번핀다;004^지하에서피는꽃;002^지하에서피는연화;005^의식의흐름;016^형님들저도한입만;003^형님들저도한입만(-담불-증백);"
			_TCHAR* pResult = _tcsrchr(_pConditionList, __TEXT(';'));		// 우측부터 ';'를 찾는다.
			if (nullptr == pResult) { break; }					// 찾지 못하면 반복문을 중지
			pResult += 1;										// ';' 를 찾았으면 그 다음 주소로 +1
			if (0 < _tcslen(pResult))							// 만약 파일의 끝에서 ';'를 찾았다면 + 1 이후에 널이다.
			{
				// ';' 다음의 문자열 어차피 잘라야하니 지금 자르자
				int nConditionIndex = ::_ttoi(pResult);			// 검색식 인덱스를 보관하고, '^' 전까지만 숫자로 변환된다.
				_TCHAR* pCondition = _tcsrchr(pResult, __TEXT('^'));		// '^' 를 찾는다.
				if (0 < _tcslen(pCondition))
				{
					pCondition += 1;
					// 검색식 인덱스를 키로해서 검색식 이름만 보관한다. GUI에서 인덱스로 요청하고 검색식 이름을 포함해서 요청한다.
					pEngine->umapConditions[nConditionIndex] = pCondition;
					ZeroMemory(&info, sizeof(_CALLBACK_SET_CONDITION));
					//_CALLBACK_SET_CONDITION* p = (_CALLBACK_SET_CONDITION*)szBuffer;
					info.nIndex = nConditionIndex;
					//*(PLONG)szBuffer = nConditionIndex;
#if defined(_USE_GUI_QT5_)
					// utf8 로 변환해서 보내준다.
					dk::AnsiToUtf8_s(szBuffer + sizeof(long), _countof(szBuffer), pCondition);
#else
					//_tcscpy_s((szBuffer + sizeof(long)), _countof(szBuffer) - sizeof(long), pCondition);
					_tcscpy_s(info.szName, sizeof(info.szName), pCondition);
					//DBGPRINT(__TEXT("%s"), (szBuffer + sizeof(long)));
#endif
					DBGPRINT(__TEXT("[BetMain] _ADD_ROW_CONDITION_: %s"), pCondition);
					pEngine->pCallback(_ADD_TABLE_ROW_CONDITION_, &info);
				}
			}
			*(_TCHAR*)(pResult - 1) = 0;							// 문자열 앞에 ';' 이었던 바이너리를 0 으로 하면 그 곳이 문자열의 끝이 된다.
		} while (true);
		// 마지막 맨 앞의 문자열을 분리해서 map에 넣는다.
		_TCHAR* pLastCondition = _tcsrchr(_pConditionList, '^');			// '^' 를 찾는다.
		if (0 < _tcslen(pLastCondition))
		{
			pLastCondition += 1;										// '^' 다음의 조건식명 포인터를 보관하고
			long nConditionIndex = ::_ttoi(_pConditionList);
			pEngine->umapConditions[nConditionIndex] = pLastCondition;
			//ZeroMemory(szBuffer, sizeof(szBuffer));
			ZeroMemory(&info, sizeof(_CALLBACK_SET_CONDITION));
			//_CALLBACK_SET_CONDITION* p = (_CALLBACK_SET_CONDITION*)szBuffer;
			info.nIndex = nConditionIndex;
			//*(PLONG)szBuffer = ::_ttoi(_pConditionList);
#if defined(_USE_GUI_QT5_)
			dk::AnsiToUtf8_s(szBuffer + sizeof(long), _countof(szBuffer), pLastCondition);
#else
			//_tcscpy_s((szBuffer + sizeof(long)), _countof(szBuffer) - sizeof(long), pLastCondition);
			_tcscpy_s(info.szName, sizeof(info.szName), pLastCondition);
#endif
			DBGPRINT(__TEXT("[BetMain] _ADD_ROW_CONDITION_: %s"), pLastCondition);
			pEngine->pCallback(_ADD_TABLE_ROW_CONDITION_, &info);
		}
#if defined(_CHECK_PERFORMANCE_)
		DBGPRINT(__TEXT("[BetMain] SetConditions() = %0.06f"), timerPerformance[0].Check());
#endif
	}
	catch (...)
	{
		DBGPRINT("[BetMain] Unexpected exception / %s / %s line: %i", __FILE__, __func__, __LINE__);

	}
}

void C_DATA_MANAGER::SetCondition(long _nConditionIndex, _TCHAR* _pStockList)
{
	DBGPRINT("[BetMain] SetStockList: %i %s", _nConditionIndex, _pStockList);
	try
	{
#if defined(_CHECK_PERFORMANCE_)
		timerPerformance[0].Start();
#endif
		//_TCHAR szBuffer[(1 << 10)] = { 0 };
		_CALLBACK_ADD_CONDITION info;

		_TCHAR szTimeStamp[(1 << 5)] = { 0 };
		time_t tToday = ::time(0);

		tm t;
		::localtime_s(&t, &tToday);
		::_tcsftime(szTimeStamp, _countof(szTimeStamp), __TEXT("%H:%M:%S"), &t);
		do
		{
			// "013700;015750;036890;045340;050090;052420;083420;241840;263810;"
			_TCHAR* pResult = _tcsrchr(_pStockList, ';');	// 우측부터 ';'를 찾는다.
			if (nullptr == pResult) { break; }			// 찾지 못하면 반복문을 중지
			pResult += 1;								// ';' 를 찾았으면 그 다음 주소로 +1
			if (0 < _tcslen(pResult))					// 만약 파일의 끝에서 ';'를 찾았다면 + 1 이후에 널이다.
			{
				setAttentionCodes.insert(pResult);

				ZeroMemory(&info, sizeof(_CALLBACK_ADD_CONDITION));
				info.nIndex = _nConditionIndex;
				// 종목 "005930" 복사
				//ZeroMemory(szBuffer, _countof(szBuffer));
#if defined(_USE_GUI_QT5_)
				_tcscpy_s(info.szTime, _countof(info.szTime), dk::AnsiToUtf8_s(szBuffer, _countof(szBuffer), szTimeStamp));
#else
				_tcscpy_s(info.szTime, _countof(info.szTime) * sizeof(_TCHAR), szTimeStamp);
#endif
				_tcscpy_s(info.szCode, _countof(info.szCode) * sizeof(_TCHAR), pResult);		// 시간은 전광판에 뿌릴게 아니기 때문에
				//DBGPRINT("[BetMain] Add Attention Code: %s", pResult);
				C_COMPANY_DATA* pCompany = umapStockCompanys[info.szCode];
				if (pCompany)
				{
					pCompany->bAttention = true;
					DBGPRINT(__TEXT("[BetMain] 관심종목 등록함: [%s] %s"), info.szCode, umapStockNames[info.szCode].c_str());
				}
				pEngine->pCallback(_ADD_TABLE_ROW_STOCK_INFO_, &info);
			}
			*(_TCHAR*)(pResult - 1) = 0;							// 문자열 앞에 ';' 이었던 바이너리를 0 으로 하면 그 곳이 문자열의 끝이 된다.
		} while (true);
		ZeroMemory(&info, sizeof(_CALLBACK_ADD_CONDITION));
		info.nIndex = _nConditionIndex;
#if defined(_USE_GUI_QT5_)
		ZeroMemory(szBuffer, _countof(szBuffer));
		_tcscpy_s(info.szTime, _countof(info.szTime), dk::AnsiToUtf8_s(szBuffer, _countof(szBuffer), szTimeStamp));
#else
		_tcscpy_s(info.szTime, _countof(info.szTime) * sizeof(_TCHAR), szTimeStamp);

#endif
		_tcscpy_s(info.szCode, _countof(info.szCode) * sizeof(_TCHAR), _pStockList);
		//DBGPRINT("[BetMain] Add Attention Code: %s", _pStockList);

		C_COMPANY_DATA* pCompany = umapStockCompanys[info.szCode];
		if (pCompany)
		{
			pCompany->bAttention = true;
			DBGPRINT(__TEXT("[BetMain] 관심종목 등록함: [%s] %s"), info.szCode, umapStockNames[info.szCode].c_str());
		}

		pEngine->pCallback(_ADD_TABLE_ROW_STOCK_INFO_, &info);

#if defined(_CHECK_PERFORMANCE_)
		DBGPRINT(__TEXT("[BetMain] SetConditions() = %0.06f"), timerPerformance[0].Check());
#endif
	}
	catch (...)
	{
		DBGPRINT("[BetMain] Unexpected exception / %s / %s line: %i", __FILE__, __func__, __LINE__);
	}
}
*/
