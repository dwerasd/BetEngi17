#pragma once


#include <DarkCore/DSocket.h>
//#include <Winsock2.h>
//#pragma comment(lib, "ws2_32")

#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")

#include <map>
#include <vector>

#include <DarkCore/DDef.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DLog.h>
#include <DarkCore/DFile.h>
#include <DarkCore/DTypes.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DMutex.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DSystemInfo.h>
#include <DarkCore/DSingleton.h>
#pragma comment(lib, "DarkCore")

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>
#pragma comment(lib, "DOpenSources")

#include <Defines/NetworkDef.h>
#include <Defines/DataDef.h>

// 이걸 반드시 써야하고 쓰면 /MP 를 사용하지 못한다.
#import <cpdib.dll>
#import <cputil.dll>
#import <cpsysdib.dll>


#include "resource.h"
#include "CTrayIcon.h"
#include "CPipeClient.h"


//#define _TRACING_DEBUGVIEW_

typedef struct _STOCK_CODE
{
	int nType;
	std::string strCode;
	std::string strName;
	std::string strFullCode;
} STOCK_CODE, * LPSTOCK_CODE, 종목코드_구조체, *종목코드_구조체포인터;

static dk::C_LOG* pLog = nullptr;

class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>
{
private:
	bool bEnable{ true }, bVisible{ false }, bChangeSizeWindow{ false };
	bool bExitProcess{ false };

	HINSTANCE hInst{ 0 };
	HWND hWnd{ 0 };

	bool 테스트모드{ false };
	bool 업데이트분봉{ false }, 확장캔들사용{ true };

	BYTE 접속서버{ 0 };
	WORD nCountSecond{ 0 };

	C_TRAY_ICON* pTrayIcon{ nullptr };
	// 메모리에 모아서 한번에 저장하기 위한 용도
	LPBYTE 대용량임시버퍼_시작포인터{ nullptr };			// 할당받은 버퍼의 위치
	LPBYTE 대용량임시버퍼_유동포인터{ nullptr };		// 체결틱을 저장할 위치

	std::string 기본경로, 캔들저장소;
	std::string 크레온계정, 크레온비밀번호, 인증서비밀번호;

	CPUTILLib::ICpCybos* pCybos{ nullptr };
	CPUTILLib::ICpStockCode* pStockCode{ nullptr };
	CPUTILLib::ICpCodeMgr* pCodeMgr{ nullptr };
	DSCBO1Lib::IDib* pStockBid{ nullptr };
	CPSYSDIBLib::ISysDib* pMarketEye{ nullptr };
	CPSYSDIBLib::ISysDib* pStockChart{ nullptr };

	//dk::DRECT rcWindow;
	dk::DPOINT ptWindow;
	dk::DSIZE sizeWindow;

	std::string strClassName, strWindowName;

	dk::C_TIMER_QP 퍼포먼스타이머[10];

	std::vector<종목코드_구조체> 모든종목벡터;
	std::vector<종목코드_구조체> 거른종목벡터;

	// 부구분코드 (CPE_KSE_SECTION_KIND)
	std::vector<std::string> vKseSectionKind;
	// 소속부 (CPE_MARKET_KIND)
	std::vector<std::string> vMarketType;
	// 감리구분 (CPE_CONTROL_KIND)
	std::vector<std::string> vControlType;
	// 관리구분 (CPE_SUPERVISION_KIND)
	std::vector<std::string> vSupervisionKind;
	// 정지구분 (CPE_STOCK_STATUS_KIND)
	std::vector<std::string> vStockStatusKind;
	
	void 요청가능확인();

	LPSTR 캔들경로세팅(LPSTR _경로포인터, size_t _경로버퍼크기, BYTE _캔들의종류, WORD _캔들의길이 = 1);
	void 캔들요청세팅(LPCSTR _종목코드, BYTE _캔들의종류, WORD _캔들의길이);
#if 0
	long 캔들받아서리턴(std::vector<확장캔들포>& _리턴벡터, LPCSTR _종목코드, BYTE _캔들의종류, WORD _캔들의길이, bool _새로요청 = false);
#endif
	bool 장중인가();
	void 종목갱신(LPCSTR _종목코드, 바이트 _캔들종류, 워드 _캔들길이);
	void 캔들갱신();
	void 캔들파일읽어서갱신(bool _덮어쓰기 = false);

	bool InitCreonAPI();
	void GetStockList();

	pipe::C_PIPE_CLIENT* pPipe;

	moodycamel::ConcurrentQueue<LPPACKET_BASE> queueNetworkPackets;

	DWORD GetWinFormStyle(bool _bWindowMode);

public:
	C_MAIN(HINSTANCE _hInst);
	~C_MAIN();

	bool Init(LPCSTR _tszClassName, LPCSTR _tszWindowName, WNDPROC _pWndProc);
	bool Create();
	bool Calculate();
	void Display();
	void Destroy();

	bool OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

	void PushData(LPPACKET_BASE _pData);
};


