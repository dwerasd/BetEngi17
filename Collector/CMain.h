#pragma once


#include <DarkCore/DSingleton.h>
#include <DarkCore/DTrayIcon.h>
#include <DarkCore/DMutex.h>
#include <DarkCore/DTypes.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DFile.h>
#pragma comment(lib, "DarkCore")

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>
#pragma comment(lib, "DOpenSources")

#include <Defines/DataDef.h>
#include <Defines/NetworkDef.h>

#include "CBridgeBase.h"
#include "CNetServer.h"
#include "resource.h"


typedef std::unordered_map<std::string, inet::C_SESSION*> UMAP_SESSIONS;

class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>	// �� �Ѱ��� �����ǰ� GetInstance Ȱ��
	, public inet::C_IOCP_CALLBACK
	, public dk::C_THREAD
{
private:
	bool bWindowMode{ true }, bShowWindow{ false }, bExitProcess{ false }, bChangeSizeWindow{ false };
	HINSTANCE hInst{ 0 };
	HWND hWnd{ 0 }, hDlgMain{ 0 };

	LPCWSTR pClassName{ nullptr };

	
	dk::DPOINT ������������ġ;
	dk::DSIZE ����������ũ��;

	dk::C_TRAY_ICON* pTrayIcon{ nullptr };

	dk::DRECT rcWindow, rcWorkSize;

	ULONG_PTR nSecondTimer{ 0 };
	ULONG_PTR nFrame{ 0 };

	net::C_NET_SERVER* pNet{ nullptr };

	typedef std::unordered_map<std::string, net::LPUSER_INFO> UMAP_USERS;
	UMAP_USERS umapUsers;


	size_t nCountRealObjects{ 0 };		// �������� ���� ��		( size() ȣ�� ����� ���δ� )


	moodycamel::BlockingConcurrentQueue<LPNET_PACKET_BUNDLE> queueNetworkPackets;
	DWORD ThreadFunc(LPVOID _pParam);

public:
	C_MAIN(HINSTANCE _hInst);
	~C_MAIN();

	bool bHighPerformance{ false };

	std::string �⺻���, ��������;

	ULONG nCountAccrueTick{ 0 };		// ���� ü��ƽ ����	
	LPBYTE pTickBuffer{ nullptr }, pTickBufferPtr{ nullptr };
	LPBYTE pTickBufferEBest{ nullptr }, pTickBufferPtrEBest{ nullptr };

	C_BRIDGE_BASE* pBridgeCreon{ nullptr };
	C_BRIDGE_BASE* pBridgeKiwoom{ nullptr };

	bool Init(LPCWSTR _wszClassName, LPCWSTR _wszWindowName, bool _bWindowMode = true);
	bool Create();
	long Calculate();
	void Display();
	void Destroy() noexcept;

	void ReceivePacket(LPNET_PACKET_BUNDLE _pData);

	LPTICK_DATA AppendTick(LPKIWOOM_REALDATA_TRANSACTION _pData);

	void ExitProcess() { bExitProcess = true; }
	void ShowWindow(bool _bShow) { bShowWindow = _bShow; }

	bool OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

};