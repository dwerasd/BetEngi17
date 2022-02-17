#pragma once


#include <DarkCore/DDef.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DUtil.h>

#include "CConfig.h"
#include "CNetServer.h"
#include "CPipeServer.h"



class C_NET_ENGINE
	: public dk::C_SINGLETON<C_NET_ENGINE>
	, public dk::C_THREAD
	, public pipe::C_PIPE_RECEIVER
{
private:
	friend pipe::C_PIPE_SERVER;
	void PushPipePacket(LPPACKET_BASE _pPacket) { 받은패킷큐.enqueue(_pPacket); }

	pipe::C_PIPE_SERVER* 파이프서버[_증권사갯수_] = { nullptr };
	net::C_NET_SERVER* 네트워크서버{ nullptr };

	void 패킷분류스레드실행();

	moodycamel::BlockingConcurrentQueue<LPPACKET_BASE> 받은패킷큐;
	moodycamel::BlockingConcurrentQueue<LPPACKET_BASE> 증권사패킷큐;
	moodycamel::BlockingConcurrentQueue<LPPACKET_BASE> 네트워크패킷큐;
	DWORD ThreadFunc(LPVOID _pParam);

public:
	C_NET_ENGINE();
	~C_NET_ENGINE();

	void Init();
	//void Create();
	//void Calculate();
	//void Display();
	void Destroy();

	void PushReceivePacket(LPPACKET_BASE _pData);

	void 네트워크_서버_실행(DWORD _아이피 = INADDR_ANY, WORD _포트 = 20000);
	void 네트워크_서버_종료();

	void 네트워크_패킷_전송(LPCSTR _아이디, LPPACKET_BASE _pData);
	void 네트워크_패킷_전송(LPCSTR _아이디, WORD _dwHeader, LPVOID _pData = nullptr, WORD _nSize = 0);

	void 파이프_서버_실행(BYTE _증권사);
	void ReleasePipe(BYTE _증권사);

	void SendPipe(BYTE _증권사, LPPACKET_BASE _pData);
	void SendPipe(BYTE _증권사, WORD _dwHeader, LPVOID _pData = nullptr, WORD _nSize = 0);

	LPPACKET_BASE 증권사패킷꺼내기()
	{
		LPPACKET_BASE pPipePacket = nullptr;
		증권사패킷큐.wait_dequeue(pPipePacket);
		return(pPipePacket);
	}
	LPPACKET_BASE 네트워크패킷꺼내기()
	{
		LPPACKET_BASE pPipePacket = nullptr;
		네트워크패킷큐.wait_dequeue(pPipePacket);
		return(pPipePacket);
	}
};

#define 넷	C_NET_ENGINE::GetInstance