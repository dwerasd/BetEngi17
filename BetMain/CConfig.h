#pragma once


#define _WINSOCKAPI_
#include <tchar.h>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")

#include <DarkCore/DSingleton.h>
#include <DarkCore/DUtil.h>

#include "CDatabase.h"
#include "CBridgeBase.h"


#if (_MSC_VER > 1600) && (__cplusplus >= 201103L)	// vs2012 + c++11 이상
constexpr long _MAIN_WINDOW_WIDTH_ = 800;	// 3840
constexpr long _MAIN_WINDOW_HEIGHT_ = 600;	// 2160
constexpr long _MIN_MAIN_WINDOW_WIDTH_ = 1280;
constexpr long _MIN_MAIN_WINDOW_HEIGHT_ = 800;
#else
#define _MAIN_WINDOW_WIDTH_			3840
#define _MAIN_WINDOW_HEIGHT_		2160
#define _MIN_MAIN_WINDOW_WIDTH_		1280
#define _MIN_MAIN_WINDOW_HEIGHT_	800
#endif

enum
{
	_크레온_ = 0
	, _키움_
	, _이베스트_
	, _한국투자_
	, _증권사갯수_
};

static char 증권사명[][(1 << 5)] = { "크 레 온", "키    움", "이베스트", "한국투자" };
static wchar_t 파이프서버이름[][(1 << 5)] =
{
	L"PipeServerCreon"
	, L"PipeServerKiwoom"
	, L"PipeServerEBest"
	, L"PipeServerKorea"
};
static wchar_t 파이프클라이름[][(1 << 5)] =
{
	L"PipeClientCreon"
	, L"PipeClientKiwoom"
	, L"PipeClientEBest"
	, L"PipeClientKorea"
};

typedef std::unordered_map<std::string, dk::LPPROCESS_INFOMATION_EX> UMAP_PROCESS;
class C_CONFIG
	: public dk::C_SINGLETON<C_CONFIG>
{
private:
	C_DATABASE* 데이터베이스{ nullptr };

	//WORD 중계기_통신상태[_증권사갯수_] = { 0 };
	//std::string 중계기_브릿지_출력용[_증권사갯수_];

public:
	C_CONFIG();
	~C_CONFIG();

	bool 옵션_자동실행_파이프서버[_증권사갯수_] = { false };
	bool 옵션_자동실행_클라이언트[_증권사갯수_] = { false };
	WORD 현재상태{ 0 };
	

	//dk::LPPROCESS_INFOMATION_EX 크레온플러스;
	//dk::LPPROCESS_INFOMATION_EX pProcessInfo[_증권사갯수_] = { nullptr };

	bool 옵션_장후_업데이트_크레온{ false };
	
	bool 테스트모드{ true }, 자동실행{ false };
	BYTE 크레온_업데이트_모드{ 0 };

	dk::DPOINT 메인윈도우위치;
	dk::DSIZE 메인윈도우크기;

	std::string 기본경로, 설정파일, 크레온설정파일, 이베스트설정파일;
	std::string 크레온계정, 크레온비밀번호, 인증서비밀번호;
	std::string 캔들저장소, 스케줄저장소;
	std::string 캐시저장소;

	bool 사용_데이터베이스{ false }, 디비_접속여부{ false };
	std::string 디비_아이피, 디비_계정, 디비_비밀번호;
	USHORT 디비_포트{ 3306 };

	ULONG 장시작시간{ 0 }, 장마감시간{ 0 };

	LPCSTR 경로_설정파일() { return(설정파일.c_str()); }
	LPCSTR 경로_설정파일_이베스트() { return(이베스트설정파일.c_str()); }
	LPCSTR 경로_캔들저장소() { return(캔들저장소.c_str()); }
	LPCSTR 경로_스케쥴저장소() { return(스케줄저장소.c_str()); }

	LPSTR MakePathSticks(LPSTR _경로버퍼, size_t _경로버퍼크기, BYTE _캔들의종류, WORD _캔들의길이, LPCSTR _코드, LPCSTR _확장자);
	LPSTR MakePrePath(LPSTR _경로버퍼, size_t _경로버퍼크기, BYTE _캔들의종류, WORD _캔들의길이, LPCSTR _코드, LPCSTR _확장자);

	void 초기화();
	void 소멸();


};

#define 설정	C_CONFIG::GetInstance

static char 설정파일_준비_날짜[] = { "test_ready_date" };

static char 설정파일_월봉_년_시작[] = { "M_year_start" };
static char 설정파일_월봉_년_끝[] = { "M_year_end" };
static char 설정파일_월봉_월_시작[] = { "M_month_start" };
static char 설정파일_월봉_월_끝[] = { "M_month_end" };

static char 설정파일_일봉_년_시작[] = { "D_year_start" };
static char 설정파일_일봉_년_끝[] = { "D_year_end" };
static char 설정파일_일봉_월_시작[] = { "D_month_start" };
static char 설정파일_일봉_월_끝[] = { "D_month_end" };
static char 설정파일_일봉_일_시작[] = { "D_day_start" };
static char 설정파일_일봉_일_끝[] = { "D_day_end" };

static char 설정파일_분봉_년_시작[] = { "B_year_start" };
static char 설정파일_분봉_년_끝[] = { "B_year_end" };
static char 설정파일_분봉_월_시작[] = { "B_month_start" };
static char 설정파일_분봉_월_끝[] = { "B_month_end" };
static char 설정파일_분봉_일_시작[] = { "B_day_start" };
static char 설정파일_분봉_일_끝[] = { "B_day_end" };


