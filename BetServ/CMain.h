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

#include "resource.h"
#include "CZmqBase.h"



typedef std::unordered_map<std::string, LPORDERBOOK_KIWOOM> UMAP_ORDERBOOKS;

class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>	// 단 한개만 생성되고 GetInstance 활성
	, public dk::C_THREAD
{
private:
	bool bWindowMode{ true }, bShowWindow{ false }, bExitProcess{ false }, bChangeSizeWindow{ false };
	HINSTANCE hInst{ 0 };
	HWND hWnd{ 0 }, hDlgMain{ 0 };

	LPCWSTR pClassName{ nullptr };

	dk::DPOINT 메인윈도우위치;
	dk::DSIZE 메인윈도우크기;

	dk::C_TRAY_ICON* pTrayIcon{ nullptr };

	dk::DRECT rcWindow, rcWorkSize;

	ULONG_PTR nSecondTimer{ 0 };
	ULONG_PTR nFrame{ 0 };

	size_t nCountRealObjects{ 0 };		// 감시중인 종목 수		( size() 호출 비용을 줄인다 )

	C_ZMQ_RECIVER* pZmqReciver{ nullptr };
	//C_ZMQ_SENDER* pZmqCreon{ nullptr };
	//C_ZMQ_SENDER* pZmqKiwoom{ nullptr };

	moodycamel::BlockingConcurrentQueue<LPNET_PACKET_BUNDLE> queueNetworkPackets;
	DWORD ThreadFunc(LPVOID _pParam);

public:
	C_MAIN(HINSTANCE _hInst);
	~C_MAIN();

	bool bHighPerformance{ false };

	std::string 기본경로, 설정파일;

	ULONG nCountAccrueTick{ 0 };		// 들어온 체결틱 개수	
	LPBYTE pTickBuffer{ nullptr }, pTickBufferPtr{ nullptr };
	LPBYTE pTickBufferEBest{ nullptr }, pTickBufferPtrEBest{ nullptr };

	UMAP_ORDERBOOKS 키움호가데이터;

	bool Init(LPCWSTR _wszClassName, LPCWSTR _wszWindowName, bool _bWindowMode = true);
	bool Create();
	long Calculate();
	void Display();
	void Destroy() noexcept;
	
	void ExitProcess() { bExitProcess = true; }

	void ReceivePacket(LPNET_PACKET_BUNDLE _pData);

	LPTICK_DATAEXF AppendTickEx(LPKIWOOM_REALDATA_TRANSACTION _pData, LPORDERBOOK_KIWOOM _pOrderBook);

	void ShowWindow(bool _bShow) { bShowWindow = _bShow; }
	
	bool OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

};