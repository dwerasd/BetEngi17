#pragma once


#include <DarkCore/DTimer.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DMariaDB.h>

#include <Defines/NetworkDef.h>


class C_DATABASE
	: public dk::C_SINGLETON< C_DATABASE>
	, public dk::C_MARIADB
	, public dk::C_THREAD
{
private:
	DWORD ThreadFunc(LPVOID _pParam);

public:
	C_DATABASE();
	~C_DATABASE();

	void 기본세팅();

	bool 체크_데이터베이스(LPCSTR _pDBName);
	void 생성_데이터베이스(LPCSTR _pDBName);


	bool 체크_테이블(LPCSTR _pDBName, LPCSTR _pTable);
	void 생성_테이블_주식정보(LPCSTR _pDBName, LPCSTR _pTable);


	void CreateTableDay(LPCSTR _pDBName, LPCSTR _pTable);


	void 추가_주식종목(
		LPCSTR _pCode
		, LPCSTR _pFullCode
		, LPCSTR _pCodeCreon		// 크레온코드
		, LPCSTR _pName
		, int _nMarketType
		, int _nKseSetionType
		, int _pControlType
		, int _nSupervisionType
		, int _nStockStatus
		, long _nIsSpac
		, int _bExchange			// [0]: 크레온, [1]: 키움, [2]: 이베스트, [3:] 한국투자증권
	);
	void 추가_주식종목(
		LPCSTR _pCode
		, LPCSTR _pName
		, int _bExchange
	);
	void 업데이트_상태(
		LPCSTR _pFullCode
		, int _nStockStatus
	);
	bool 체크_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _기준필드, LPCSTR _기준값);
	void 추가_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _필드, LPCSTR _데이터);
	void 수정_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _수정할필드, LPCSTR _기준필드, LPCSTR _기준데이터, int _수정할데이터);
	void 수정_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _수정할필드, LPCSTR _기준필드, LPCSTR _기준데이터, LPCSTR _수정할데이터);
	void 가져오기_데이터(LPCSTR _데이터베이스, LPCSTR _테이블, LPCSTR _기준필드, LPCSTR _기준값, LPCSTR _가져올필드, LPSTR _반환 = nullptr, size_t _반환배열크기 = 0);

	void 가져오기_풀코드(LPCSTR _종목코드, LPSTR _반환, size_t _반환배열크기);
	void 가져오기_크레온코드(LPCSTR _풀코드, LPSTR _반환, size_t _반환배열크기);

	bool 확인_블랙리스트(LPCSTR _풀코드);
	void 추가_블랙리스트(LPCSTR _풀코드, LPCSTR _종목명);

	bool 체크_주식종목(LPSTOCK_INFO_CREON _정보포);

	void GetLastTime(LPCSTR _pFullCode, int* _pLastDate, int* _pLastTime, bool _bIsFull = true);
	void SetCodeMarketType(LPCSTR _pFullCode, int _nMarketType);

	bool AddMinute1Data(
		LPCSTR _pCode
		, LPCSTR _pFullCode
		, int _nDate
		, int _nTime
		, int _nOpen
		, int _nHigh
		, int _nLow
		, int _nClose
		, ULONG _nVolume
	);
};

#define 디비	C_DATABASE::GetInstance