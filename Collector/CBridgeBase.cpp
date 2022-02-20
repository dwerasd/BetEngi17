#include "framework.h"
#include "CBridgeBase.h"



C_BRIDGE_BASE::C_BRIDGE_BASE(BridgeCallback_PROC _pCallback, LPCWSTR _pPipeRecvName, LPCWSTR _pPipeSendName)
	: pCallback(_pCallback)
{
	pPipe = new pipe::C_PIPE_SERVER(_pPipeRecvName, _pPipeSendName, this);
}

C_BRIDGE_BASE::~C_BRIDGE_BASE()
{
	Destroy();
	DSAFE_DELETE(pPipe);
}

void C_BRIDGE_BASE::Init(LPCSTR _pPath, LPCSTR _pParam)
{
	Destroy();
	디뷰("C_BRIDGE_CREON::Init() - Path: %s, Param: %s", _pPath, _pParam);
	ProcessInfo.SetPath(_pPath);
	if (_pParam) { ProcessInfo.SetParam(_pParam); }
	// PushPipePacket 함수로 들어온 패킷을 처리하는 스레드를 시작한다
	C_THREAD::ThreadStart();
	// 파이프 접속 스레드를 시작한다
	pPipe->ThreadStart();
	디뷰("C_BRIDGE_SAMPLE::ExecuteBridgeProcess() - 파이프 서버 생성");
}

void C_BRIDGE_BASE::Destroy()
{
	SetStatus(_브릿지_연결끊김_);
	C_THREAD::Terminate();	// 파이프 패킷 처리 스레드 해제.
}

void C_BRIDGE_BASE::ExecuteBridgeProcess()
{
	do
	{
		dk::Sleep();
	} while (!pPipe->IsOnThread());
	디뷰("C_BRIDGE_SAMPLE::ExecuteBridgeProcess() - 프로세스 실행");
	ProcessInfo.Execute();
}

void C_BRIDGE_BASE::TerminateBridgeProcess()
{
	if (GetStatus() != _브릿지_연결끊김_)
	{
		pPipe->Send(_PKT_PIPE_DESTROY_);
	}
	Destroy();
}


void C_BRIDGE_BASE::PushPipePacket(LPPACKET_BASE _pPacket)
{	// 여기로 온전한 패킷 한개가 들어온다.
	queuePipePackets.enqueue(_pPacket);
}

void C_BRIDGE_BASE::SetStatus(ULONG _nStatus)
{
	nStatus = _nStatus;
	strStatus = szStatus[_nStatus];
}

void C_BRIDGE_BASE::SendPipe(LPPACKET_BASE _pPacket)
{
	pPipe->Send(_pPacket);
}

void C_BRIDGE_BASE::SendPipe(WORD _dwHeader, LPVOID _pData, WORD _nSize)
{
	pPipe->Send(_dwHeader, _pData, _nSize);
}

DWORD C_BRIDGE_BASE::ThreadFunc(LPVOID _pParam)
{
	_pParam;
	LPPACKET_BASE pPipePacket = nullptr;
	do
	{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에 
		try
		{
			pPipePacket = nullptr;
			queuePipePackets.wait_dequeue(pPipePacket);
			if (pPipePacket)
			{
				if (!pPipePacket->nPacketIndex)
				{
					디뷰("C_BRIDGE_BASE::ThreadFunc() - 받음(_PKT_INDEX_NONE) 인덱스가 없음");
					//pLog->Write("파이프 받음(_PKT_INDEX_NONE) 인덱스가 없음");
				}
				else
				{	// 콜백으로 넘겨서 처리하도록 한다.
					if (pCallback)
					{
						pCallback(pPipePacket->nPacketIndex, pPipePacket->bytBuffer);
					}
				}
			}
		}
		catch (...)
		{
			디뷰메시지("Unexpected exception");
		}
		//디뷰("처리한 패킷 할당 해제");
		DSAFE_DELETE(pPipePacket);
	} while (true);
	return(0);
}

