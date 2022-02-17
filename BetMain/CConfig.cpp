#include "framework.h"
#include "CConfig.h"



C_CONFIG::C_CONFIG()
{

}

C_CONFIG::~C_CONFIG()
{

}

void C_CONFIG::초기화()
{
	char 임시버퍼[(1 << 10)] = { 0 };
	dk::GetCurrentDirectoryA(임시버퍼);
	기본경로 = 임시버퍼;

	// 실행파일 경로를 구하고
	::GetModuleFileNameA(::GetModuleHandleW(0), 임시버퍼, 배열크기(임시버퍼));
	// '\\' 를 '/' 로 수정
	for (size_t i = 0; i < ::strlen(임시버퍼); i++) { if ('\\' == 임시버퍼[i]) { 임시버퍼[i] = '/'; } }
	// 확장자를 자르고
	임시버퍼[::strlen(임시버퍼) - 3] = 0;
	// ini 를 붙인다.
	strcat_s(임시버퍼, "ini");
	설정파일 = 임시버퍼;

	sprintf_s(임시버퍼, "%sDCreonAPI.ini", 기본경로.c_str());
	크레온설정파일 = 임시버퍼;
	sprintf_s(임시버퍼, "%sDXingAPI.ini", 기본경로.c_str());
	이베스트설정파일 = 임시버퍼;

	if (!dk::파일체크(설정파일.c_str()))
	{	// 설정파일이 없으면 작성한다.
		디뷰("C_CONFIG::Init() - 설정파일이 없음: %s", 설정파일.c_str());
		캔들저장소 = "F:/data/StockSticks/";
		::WritePrivateProfileStringA("path", "sticks", 캔들저장소.c_str(), 크레온설정파일.c_str());

		스케줄저장소 = "F:/data/Schedule/";
		::WritePrivateProfileStringA("path", "schedule", 스케줄저장소.c_str(), 크레온설정파일.c_str());
	}
	else
	{
		디뷰("C_CONFIG::Init() - 설정파일: %s", 설정파일.c_str());
		테스트모드 = (bool)::GetPrivateProfileIntA("base", "test_mode", 0, 설정파일.c_str());
		if (!테스트모드)
		{
			자동실행 = (bool)::GetPrivateProfileIntA("base", "auto_mode", 0, 설정파일.c_str());
			if (자동실행)
			{
				디뷰("C_CONFIG::Init() - >>> 오토 모드 <<< ");
			}
		}
		else
		{
			디뷰("C_CONFIG::Init() - >>> 테스트 모드 <<< ");
		}
		::GetPrivateProfileStringA("path", "sticks", "", 임시버퍼, 배열크기(임시버퍼), 설정파일.c_str());
		캔들저장소 = 임시버퍼;
		::GetPrivateProfileStringA("path", "schedule", "", 임시버퍼, 배열크기(임시버퍼), 설정파일.c_str());
		스케줄저장소 = 임시버퍼;

		옵션_자동실행_파이프서버[_크레온_] = (bool)::GetPrivateProfileIntA("autostart", "pipe_server_creon", 0, 설정파일.c_str());
		옵션_자동실행_파이프서버[_키움_] = (bool)::GetPrivateProfileIntA("autostart", "pipe_server_kiwoom", 0, 설정파일.c_str());
		옵션_자동실행_파이프서버[_이베스트_] = (bool)::GetPrivateProfileIntA("autostart", "pipe_server_ebest", 0, 설정파일.c_str());
		옵션_자동실행_파이프서버[_한국투자_] = (bool)::GetPrivateProfileIntA("autostart", "pipe_server_korea", 0, 설정파일.c_str());
		옵션_자동실행_클라이언트[_크레온_] = (bool)::GetPrivateProfileIntA("autostart", "DCreonAPI", 0, 설정파일.c_str());
		옵션_자동실행_클라이언트[_키움_] = (bool)::GetPrivateProfileIntA("autostart", "DOpenAPI", 0, 설정파일.c_str());
		옵션_자동실행_클라이언트[_이베스트_] = (bool)::GetPrivateProfileIntA("autostart", "DXingAPI", 0, 설정파일.c_str());
		옵션_자동실행_클라이언트[_한국투자_] = (bool)::GetPrivateProfileIntA("autostart", "DKorAPI", 0, 설정파일.c_str());

		옵션_장후_업데이트_크레온 = (bool)::GetPrivateProfileIntA("autostart", "update_creon", 0, 설정()->경로_설정파일());

		메인윈도우위치.x = ::GetPrivateProfileIntA("location", "x", 0, 설정파일.c_str());
		메인윈도우위치.y = ::GetPrivateProfileIntA("location", "y", 0, 설정파일.c_str());
		메인윈도우크기.cx = ::GetPrivateProfileIntA("location", "w", 0, 설정파일.c_str());
		메인윈도우크기.cy = ::GetPrivateProfileIntA("location", "h", 0, 설정파일.c_str());

		사용_데이터베이스 = (bool)::GetPrivateProfileIntA("base", "use_database", 0, 설정파일.c_str());
		if (사용_데이터베이스)
		{
			디뷰("C_CONFIG::Init() - MariaDB 사용");
			::GetPrivateProfileStringA("database", "ip", "localhost", 임시버퍼, 배열크기(임시버퍼), 설정파일.c_str());
			디비_아이피 = 임시버퍼;
			::GetPrivateProfileStringA("database", "account", "root", 임시버퍼, 배열크기(임시버퍼), 설정파일.c_str());
			디비_계정 = 임시버퍼;
			::GetPrivateProfileStringA("database", "password", "akfldk1212", 임시버퍼, 배열크기(임시버퍼), 설정파일.c_str());
			디비_비밀번호 = 임시버퍼;
			디비_포트 = (USHORT)::GetPrivateProfileIntA("database", "port", 3306, 설정파일.c_str());
		}
		else
		{
			디뷰("C_CONFIG::Init() - MariaDB 사용 안함");
		}
	}

	if (설정()->사용_데이터베이스)
	{
		if (!데이터베이스)
		{	// 설정을 디비에서 읽어올까?
			데이터베이스 = new C_DATABASE();
			데이터베이스->ThreadStart();
			디비_접속여부 = 데이터베이스->Connect(디비_아이피.c_str(), 디비_포트, 디비_계정.c_str(), 디비_비밀번호.c_str());
			if (디비_접속여부)
			{
				디뷰("C_CONFIG::Init() - 데이터베이스 접속 성공");
				데이터베이스->기본세팅();
			}
			else
			{
				디뷰("C_CONFIG::Init() - 데이터베이스 접속 실패(%s:%d - %s / %s)", 디비_아이피.c_str(), 디비_포트, 디비_계정.c_str(), 디비_비밀번호.c_str());
			}
		}
	}


	디뷰("C_CONFIG::Init() - 캔들저장소: %s", 캔들저장소.c_str());
	디뷰("C_CONFIG::Init() - 스케줄저장소: %s", 스케줄저장소.c_str());
	if (!메인윈도우크기.cx) { 메인윈도우크기.cx = _MAIN_WINDOW_WIDTH_; }
	if (!메인윈도우크기.cy) { 메인윈도우크기.cy = _MAIN_WINDOW_HEIGHT_; }
	if (!메인윈도우위치.x) { 메인윈도우위치.x = ((::GetSystemMetrics(SM_CXSCREEN) - (int)메인윈도우크기.cx) / 2); }
	if (!메인윈도우위치.y) { 메인윈도우위치.y = ((::GetSystemMetrics(SM_CYSCREEN) - (int)메인윈도우크기.cy) / 2) - 200; }
	디뷰("C_CONFIG::Init() - 윈도우: %d / %d / %d / %d", 메인윈도우위치.x, 메인윈도우위치.y, 메인윈도우크기.cx, 메인윈도우크기.cy);
	// 크레온 설정파일에서 모드 상태를 얻어온다. ( BetMain 에서 설정 후 실행을 위해 )

	if (!dk::파일체크(크레온설정파일.c_str()))
	{	// 크레온 설정파일이 없으면 작성한다.
		디뷰("C_CONFIG::Init() - 크레온 설정파일이 없음: %s", 크레온설정파일.c_str());
		::WritePrivateProfileStringA("base", "test_mode", "0", 크레온설정파일.c_str());
		::WritePrivateProfileStringA("base", "server", "2", 크레온설정파일.c_str());
		::WritePrivateProfileStringA("update", "use_extra_candle", "1", 크레온설정파일.c_str());
		::WritePrivateProfileStringA("update", "update_candle_minute", "1", 크레온설정파일.c_str());
	}
	else
	{
		디뷰("C_CONFIG::Init() - 설정파일(크레온): %s", 크레온설정파일.c_str());
		크레온_업데이트_모드 = (BYTE)::GetPrivateProfileIntA("base", "server", 2, 크레온설정파일.c_str());
		디뷰("C_CONFIG::Init() - 업데이트모드(크레온): %d", 크레온_업데이트_모드);

		::GetPrivateProfileStringA("account", "creonplus_id", "", 임시버퍼, 배열크기(임시버퍼), 크레온설정파일.c_str());
		크레온계정 = 임시버퍼;
		::GetPrivateProfileStringA("account", "creonplus_pwd", "", 임시버퍼, 배열크기(임시버퍼), 크레온설정파일.c_str());
		크레온비밀번호 = 임시버퍼;
		::GetPrivateProfileStringA("account", "creonplus_certpwd", "", 임시버퍼, 배열크기(임시버퍼), 크레온설정파일.c_str());
		인증서비밀번호 = 임시버퍼;

	}
}

void C_CONFIG::소멸()
{
	if (데이터베이스)
	{
		디뷰("C_CONFIG::소멸() - MariaDB 접속 종료");
		데이터베이스->Disconnect();
	}
	DSAFE_DELETE(데이터베이스);
}

LPSTR C_CONFIG::MakePathSticks(LPSTR _경로버퍼, size_t _경로버퍼크기, BYTE _캔들의종류, WORD _캔들의길이, LPCSTR _코드, LPCSTR _확장자)
{
	dk::초기화(_경로버퍼, _경로버퍼크기);
	::strcpy_s(_경로버퍼, _경로버퍼크기, 캔들저장소.c_str());
	if (!::PathIsDirectoryA(_경로버퍼)) { ::CreateDirectoryA(_경로버퍼, NULL); }	// StockSticks 폴더가 없다면 생성.
	char 임시버퍼[(1 << 5)] = { 0 };
	if (0 == _캔들의종류)
	{	// 분봉, StockSticks/minute1/, StockSticks/minute15/
		::sprintf_s(임시버퍼, "minute%d/", _캔들의길이);
	}
	else if (1 == _캔들의종류)
	{	// StockSticks/day/
		::sprintf_s(임시버퍼, "day%d/", _캔들의길이);
	}
	else if (2 == _캔들의종류)
	{	// StockSticks/week/
		::sprintf_s(임시버퍼, "week%d/", _캔들의길이);
	}
	else if (3 == _캔들의종류)
	{	// StockSticks/month/
		::sprintf_s(임시버퍼, "month%d/", _캔들의길이);
	}
	else if (4 == _캔들의종류)
	{	// StockSticks/month/
		::sprintf_s(임시버퍼, "ticks/");
	}
	::strcat_s(_경로버퍼, _경로버퍼크기, 임시버퍼);
	// 폴더가 없으면 생성한다.
	if (!::PathIsDirectoryA(_경로버퍼)) { ::CreateDirectoryA(_경로버퍼, NULL); }
	if (_코드) { strcat_s(_경로버퍼, _경로버퍼크기, _코드); }
	if (_확장자) { strcat_s(_경로버퍼, _경로버퍼크기, _확장자); }
	return(_경로버퍼);
}

