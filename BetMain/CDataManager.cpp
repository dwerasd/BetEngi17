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
	::time_t tToday = ::time(0);		// ���� ��¥�� ����

	::tm t;
	::localtime_s(&t, &tToday);
	::_tcsftime(tszDateToday, _countof(tszDateToday), __TEXT("%Y%m%d"), &t);		// "20201215", ���� ��¥ �����
	nDateToday = ::_ttoi(tszDateToday);									// 20201215 �� �״�� ����.

	DBGPRINT(__TEXT("[BetMain] C_DATA_MANAGER(): %d"), nDateToday);

	::ZeroMemory(tszCurrentPath, _countof(tszCurrentPath));
	dk::GetCurrentDirectory(tszCurrentPath);

	///////////////////////////////////////////////////////////////////////////////
	// ƽ������ ��Ƶ� ����
	///////////////////////////////////////////////////////////////////////////////
	pTickBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(TICK_DATA) * 40000000));	// 4õ�����̸� 960�ް�, ������ ��ϸ� �ҰŴϱ�
	if (pTickBuffer)
	{
		DBGPRINT("[BetMain] ticks::HeapSize1 = %d", DHEAP_SIZE(pTickBuffer));
		pTickBufferPtr = pTickBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] ticks �� �Ҵ� ���� 1, ���α׷��� �����ؾ���.");
	}
	///////////////////////////////////////////////////////////////////////////////
	// ƽ�����͸� ������ �� ��������� �ݺ��� ����� ����
	///////////////////////////////////////////////////////////////////////////////
	pStocksTickBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(TICK_DATA) * 10000000));	// Ŭ����� ���ϰ� ������ �ᵵ �ɵ�
	if (pStocksTickBuffer)
	{
		DBGPRINT("[BetMain] stocks::HeapSize2 = %d", DHEAP_SIZE(pStocksTickBuffer));
		pStocksTickBufferPtr = pStocksTickBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] stocks �� �Ҵ� ���� 2, ���α׷��� �����ؾ���.");
	}
	///////////////////////////////////////////////////////////////////////////////
	// ȣ�� �ܷ��� ��Ƶ� ����
	///////////////////////////////////////////////////////////////////////////////
	pRemainVolumeBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(REMAIN_VOLUME_DATA) * 40000000));	// 4õ�����̸� 960�ް�, ������ ��ϸ� �ҰŴϱ�
	if (pRemainVolumeBuffer)
	{
		DBGPRINT("[BetMain] ticks::HeapSize3 = %d", DHEAP_SIZE(pRemainVolumeBuffer));
		pRemainVolumeufferPtr = pRemainVolumeBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] ticks �� �Ҵ� ���� 3, ���α׷��� �����ؾ���.");
	}
	///////////////////////////////////////////////////////////////////////////////
	// ȣ�� �ܷ��� ������ �� ��������� �ݺ��� ����� ����
	///////////////////////////////////////////////////////////////////////////////
	pStocksRemainVolumeBuffer = (LPBYTE)DHEAP_ALLOC((sizeof(REMAIN_VOLUME_DATA) * 10000000));	// Ŭ����� ���ϰ� ������ �ᵵ �ɵ�
	if (pStocksRemainVolumeBuffer)
	{
		DBGPRINT("[BetMain] stocks::HeapSize4 = %d", DHEAP_SIZE(pStocksRemainVolumeBuffer));
		pStocksRemainVolumeBufferPtr = pStocksRemainVolumeBuffer;
	}
	else
	{
		DBGPRINT("[BetMain] stocks �� �Ҵ� ���� 4, ���α׷��� �����ؾ���.");
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
		// �̸� �����ص� ȸ�� �����͸� ���ͼ� ����� �� �ְ� �Ѵ�.
		C_COMPANY_DATA* pCompanyData = umapStockCompanys[_pTickData->szStockCode];
		if (nullptr == pCompanyData)
		{	// ����� ������ ���� ����� ������ Ȥ�� �𸣴ϱ� �ۼ��صд�.
			DBGPRINT(__TEXT("����: %s"), _pTickData->szStockCode);
			pCompanyData = umapStockCompanys[_pTickData->szStockCode] = new C_COMPANY_DATA(nDateToday);
			pCompanyData->Init(_pTickData->szStockCode);
		}
		///////////////////////////////////////////////////////////////////////////////
		// �ϴ� ƽ �����͸� ���ӵ� �޸𸮿� ��ȯ�ؼ� �״´�.
		///////////////////////////////////////////////////////////////////////////////

		// ���� �����Ϳ��� ����Ҳ� (�ڵ�, �ð�, ����, ü�ᷮ, �ż��ŵ�����);
		LPTICK_DATA pTickData = (LPTICK_DATA)pTickBufferPtr;

		pTickData->nSequence = nAccrueTickCount++;	// ���ϰ� ����, ������ 0���� ����Ѵ�.
		// �ڵ�� ����, 7����Ʈ ���ڹ迭�̶� ���� �޸𸮴� ���� �Ⱦ���.
		_tcsncpy_s(pTickData->szCode, _countof(pTickData->szCode) * sizeof(_TCHAR), _pTickData->szStockCode, 6);
		pTickData->szCode[6] = 0;

		// �� �Լ��� ȣ��� �ý��� �ð��� ���´�.
		SYSTEMTIME cur_time;
		::GetLocalTime(&cur_time);

		// ���� �����Ұ� �ƴϴϱ� endian ��ȯ�� ���� �ʰ�, ���� ������ �и��ʸ� ���δ�. ( �ð� ��ȯ�� ���⼭ ��������� �ѹ��� �Ѵ�.
		_TCHAR szTime[(1 << 4)] = { 0 };
		_stprintf_s(szTime, __TEXT("%d"), (_pTickData->nTime * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pTickData->nTime = ::_ttoi(szTime);

		// �Ǵ��ؼ� ��ȯ �Ŀ� �Ѱܹ޵��� ����.
		pTickData->nTransType = _pTickData->bytTransType;

		// ��ȣ�� �پ� ������ �����ϰ� ���ڸ� ��ȯ
		pTickData->nPrice = _pTickData->nClose;
		pTickData->nTransVolume = _pTickData->nVolume;

		pTickBufferPtr += sizeof(TICK_DATA);	// ä�ŭ ������ �̵�
		///////////////////////////////////////////////////////////////////////////////
		// ������� ���ӵ� �޸𸮿� ���� �Ϸ�, �к� ���� ������ǥ�� ���� ������Ʈ
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
		// ���������̸� ������ ä�� ����ü �����͸� �� ���Ϲ޴´�
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
	{	// ����� ������ ���� ����� ������ Ȥ�� �𸣴ϱ� �ۼ��صд�.
		pCompanyData = umapStockCompanys[_szCode] = new C_COMPANY_DATA(nDateToday);
		pCompanyData->Init(_szCode);
	}
	// ���� �����Ϳ��� ����Ҳ� (�ڵ�, �ð�, ����, ü�ᷮ, �ż��ŵ�����);
	LPREMAIN_VOLUME_DATA pRemainVolumeData = (LPREMAIN_VOLUME_DATA)pRemainVolumeufferPtr;

	pRemainVolumeData->nSequence = nAccrueRemainVolumeCount++;	// ���ϰ� ����, ������ 0���� ����Ѵ�.
	// �ڵ�� ����, 7����Ʈ ���ڹ迭�̶� ���� �޸𸮴� ���� �Ⱦ���.
	memcpy_s(pRemainVolumeData->szCode, 6, _szCode, 6);
	pRemainVolumeData->szCode[6] = pRemainVolumeData->szCode[7] = 0;

	// �� �Լ��� ȣ��� �ý��� �ð��� ���´�.
	SYSTEMTIME cur_time;
	::GetLocalTime(&cur_time);
	// �и��ʸ� ���Ѵ�. ( �ð� ��ȯ�� ���⼭ ��������� �ѹ��� �Ѵ�.
	_TCHAR szTime[(1 << 4)] = { 0 };
	// ���� �����Ұ� �ƴϴϱ� endian ��ȯ�� ���� �ʴ´�.
	_stprintf_s(szTime, __TEXT("%d"), (::_ttoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
	pRemainVolumeData->nTime = ::_ttoi(szTime);

	pRemainVolumeData->nRemainVolumeBuy = dk::_ttoi_s(_szRemainVolumeBuy);
	pRemainVolumeData->nRemainVolumeSell = dk::_ttoi_s(_szRemainVolumeSell);

	pRemainVolumeufferPtr += sizeof(REMAIN_VOLUME_DATA);	// ä�ŭ ������ �̵�
///////////////////////////////////////////////////////////////////////////////
// ������� ���ӵ� �޸𸮿� ���� �Ϸ�, ���⿡�� ƽ ó�� ����.
///////////////////////////////////////////////////////////////////////////////
	// ���� �����Ϳ� ��� �ֽŰɷ� �����ϴ°Ŵ�.
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
			// "014^�������ǿ�ȭ�µι��ɴ�;004^���Ͽ����Ǵ²�;002^���Ͽ����Ǵ¿�ȭ;005^�ǽ����帧;016^���Ե��������Ը�;003^���Ե��������Ը�(-���-����);"
			_TCHAR* pResult = _tcsrchr(_pConditionList, __TEXT(';'));		// �������� ';'�� ã�´�.
			if (nullptr == pResult) { break; }					// ã�� ���ϸ� �ݺ����� ����
			pResult += 1;										// ';' �� ã������ �� ���� �ּҷ� +1
			if (0 < _tcslen(pResult))							// ���� ������ ������ ';'�� ã�Ҵٸ� + 1 ���Ŀ� ���̴�.
			{
				// ';' ������ ���ڿ� ������ �߶���ϴ� ���� �ڸ���
				int nConditionIndex = ::_ttoi(pResult);			// �˻��� �ε����� �����ϰ�, '^' �������� ���ڷ� ��ȯ�ȴ�.
				_TCHAR* pCondition = _tcsrchr(pResult, __TEXT('^'));		// '^' �� ã�´�.
				if (0 < _tcslen(pCondition))
				{
					pCondition += 1;
					// �˻��� �ε����� Ű���ؼ� �˻��� �̸��� �����Ѵ�. GUI���� �ε����� ��û�ϰ� �˻��� �̸��� �����ؼ� ��û�Ѵ�.
					pEngine->umapConditions[nConditionIndex] = pCondition;
					ZeroMemory(&info, sizeof(_CALLBACK_SET_CONDITION));
					//_CALLBACK_SET_CONDITION* p = (_CALLBACK_SET_CONDITION*)szBuffer;
					info.nIndex = nConditionIndex;
					//*(PLONG)szBuffer = nConditionIndex;
#if defined(_USE_GUI_QT5_)
					// utf8 �� ��ȯ�ؼ� �����ش�.
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
			*(_TCHAR*)(pResult - 1) = 0;							// ���ڿ� �տ� ';' �̾��� ���̳ʸ��� 0 ���� �ϸ� �� ���� ���ڿ��� ���� �ȴ�.
		} while (true);
		// ������ �� ���� ���ڿ��� �и��ؼ� map�� �ִ´�.
		_TCHAR* pLastCondition = _tcsrchr(_pConditionList, '^');			// '^' �� ã�´�.
		if (0 < _tcslen(pLastCondition))
		{
			pLastCondition += 1;										// '^' ������ ���ǽĸ� �����͸� �����ϰ�
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
			_TCHAR* pResult = _tcsrchr(_pStockList, ';');	// �������� ';'�� ã�´�.
			if (nullptr == pResult) { break; }			// ã�� ���ϸ� �ݺ����� ����
			pResult += 1;								// ';' �� ã������ �� ���� �ּҷ� +1
			if (0 < _tcslen(pResult))					// ���� ������ ������ ';'�� ã�Ҵٸ� + 1 ���Ŀ� ���̴�.
			{
				setAttentionCodes.insert(pResult);

				ZeroMemory(&info, sizeof(_CALLBACK_ADD_CONDITION));
				info.nIndex = _nConditionIndex;
				// ���� "005930" ����
				//ZeroMemory(szBuffer, _countof(szBuffer));
#if defined(_USE_GUI_QT5_)
				_tcscpy_s(info.szTime, _countof(info.szTime), dk::AnsiToUtf8_s(szBuffer, _countof(szBuffer), szTimeStamp));
#else
				_tcscpy_s(info.szTime, _countof(info.szTime) * sizeof(_TCHAR), szTimeStamp);
#endif
				_tcscpy_s(info.szCode, _countof(info.szCode) * sizeof(_TCHAR), pResult);		// �ð��� �����ǿ� �Ѹ��� �ƴϱ� ������
				//DBGPRINT("[BetMain] Add Attention Code: %s", pResult);
				C_COMPANY_DATA* pCompany = umapStockCompanys[info.szCode];
				if (pCompany)
				{
					pCompany->bAttention = true;
					DBGPRINT(__TEXT("[BetMain] �������� �����: [%s] %s"), info.szCode, umapStockNames[info.szCode].c_str());
				}
				pEngine->pCallback(_ADD_TABLE_ROW_STOCK_INFO_, &info);
			}
			*(_TCHAR*)(pResult - 1) = 0;							// ���ڿ� �տ� ';' �̾��� ���̳ʸ��� 0 ���� �ϸ� �� ���� ���ڿ��� ���� �ȴ�.
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
			DBGPRINT(__TEXT("[BetMain] �������� �����: [%s] %s"), info.szCode, umapStockNames[info.szCode].c_str());
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
