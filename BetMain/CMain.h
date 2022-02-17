#pragma once


#include <string>
#include <unordered_map>

#include <DarkCore/DDef.h>
#include <DarkCore/DTypes.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DTrayIcon.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DEvent.h>
#include <DarkCore/DMutex.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DFile.h>
#include <DarkCore/DLog.h>

#include <ta-lib/ta_libc.h>
#pragma comment(lib, "ta-lib")

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include "resource.h"

//#define _USE_WIN32_GUI_
#if defined(_USE_WIN32_GUI_)
#include <DarkCore/DWin32Obj.h>
#endif

#include <DX9Wrapp/CImGui.h>
#if defined(_USE_LIB_IMGUI_)
#pragma comment(lib, "DX9Wrapp")
#endif

#include "CConfig.h"
#if defined(_USE_NET_ENGINE_)
#include "CNetEngine.h"
#endif

#include "CBridgeKiwoom.h"

#include "GameDef.h"
#include "CGameEngine.h"
#include "COrder.h"

enum
{
	_팝업_메인메뉴_ = 0
	, _팝업_현황판_
	, _팝업_옵션_
	, _팝업_명령_
	, _팝업_정보_
	, _팝업_잔고_
	, _팝업_미체결_
	, _팝업_시뮬레이션_
	, _팝업창갯수_

};
#if (_MSC_VER > 1600) && (__cplusplus >= 201103L)	// vs2012 + c++11 이상
constexpr WORD _기본타이머_ = 1;
#else
#define _기본타이머_	(WORD)1
#endif


class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>	// 단 한개만 생성되고 GetInstance 활성
#if defined(_USE_WIN32_GUI_)
	, public dk::C_WIN32_WINDOW
#endif
	//, public dk::C_THREAD
#if defined(_USE_LIB_IMGUI_)
	, public C_IMGUI
#endif
{
private:
#if !defined(_USE_WIN32_GUI_)
	LPCWSTR pClassName{ nullptr };
	HWND hWnd{ 0 };
#endif
	bool bWindowMode{ true }, bShowWindow{ false }, bExitProcess{ false }, bHighPerformance{ false }, bChangeSizeWindow{ false };
	HINSTANCE hInst{ 0 };

	dk::C_LOG_EX* pLog{ nullptr };
	dk::C_TRAY_ICON* pTrayIcon{ nullptr };

	dk::DRECT rcWindow, rcWorkSize;

	bool 테스트모드{ true }, 자동실행{ false };
	BYTE 크레온_업데이트_모드{ 0 };

	C_CONFIG* 설정포{ nullptr };

	C_GAME* pGame{ nullptr };

	size_t DropFile(LPCSTR _pPath);
#if defined(_USE_NET_ENGINE_)
	C_NET_ENGINE* pNetEngi{ nullptr };
#endif
#if defined(_USE_WIN32_GUI_)
	dk::C_WIN32_LABEL* pLabel1{ nullptr };
	dk::C_WIN32_BUTTON* pButton1{ nullptr };

	std::vector<HFONT> vFonts;
	HWND hListView_NoticeBoard;
	HWND hListView_Conditions;
	HWND hListView_CaptureCondition;
	HWND hListboxStockCode{ nullptr };

	void CreateWin32Gui();
#endif

#if defined(_USE_LIB_IMGUI_)
	bool bDraw[_팝업창갯수_] = { false };
	long Update_ImGui();
	long Draw_MainMenu(bool& _bVisible);
	long Draw_Popup_StatusBoard(bool& _bVisible);
	long Draw_Popup_Option(bool& _bVisible);
	long Draw_Popup_Order(bool& _bVisible);
	long Draw_Popup_Info(bool& _bVisible);
	long Draw_Popup_Bank(bool& _bVisible);
	long Draw_Popup_WaitingList(bool& _bVisible);
#endif
	bool bInSecond5{ false };
	ULONG_PTR nSecondTimer{ 0 };
	ULONG_PTR nFrame{ 0 };

	WORD 기본타이머_초버퍼{ 0 };
	WORD 기본타이머_일버퍼{ 0 };
	ULONG nStartTime{ 0 }, nEndTime{ 0 };
	//
	dk::C_TIMER_QP 퍼포먼스타이머[2];
	dk::C_EVENT eventInitToday;
	void 스케줄생성(LPCSTR _파일명);
	void timer200();

	moodycamel::ConcurrentQueue<메인오더포> 오더큐;
	//moodycamel::BlockingConcurrentQueue<LPPACKET_BASE> 증권사패킷큐;
	//DWORD ThreadFunc(LPVOID _pParam);

public:
	C_MAIN(HINSTANCE _hInst);
	~C_MAIN();

	bool Init(LPCWSTR _wszClassName, LPCWSTR _wszWindowName, bool _bWindowMode = true);
	bool Create();
	long Calculate();
	void Display();
	void Destroy() noexcept;

	void ExitProcess() { bExitProcess = true; }

	C_BRIDGE_BASE* pBridgeCreon{ nullptr };
	C_BRIDGE_KIWOOM* pBridgeKiwoom{ nullptr };
	C_BRIDGE_BASE* pBridgeEBest{ nullptr };

	void 클라이언트_실행(BYTE _증권사, BYTE _실행모드 = 0);
	void 클라이언트_종료(BYTE _증권사);

	void PushOrder(메인오더포 _오더);

	bool OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);
};

#define 메인	C_MAIN::GetInstance