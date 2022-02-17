#include "framework.h"
#include "CNetEngine.h"



C_NET_ENGINE::C_NET_ENGINE()
{

}

C_NET_ENGINE::~C_NET_ENGINE()
{
	Destroy();
}

void C_NET_ENGINE::Init()
{

}
//void C_NET_ENGINE::Create();
//void C_NET_ENGINE::Calculate();
//void C_NET_ENGINE::Display();
void C_NET_ENGINE::Destroy()
{
	DSAFE_DELETE(네트워크서버);
	for (size_t 증권사인덱스 = 0; 증권사인덱스 < _증권사갯수_; 증권사인덱스++)
	{
		DSAFE_DELETE(파이프서버[증권사인덱스]);
	}
	dk::C_THREAD::Terminate();
}

void C_NET_ENGINE::패킷분류스레드실행()
{
	if (INVALID_HANDLE_VALUE == GetCurrentThreadHandle())
	{	// 받은 패킷을 분류하는 스레드를 먼저 실행한다.
		//this->ThreadStart();
		ULONG_PTR 타임카운트 = 0;
		do
		{
			if (INVALID_HANDLE_VALUE != GetCurrentThreadHandle())
			{	// 다음 1초가 되기전에 이 루프를 빠져나가야만 정상이다.
				디뷰("C_NET_ENGINE::패킷분류스레드실행() - 스레드 생성 완료. (이건 한번만 호출 되어야함)");

				break;
			}
			ULONG_PTR 현재카운트 = dk::GetTickCount();
			if (1000 <= (현재카운트 - 타임카운트))
			{	// 1초에 한번씩 들어온다.
				디뷰("C_NET_ENGINE::패킷분류스레드실행() - 스레드 생성한다. (이건 한번만 호출 되어야함)");
				this->ThreadStart();	// 처음에 0을 빼기 때문에 한번 들어온다.
				타임카운트 = 현재카운트;
			}
			dk::멈춰();
		} while (true);
	}
}

void C_NET_ENGINE::네트워크_서버_실행(DWORD _아이피, WORD _포트)
{
	패킷분류스레드실행();
	if (!네트워크서버)
	{
		네트워크서버 = new net::C_NET_SERVER(_아이피, _포트);
		네트워크서버->ThreadStart();
	}
}

void C_NET_ENGINE::네트워크_서버_종료()
{
	DSAFE_DELETE(네트워크서버);
}

void C_NET_ENGINE::파이프_서버_실행(BYTE _증권사)
{
	패킷분류스레드실행();
	if (!파이프서버[_증권사])
	{	// 크레온 전용 파이프 서버 생성
		파이프서버[_증권사] = new pipe::C_PIPE_SERVER(파이프서버이름[_증권사], 파이프클라이름[_증권사], this);
		파이프서버[_증권사]->ThreadStart();
		디뷰("C_NET_ENGINE::통신_서버_실행() - %s", 증권사명);
		//pLog->Write(L"파이프 서버 스레드(크레온) 생성(1)");
	}
}
void C_NET_ENGINE::ReleasePipe(BYTE _증권사)
{
	if (파이프서버[_증권사])
	{
		//파이프서버[_증권사]->Destroy();
		DSAFE_DELETE(파이프서버[_증권사]);
	}
}
void C_NET_ENGINE::네트워크_패킷_전송(LPCSTR _아이디, LPPACKET_BASE _패킷)
{
	_아이디;
	_패킷;
}
void C_NET_ENGINE::네트워크_패킷_전송(LPCSTR _아이디, WORD _패킷인덱스, LPVOID _패킷, WORD _패킷크기)
{
	_아이디;
	_패킷인덱스;
	_패킷;
	_패킷크기;
}
void C_NET_ENGINE::SendPipe(BYTE _증권사, LPPACKET_BASE _패킷)
{
	파이프서버[_증권사]->Send(_패킷);
}

void C_NET_ENGINE::SendPipe(BYTE _증권사, WORD _패킷인덱스, LPVOID _패킷, WORD _패킷크기)
{
	파이프서버[_증권사]->Send(_패킷인덱스, _패킷, _패킷크기);
}

void C_NET_ENGINE::PushReceivePacket(LPPACKET_BASE _패킷)
{	// 여기로 온전한 패킷 한개가 들어온다.
	받은패킷큐.enqueue(_패킷);
}

DWORD C_NET_ENGINE::ThreadFunc(LPVOID _파라미터)
{
	_파라미터;
	LPPACKET_BASE pPipePacket = nullptr;
	do
	{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에 
		bool 다른큐로넘김 = false;
		try
		{
			pPipePacket = nullptr;
			받은패킷큐.wait_dequeue(pPipePacket);
			if (pPipePacket)
			{
				if (!pPipePacket->nPacketIndex)
				{
					디뷰("C_NET_ENGINE::ThreadFunc() - 받음(_PKT_INDEX_NONE) 인덱스가 없음");
					//pLog->Write("파이프 받음(_PKT_INDEX_NONE) 인덱스가 없음");
				}
				else
				{
					//디뷰("받음(): %d / %d / %d", pPipePacket->nPacketIndex, pPipePacket->bytClientType, pPipePacket->nClientVersion);
					if (_PKT_STOCK_PACKET_INDEX_NONE_ < pPipePacket->nPacketIndex && _PKT_STOCK_PACKET_INDEX_MAX_ > pPipePacket->nPacketIndex)
					{
						다른큐로넘김 = 증권사패킷큐.enqueue(pPipePacket);
					}
					/*
					if (_PKT_PIPE_CREON_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_CREON_MAX_ > pPipePacket->nPacketIndex)
					{	// 크레온 메시지
						증권사패킷큐.enqueue(pPipePacket);
					}
					else if (_PKT_PIPE_KIWOOM_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_KIWOOM_MAX_ > pPipePacket->nPacketIndex)
					{	// 키움
						증권사패킷큐.enqueue(pPipePacket);
					}
					else if (_PKT_PIPE_XINGAPI_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_XINGAPI_MAX_ > pPipePacket->nPacketIndex)
					{	// 이베스트
						증권사패킷큐.enqueue(pPipePacket);
					}
					else if (_PKT_PIPE_KOREA_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_KOREA_MAX_ > pPipePacket->nPacketIndex)
					{	// 한국투자증권
						//패킷_처리기_한국투자(pPipePacket);
					}
					*/
					else if (_PKT_NET_PACKET_NONE_ < pPipePacket->nPacketIndex && _PKT_NET_PACKET_MAX_ > pPipePacket->nPacketIndex)
					{	// 네트워크 패킷
						//네트워크패킷큐.enqueue(pPipePacket);
					}
				}
			}
		}
		catch (...)
		{
			디뷰메시지("Unexpected exception");
		}
		//디뷰("처리한 패킷 할당 해제");
		if (!다른큐로넘김)
		{
			DSAFE_DELETE(pPipePacket);
		}
	} while (true);
	return(0);
}
