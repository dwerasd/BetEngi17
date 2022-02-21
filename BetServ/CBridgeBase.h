#pragma once


#include <DarkCore/DUtil.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DSingleton.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include "CPipeServer.h"


enum
{
	_브릿지_연결끊김_ = 0
	, _브릿지_실행중_
	, _브릿지_로그인중_
	, _브릿지_연결됨_
	, _브릿지_종목전송중_
	, _브릿지_종목전송완료_
};

static char szStatus[][(1 << 8)] =
{
	"연결끊김"
	, "프로세스 실행중"
	, "로그인중"
	, "연결됨"
	, "종목전송중"
	, "종목전송완료"
};

typedef LPVOID(__stdcall* BridgeCallback_PROC) (ULONG_PTR _Order, LPVOID _pData);


class C_BRIDGE_BASE
	: public pipe::C_PIPE_RECEIVER	// 파이프서버에서 리시버의 PushPipePacket 함수에 패킷을 넣어준다.
	, public dk::C_THREAD
{
protected:
	friend pipe::C_PIPE_SERVER;
	pipe::C_PIPE_SERVER* pPipe{ nullptr };
	void PushPipePacket(LPPACKET_BASE _pPacket);
	moodycamel::BlockingConcurrentQueue<LPPACKET_BASE> queuePipePackets;

	ULONG nStatus{ 0 };
	std::string strStatus;

	dk::PROCESS_INFOMATION_EX ProcessInfo;
	BridgeCallback_PROC pCallback{ nullptr };
	DWORD ThreadFunc(LPVOID _pParam);

public:
	C_BRIDGE_BASE(BridgeCallback_PROC _pCallback, LPCWSTR _pPipeRecvName, LPCWSTR _pPipeSendName);
	~C_BRIDGE_BASE();

	virtual void Init(LPCSTR _pPath, LPCSTR _pParam = nullptr);	// 이건 파이프 경로를 받는걸로 하자.
	virtual void Destroy();

	virtual void ExecuteBridgeProcess();
	virtual void TerminateBridgeProcess();

	void SetStatus(ULONG _nStatus);
	ULONG GetStatus() { return(nStatus); }
	LPCSTR GetStatusText() { return(strStatus.c_str()); }

	void SendPipe(LPPACKET_BASE _pPacket);
	void SendPipe(WORD _dwHeader, LPVOID _pData = nullptr, WORD _nSize = 0);
};