
// OpenAPI.h: PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include <DarkCore/DTimer.h>
#include <DarkCore/DThread.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include "CNetClient.h"

#include "resource.h"		// 주 기호입니다.
#include "CKHOpenAPICtrl.h"			// 키움 필수
#if defined(_USE_RADAR_)
#include "CPipeClient.h"
#endif


// COpenAPI:
// 이 클래스의 구현에 대해서는 OpenAPI.cpp을(를) 참조하세요.
//

class COpenAPI
	: public CWinApp
	, public dk::C_THREAD
{
private:
	DECLARE_MESSAGE_MAP()

	dk::C_TIMER_QP 퍼포먼스타이머[2];

	moodycamel::BlockingConcurrentQueue<LPPACKET_BASE> 받은패킷큐;
	DWORD ThreadFunc(LPVOID _pParam);

public:
	COpenAPI();

	virtual BOOL InitInstance();

	// 여기부터 추가하는거다.
	C_KH_OPEN_API* 키움OCX{ nullptr };			// 키움 필수

	pipe::C_PIPE_CLIENT* pPipe{ nullptr };
	net::C_NET_CLIENT* pNetClient{ nullptr };

	void PushReceivePacket(LPPACKET_BASE _pData);
	
	//void RequestListConditions();			// 저장 된 키움 조건식 리스트 요청
	//void RequestListStocks();
};

extern COpenAPI theApp;
