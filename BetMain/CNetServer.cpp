#include "framework.h"
#include "CNetServer.h"
//#include "CNetEngine.h"



namespace net
{
	C_NET_SERVER::C_NET_SERVER(DWORD _아이피, WORD _포트)
		: pNet(nullptr)
		, 아이피(_아이피)
		, 포트(_포트)
	{

	}

	C_NET_SERVER::~C_NET_SERVER()
	{
		Destroy();
	}
	void C_NET_SERVER::Init()
	{

	}
	void C_NET_SERVER::Destroy()
	{
		dk::C_THREAD::Terminate();
		DSAFE_DELETE(pNet);
	}

	void C_NET_SERVER::ReceivePacket(LPNET_PACKET_BUNDLE _pData)
	{	// 여기로 온전한 패킷 한개가 들어온다.
		queueNetworkPackets.enqueue(_pData);
	}

	DWORD C_NET_SERVER::ThreadFunc(LPVOID _pParam)
	{
		DBGPRINT("C_NET_SERVER::ThreadFunc() - 네트워크 패킷 처리 스레드");
		_pParam;
		if (!pNet)
		{
			pNet = new inet::C_IOCP_SERVER(
				this
				, true			// Accept 에 버퍼를 추가할것인가?
				, 3				// 접속 후 3초동안 메시지가 오지 않으면 접속을 끊는다.
				, 아이피
				, 포트
			);
		}
		//pNet->SetValueWorkerThread(작업스레드갯수);
		pNet->ThreadStart();
		pNet->StartAcceptingConnections();
		DBGPRINT("C_NET_SERVER::ThreadFunc() - 네트워크 접속 처리 스레드 생성");

		LPNET_PACKET_BUNDLE pNetPacketBundle = nullptr;
		inet::C_SESSION* pSession = nullptr;
		do
		{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에 
			try
			{
				pNetPacketBundle = nullptr;
				queueNetworkPackets.wait_dequeue(pNetPacketBundle);
				if (nullptr != pNetPacketBundle)
				{	// pSession 이 nullptr 일 수는 없다.
					pSession = (inet::C_SESSION*)pNetPacketBundle->pSession;
					// 사이즈는 이제 의미가 없나? 내용만 처리하면 될 것 같다.
					if (pSession)
					{	// 세션이 없을 수는 없다.
						LPPACKET_BASE pNetPacket = (LPPACKET_BASE)&pNetPacketBundle->bytBuffer[0];
						if (_PKT_NET_PACKET_NONE_ < pNetPacket->nPacketIndex && _PKT_PACKET_INDEX_MAX_ > pNetPacket->nPacketIndex)
						{
							// 복사해서 큐에 넣기만 한다.
							LPPACKET_BASE pPacketBase = new PACKET_BASE();		// 새로 할당받는다.
							pPacketBase->Init();

							dk::CopyMem(pPacketBase, sizeof(PACKET_BASE), &pNetPacket, pNetPacket->nPacketSize);
							넷()->PushReceivePacket(pPacketBase);
						}
						/*
						switch (pNetPacket->nPacketIndex)
						{
						case _PKT_NET_PACKET_NONE_:
							DBGPRINT("C_NET_SERVER::ThreadFunc() - 받음(_PKT_INDEX_NONE): %d", pNetPacket->nPacketIndex);
							break;
						case _PKT_NET_CONNECTED_:
							DBGPRINT("C_NET_SERVER::ThreadFunc() - 받음(_PKT_NET_CONNECTED_): %s / 세션: %x", pNetPacket->bytBuffer, pNetPacketBundle->pSession);
							{
								char szBuffer[] = "_PKT_NET_CONNECTED_";
								pSession->Send(_PKT_NET_CONNECTED_, (LPBYTE)szBuffer, (WORD)strlen(szBuffer));
							}
							break;
						case _PKT_NET_DISCONNECTED_:
							pSession->Shutdown();
							break;
						case _PKT_NET_REQUEST_LOGIN_:
						{
							LPNET_MESSAGE_LOGIN pMessage = (LPNET_MESSAGE_LOGIN)pNetPacket->bytBuffer;
							if (0 == pMessage->bytLocale)
							{
								DBGPRINT("C_NET_SERVER::ThreadFunc() - 로그인 멀바: %s / %s", pMessage->szAccount, pMessage->szPassword);
								pSession->Send(_PKT_NET_RESULT_LOGIN_SUCCEED_);
							}
							else if (1 == pMessage->bytLocale)
							{
								DBGPRINT(L"C_NET_SERVER::ThreadFunc() - 로그인 유니: %s / %s", pMessage->wszAccount, pMessage->wszPassword);
								pSession->Send(_PKT_NET_RESULT_LOGIN_FAILED_);
							}
							
							//if (pDB->CheckAccount(pLoginPacket->szAccount))
							//{
							//	DBGPRINT("계정이 있어! - %s", pLoginPacket->szAccount);
							//}
							//else
							//{
							//	DBGPRINT("계정이 없어! - %s", pLoginPacket->szAccount);
							//}
							
						}
						break;
						case _PKT_NET_EXECUTE_:
						{
							LPNET_MESSAGE_EXECUTE_API pMessage = (LPNET_MESSAGE_EXECUTE_API)pNetPacket->bytBuffer;
							switch (pMessage->nAPIType)
							{
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							}
						}
						break;
						}
						*/
					}
					else
					{
						DBGPRINT("C_NET_SERVER::ThreadFunc() - C_NET_HANDLER::ThreadFunc() - 세션이 없을 수는 없는데?");
					}
				}
			}
			catch (...)
			{
				디뷰메시지("Unexpected exception");
			}
			//DBGPRINT("처리한 패킷 할당 해제");
			if (!pSession)
			{
				pSession = (inet::C_SESSION*)pNetPacketBundle->pSession;
			}
			pSession->ReleasePacketBundle();
		} while (true);
		return(0);
	}
}