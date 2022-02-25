#pragma once


#include <DarkCore/DSingleton.h>
#include <DarkCore/DTrayIcon.h>
#include <DarkCore/DMutex.h>
#include <DarkCore/DTypes.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DFile.h>
#pragma comment(lib, "DarkCore")

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>
#pragma comment(lib, "DOpenSources")

#include <Defines/DataDef.h>
#include <Defines/NetworkDef.h>

#include "CZmqBase.h"
#include "resource.h"

#include "CNetClient.h"
#include "CEngine.h"


class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>	// 단 한개만 생성되고 GetInstance 활성
	, public inet::C_IOCP_CALLBACK
	, public dk::C_THREAD
{
private:
	bool bWindowMode{ true }, bShowWindow{ false }, bExitProcess{ false }, bChangeSizeWindow{ false }, bHighPerformance{ false };
	HINSTANCE hInst{ 0 };
	HWND hWnd{ 0 }, hDlgMain{ 0 };

	LPCWSTR pClassName{ nullptr };

	dk::DPOINT 메인윈도우위치;
	dk::DSIZE 메인윈도우크기;

	dk::C_TRAY_ICON* pTrayIcon{ nullptr };

	dk::DRECT rcWindow, rcWorkSize;

	ULONG_PTR nSecondTimer{ 0 };
	ULONG_PTR nFrame{ 0 };

	
	moodycamel::BlockingConcurrentQueue<LPNET_PACKET_BUNDLE> queueNetworkPackets;
	void ReceivePacket(LPNET_PACKET_BUNDLE _pData);
	DWORD ThreadFunc(LPVOID _pParam);

public:
	C_MAIN(HINSTANCE _hInst);
	~C_MAIN();

	std::string 기본경로, 설정파일;
	C_ENGINE* pEngine{ nullptr };
	net::C_NET_CLIENT* pNetClient{ nullptr };
	size_t nCountSend{ 0 };

#if defined(_USE_ZEROMQ_)
	C_ZMQ_SENDER* pZmqSender{ nullptr };
#endif
	bool Init(LPCWSTR _wszClassName, LPCWSTR _wszWindowName, bool _bWindowMode = true);
	bool Create();
	long Calculate();
	void Display();
	void Destroy() noexcept;

	bool OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

	void ShowWindow(bool _bShow) { bShowWindow = _bShow; }
	void ExitProcess() { bExitProcess = true; }
	void SetHighPerformance(bool _b = true) { bHighPerformance = _b; }
	void StopHighPerformance(bool _b = false) { bHighPerformance = _b; }

	size_t DropFile(LPCSTR _pPath);
};