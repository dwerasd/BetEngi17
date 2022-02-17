#pragma once


#include <set>

#include <DarkCore/DSingleton.h>

#include <Defines/DataDef.h>

#include "CCompanyData.h"



// 종목은 어차피 한번 등록하고 계속 쓰니까 stl을 쓴다.
typedef std::unordered_map<std::_tstring, C_COMPANY_DATA*> UMAP_STOCK_COMPANYS;		// 종목당 정보 (당일의 일봉 포함)
typedef std::unordered_map<long, std::_tstring> UMAP_CONDITIONS;					// 키움 조건검색 키와 이름
typedef std::unordered_map<std::_tstring, std::_tstring> UMAP_TWO_STRING;			// 종목코드와 종목명
// 종목당 틱데이터 링크를 담기 위한 맵
typedef std::unordered_map<std::_tstring, std::vector<LPTICK_DATA>> UMAP_TICK_DATAS;

class C_DATA_MANAGER
	: public dk::C_SINGLETON<C_DATA_MANAGER>
{
private:
	///////////////////////////////////////////////////////////////////////////////
	// 체결 데이터를 모두 저장한다. 연산을 줄이고 가지고 있다가 저장하기 좋고,
	// 하루치를 재생하면서 시뮬레이션 하기에 매우 용이하다
	///////////////////////////////////////////////////////////////////////////////
	// 모든 틱 저장 용도.
	LPBYTE pTickBuffer;			// 할당받은 버퍼의 위치
	LPBYTE pTickBufferPtr;		// 체결틱을 저장할 위치
	ULONG nAccrueTickCount;		// 들어온 체결틱 개수
	// 종목별 저장 용도.
	LPBYTE pStocksTickBuffer;
	LPBYTE pStocksTickBufferPtr;
	ULONG nAccrueStocksTickCount;
	///////////////////////////////////////////////////////////////////////////////
	// 모든 호가 잔량 저장 용도
	LPBYTE pRemainVolumeBuffer;
	LPBYTE pRemainVolumeufferPtr;
	ULONG nAccrueRemainVolumeCount;
	// 종목별 호가 잔량 저장 용도
	LPBYTE pStocksRemainVolumeBuffer;
	LPBYTE pStocksRemainVolumeBufferPtr;
	ULONG nAccrueStocksRemainVolumeCount;
	///////////////////////////////////////////////////////////////////////////////

public:
	C_DATA_MANAGER();
	~C_DATA_MANAGER();

	_TCHAR tszDateToday[1 << 5];	// "20201215"
	_TCHAR tszCurrentPath[_MAX_PATH];

	unsigned long nDateToday;

	// 종목코드들
	std::vector<std::_tstring> vStockCodes;
	UMAP_TWO_STRING umapStockNames;

	// key: 종목이름 value: (포인터)종목정보구조체
	UMAP_STOCK_COMPANYS umapStockCompanys;

	UMAP_TICK_DATAS umapTickDatas;

	std::set<std::_tstring> setAttentionCodes;		// 관심종목 코드

	// 모든 체결 데이터를 순서대로 담는다.	[0] 에는 가장 첫 거래가 담긴다.
	std::vector<LPTRANSFER_DATA> vTransferDatas;

	long Init();
	void Destroy();

	void MakeCompany(LPTSTR _szCode, LPTSTR _szName, LPTSTR _szState, LPTSTR _szControlKind);
	void PushTickDataOfFile(LPRECIVE_REALTIME_DATA_CONVERTED _pTickData);

	void PushRemainVolumeData(LPTSTR _szCode, LPTSTR _szTime, LPTSTR _szRemainVolumeBuy, LPTSTR _szRemainVolumeSell);

	/*
	// 검색식들을 받아오면 검색식 리스트에 등록하는 함수
	void SetConditions(_TCHAR* _pConditionList);
	// 검색식 실시간 등록 요청 결과를 받아오면 리스트에 등록하는 함수
	void SetCondition(long _nConditionIndex, _TCHAR* _pStockList);
	*/
};