#pragma once


#include <DarkCore/DDef.h>
#include <DarkCore/DTypes.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DMutex.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include <Defines/NetworkDef.h>

#include "Resource.h"
#include "CTrayIcon.h"
#include "CXingAPI.h"
#include "CPipeClient.h"


typedef struct _종목정보
{
	BYTE 장구분{0}; // [1]: 코스피, [2]: 코스닥
	char 종목코드[(1 << 3)] = { 0 };
} 종목정보, * 종목정보포;


class C_MAIN
	: public dk::C_SINGLETON<C_MAIN>
	//, public dk::C_THREAD
{
private:
	HINSTANCE hInst;
	HWND hWnd;
	bool bEnable, bVisible, bChangeSizeWindow, bExitProcess;

	C_TRAY_ICON* pTrayIcon;

	std::string strClassName, strWindowName;
	dk::DRECT rcWindow;

	DWORD GetWinFormStyle(bool _bWindowMode);

	bool 테스트모드{ false };

	int 접속서버{ 0 };	// [0]: 실서버, [1]: 모의서버
	BOOL 에러메시지표시여부{ TRUE };

	std::string 기본경로, 캔들저장소;
	std::string 이베스트계정, 이베스트비밀번호, 인증서비밀번호;

	std::vector<std::string> 종목코드들;

	//std::unordered_map<std::string, 실시간_등록_이베스트> 실시간등록코드;
	std::vector<실시간_등록_이베스트> 실시간등록코드;

	C_XING_API* pXingAPI;
	pipe::C_PIPE_CLIENT* pPipe;

	moodycamel::ConcurrentQueue<LPPACKET_BASE> queueNetworkPackets;
	//DWORD ThreadFunc(LPVOID _pParam);

public:
	/*
	static C_MAIN *GetInstance()
	{
		static C_MAIN *pServer = nullptr;
		if (nullptr == pServer)
		{
			pServer = new C_MAIN();
		}
		return(pServer);
	}
	*/
	C_MAIN(HINSTANCE _hInst);
	~C_MAIN();

	bool Init(LPCSTR _tszClassName, LPCSTR _tszWindowName);
	bool Create();
	long Calculate();
	void Display();
	void Destroy();

	bool OnWindowMessage(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam);

	void PushReceivePacket(LPPACKET_BASE _pData);

	void 파이프전송(LPPACKET_BASE _pData);
	void 파이프전송(WORD _pData);
};

#define SERVER		C_MAIN::GetInstance
#define SERVERLOG	C_MAIN::GetInstance()->pServerLog->Write
