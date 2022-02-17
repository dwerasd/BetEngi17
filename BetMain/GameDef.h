#pragma once


#include <DarkCore/DDef.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DMemory.h>

#include <Defines/DataDef.h>



typedef struct _캔들_기본
{	// 이유는 모르겠는데 이대로 유맵에 넣으면 첫번째 데이터에 접근이 안돼서 거래량을 위로 올린다.
	ULONG 거래량{ 0 };							// 거래량
	ULONG 날짜{ 0 };							// 날짜
	ULONG 시간{ 0 };							// 시간
	double 시가{ 0.0 };							// 시가
	double 고가{ 0.0 };							// 고가
	double 저가{ 0.0 };							// 저가
	double 종가{ 0.0 };							// 종가(현재가)
	
	virtual void 초기화()
	{
		날짜 = 시간 = 거래량 = 0;
		시가 = 고가 = 저가 = 종가  = 0.0;
	}
} 캔들_기본, * 캔들_기본포;

typedef struct _동적버퍼_더블
{
	double* 버퍼{ nullptr };
	size_t 버퍼크기{ 0 };

	void 해제()
	{
		힙해제(버퍼);
		버퍼크기 = 0;
	}
	void 할당(size_t _버퍼크기)
	{
		//DBGPRINT("_동적버퍼_더블::할당(%d)", _버퍼크기);
		버퍼 = (double*)힙할당(_버퍼크기);
		버퍼크기 = _버퍼크기;
	}
	void 초기화() { dk::초기화(버퍼, 버퍼크기); }
} 동적버퍼_더블, * 동적버퍼_더블포;

typedef struct _동적버퍼_캔들
{
	size_t 캔들배열수{ 0 }, 적재된캔들수{ 0 };
	캔들_기본포 캔들배열{ nullptr };
	void 해제()
	{
		힙해제(캔들배열);
		캔들배열수 = 적재된캔들수 = 0;
	}
	void 할당(size_t _캔들배열수)
	{
		캔들배열 = (캔들_기본포)힙할당(sizeof(캔들_기본) * _캔들배열수);
		캔들배열수 = _캔들배열수;
	}
} 동적버퍼_캔들, * 동적버퍼_캔들포;

typedef struct _캔들_시뮬레이션용
{
	ULONG 시간{ 0 };
	ULONG 초_시;					// 09 는 32400
	WORD 초_분;						// 30 은 1800
	BYTE 초;						// 27 은 27
	double 시가{ 0.0 };				// 시가
	double 고가{ 0.0 };				// 고가
	double 저가{ 0.0 };				// 저가
	double 종가{ 0.0 };				// 종가(현재가)

	virtual void 초기화()
	{
		초_시 = 초_분 = 초 = 0;
		시가 = 고가 = 저가 = 종가 = 0.0;
	}
} 캔들_시뮬레이션용, * 캔들_시뮬레이션용포;

typedef struct _캔들_보조
{
	double 고저종{ 0.0 };						// (고+저+종) / 3
	double 시고저종{ 0.0 };						// (시+고+저+종) / 4

	double SWMA{ 0.0 };
	double WMA2_시고저종{ 0.0 };						// wma(시고저종, 2)

	double 볼린저밴드[3] = { 0.0 };

	double 상대강도지수12{ 0.0 };
	double 다크밴드_ATR{ 0.0 };

	double 다크밴드_EMA[3] = { 0.0 };
	double 다크밴드_Highest[3] = { 0.0 };
	double 다크밴드_Lowest[3] = { 0.0 };

	double 다크밴드_상단배열[3] = { 0.0 };
	double 다크밴드_하단배열[3] = { 0.0 };

	double 다크밴드_상단{ 0.0 };
	double 다크밴드_하단{ 0.0 };
	double 다크밴드_중단{ 0.0 };

	double dqfTrailStopLoss{ 0.0 };			// 중앙선
	double dqfTrailLine{ 0.0 };

	double CAZ_FAST{ 0.0 };					// fast
	double CAZ_SLOW{ 0.0 };					// slow

	double 엔벨로프[3] = { 0.0 };
	virtual void 초기화()
	{
		고저종 = 시고저종 = SWMA = WMA2_시고저종 = 상대강도지수12 = 다크밴드_ATR = 다크밴드_상단 = 다크밴드_하단 = 다크밴드_중단 = dqfTrailStopLoss = dqfTrailLine = CAZ_FAST = CAZ_SLOW = 0.0;
		for (BYTE i = 0; i < 3; i++)
		{
			볼린저밴드[i] = 다크밴드_EMA[i] = 다크밴드_Highest[i] = 다크밴드_Lowest[i] = 다크밴드_상단배열[i] = 다크밴드_하단배열[i] = 엔벨로프[i] = 0.0;
		}
	}
} 캔들_보조, * 캔들_보조포;


typedef struct _종목정보_기본
{
	bool 상태{ false };
	bool 관리종목{ false };					// 0 이 아니면 거름
	bool 상태_크레온{ false };				// 위험 경고 등 0 이 아니면 거름
	bool 스팩주{ false };
	BYTE 섹션{ 0 };							// ETF, ELW 등의 직업
	BYTE 장구분{ 0 };						// [1]: 코스피, [2]: 코스닥
	ULONG 시가총액{ 0 };
	double 전일종가{ 0.0 };					// 전일종가

	char 종목코드[7] = { 0 };
	char 풀코드[13] = { 0 };
	char 크레온코드[8] = { 0 };
	char 종목명[(1 << 6)] = { 0 };
	virtual void Init()
	{
		상태 = false;
		장구분 = 0;
		전일종가 = 0.0;
		dk::초기화(종목코드, 배열크기(종목코드));
		dk::초기화(크레온코드, 배열크기(크레온코드));
		dk::초기화(풀코드, 배열크기(풀코드));
		dk::초기화(종목명, 배열크기(종목명));
		//dk::초기화(this, 크기(_종목정보_기본));
	}
} 종목정보_기본, * 종목정보_기본포;

typedef std::unordered_map<std::string, 종목정보_기본포> 유맵_종목_기본정보;

typedef struct _종목정보_키움
{
	bool 거래정지_키움{ false };		// 거래정지
	bool 관리종목_키움{ false };		// 관리종목
	bool 담보대출_키움{ false };		// 담보대출
	bool 신용가능_키움{ false };		// 신용가능
	bool 뮤추얼펀드_키움{ false };		// 뮤추얼 펀드	( 구려 )
	bool 신주인수권_키움{ false };		// 신주인수권	( 작전주 같음 )
	bool 리츠_키움{ false };			// 리츠			( 좋아 )
	bool ETF_키움{ false };				// ETF			( 코스피 코스닥에 많음 )
	BYTE 감리구분_키움{ 0 };			// [0]: 정상, [1]: 투자주의, [2]: 투자경고, [3]: 투자위험, [4]: 투자주의환기종목

	WORD 증거금_키움{ 0 };				// 증거금
	double 전일종가_키움{ 0.0 };			// 전일종가		( 모두 있음 )
	virtual void Init()
	{
		거래정지_키움 = 관리종목_키움 = 담보대출_키움 = 신용가능_키움 = 뮤추얼펀드_키움 = 신주인수권_키움 = 리츠_키움 = ETF_키움 = false;
		감리구분_키움 = 0;
		증거금_키움 = 0;
		전일종가_키움 = 0.0;
	}
} 종목정보_키움, * 종목정보_키움포;


