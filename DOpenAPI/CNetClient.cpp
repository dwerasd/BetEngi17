#include "pch.h"
#include "CNetClient.h"
#include "DOpenAPI.h"



namespace net
{
	HANDLE hEventLogin = NULL;
	static COpenAPI* pEngine = nullptr;

	C_NET_CLIENT::C_NET_CLIENT(LPCSTR _pAddress, WORD _nPort)
		: nSocket(INVALID_SOCKET)
		, nPort(_nPort)
		, pNetPacketBundle(nullptr)
	{
		if (_pAddress)
		{
			::strcpy_s(szRemoteAddress, _pAddress);
		}
		else
		{
			::strcpy_s(szRemoteAddress, "127.0.0.1");
		}
		ZeroMemory(&netPacket, sizeof(PACKET_BASE));

		::WSAData wsaData;
		::WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (!pEngine)
		{
			pEngine = &theApp;
		}
	}

	C_NET_CLIENT::~C_NET_CLIENT()
	{
		::WSACleanup();
	}

	long C_NET_CLIENT::Connect(LPCSTR _pStrIP, WORD _nPort)
	{
		dk::_SOCKADDR_IN_EX sockAddr(_pStrIP, _nPort);

		dk::C_SOCKET sock;
		sock.Generate(AF_INET, SOCK_STREAM, IPPROTO_TCP);

		int nResult = ::connect(sock.GetSocket(), (struct sockaddr*)&sockAddr, sizeof(sockAddr));
		if (NO_ERROR == nResult)	// connect 는 성공하면 0 을 리턴한다.
		{
			nPort = _nPort;
			::strcpy_s(szRemoteAddress, sizeof(szRemoteAddress), _pStrIP);

			sock.SetNagle(1);	// nagle off
			nSocket = sock.Detatch();

			Send(_PKT_NET_CONNECTED_);
			디뷰("C_NET_CLIENT::Connect() - 성공");
			bAccept = true;
		}
		return(nResult);
	}

	void C_NET_CLIENT::Disconnect()
	{
		if (INVALID_SOCKET != nSocket)
		{
			::closesocket(nSocket);
			nSocket = INVALID_SOCKET;
		}
		bAccept = false;
	}

	void C_NET_CLIENT::Send(WORD _nIndex, LPBYTE _pData, WORD _nSize)
	{
		if (INVALID_SOCKET != nSocket)
		{	// 앞에 사이즈를 박고
			netPacket.nPacketSize = sizeof(PACKET_HEADER) + _nSize;	// 헤더만 보낼 수도 있다.
			netPacket.nPacketIndex = (WORD)_nIndex;					// 인덱스를 박고
			ZeroMemory(netPacket.bytBuffer, _countof(netPacket.bytBuffer));
			// 내용을 복사하고
			::memcpy_s(&netPacket.bytBuffer, _countof(netPacket.bytBuffer), _pData, _nSize);
			// 보낸다
			::send(nSocket, (char*)&netPacket, netPacket.nPacketSize, 0);
		}
	}

	void C_NET_CLIENT::Send(LPPACKET_BASE _pMessage)
	{
		if (INVALID_SOCKET != nSocket)
		{
			_pMessage->nPacketSize += sizeof(PACKET_HEADER);
			::send(nSocket, (char*)_pMessage, netPacket.nPacketSize, 0);
		}
	}

	long C_NET_CLIENT::HandleRecived(LPBYTE bytRecvBuf, int nReciveSize)
	{
		try
		{
			DBGPRINT("[ENGINE] 받음: %i / %s, 이벤트를 생성한다", nReciveSize, (bytRecvBuf + sizeof(PACKET_HEADER)));
			// 사실 처리할 패킷은 없다. 보내기만 할꺼니까.

		}
		catch (...)
		{
			디뷰위치();
		}
		DSAFE_DELETE(pNetPacketBundle);	// 이 함수를 호출하면 nullptr 로 수정하기 때문에 이 함수에서 해제해야한다.
		return(0);
	}

	DWORD C_NET_CLIENT::ThreadFunc(LPVOID _lpParam)
	{
		_lpParam;
		//CMDPRINT(_T("C_NET_CLIENT::ThreadFunc(0)"));
		do
		{
			try
			{
				if (!bAccept)
				{
					if (!Connect(szRemoteAddress, nPort))
					{
						bAccept = true;
					}
					else
					{
						DSAFE_DELETE(pNetPacketBundle);
					}
					dk::멈춰(500);
				}
				else
				{
					if (INVALID_SOCKET != nSocket)
					{
						BYTE bytRecvBuf[(1 << 14)] = { 0 };
						int nReciveSize = ::recv(nSocket, (char*)bytRecvBuf, sizeof(bytRecvBuf), 0);
						// 메시지가 들어오면
						if (0 < nReciveSize)
						{
							LPBYTE _pBuffer = &bytRecvBuf[0];
							int nCopySize = 0, nCopied = 0;
							do
							{
								if (pNetPacketBundle)		// 처리 안된 패킷이 있나?
								{
									// 채워야할크기 = 총크기 - 보관중인크기
									short nRemainSize = pNetPacketBundle->nBytesPacket - pNetPacketBundle->nBytesTransferred;
									// 복사할크기 = 받은크기가 남은크기보다 작으면 받은크기, 아니면 남은크기만큼 복사해서 완성하자.
									nCopySize = (nReciveSize < nRemainSize) ? nReciveSize : nRemainSize;
								}
								else // 처리 안된 패킷이 없을 경우
								{
									pNetPacketBundle = new NET_PACKET_BUNDLE();		// 새로 할당받는다.
									::memset(pNetPacketBundle, 0, sizeof(NET_PACKET_BUNDLE));
									// 받을 패킷의 총크기를 얻어온다.
									pNetPacketBundle->nBytesPacket = *(LPWORD)(_pBuffer);
									// 복사할크기 = 총크기보다 받은크기가 작으면 받은크기, 아니면 총크기만큼 복사해서 완성
									nCopySize = (nReciveSize < pNetPacketBundle->nBytesPacket) ? nReciveSize : pNetPacketBundle->nBytesPacket;
								}
								// 복사할크기는 (최대크기 - 보관된크기) 보다 작거나 같아야만 한다.
								if (((_MAX_PACKET_SIZE_ - _ALIGNMENT_) - (WORD)pNetPacketBundle->nBytesTransferred) >= nCopySize)
								{
									// 받은 내용에서 완성 될 만큼만 복사 ( 받아둔 크기를 더하고 빼는데 없으면 그냥 0 이다 )
									::memcpy_s(pNetPacketBundle->bytBuffer + pNetPacketBundle->nBytesTransferred
										, _countof(pNetPacketBundle->bytBuffer) - pNetPacketBundle->nBytesTransferred
										, _pBuffer + nCopied	// 처음에는 그냥 0 이다. 근데 다음번에 들어오게 되면 복사한 다음 위치이다.
										, nCopySize	// 완성 안될거 같으면 받은크기, 완성될거 같으면 딱 채울만큼 복사한다.
									);
									// 복사한만큼 더하면 이게 보관중인크기가 되는거다.
									pNetPacketBundle->nBytesTransferred += (WORD)nCopySize;
									// 패킷이 완성되었는가?
									if (pNetPacketBundle->nBytesPacket == pNetPacketBundle->nBytesTransferred)
									{
										//pNetPacketBundle->pSession = _pSession;	// 세션을 패킷에 붙이고
										//DataQueue.enqueue(pNetPacketBundle);	// 메모리 해제를 해야하니까 그대로 넘긴다.
										HandleRecived(pNetPacketBundle->bytBuffer, pNetPacketBundle->nBytesPacket);
										pNetPacketBundle = nullptr;				// 완성된 패킷 처리 끝.
									}
									nCopied += nCopySize;									// 남아있을 패킷을 위해 포인터를 저장.
									nReciveSize -= nCopySize;								// 받은크기도 복사한만큼 뺀다.
								}
								else
								{
									DSAFE_DELETE(pNetPacketBundle);
									Disconnect();
								}
							} while (0 < nReciveSize);		// 받은크기가 남았는가?
						}
						else // if (0 < nReciveSize)
						{
							DSAFE_DELETE(pNetPacketBundle);
							Disconnect();
						}
					}
					else // if (INVALID_SOCKET != nSocket)
					{
						DSAFE_DELETE(pNetPacketBundle);
						bAccept = false;
					}
				}
			}
			catch (...)
			{
				디뷰위치();
				DSAFE_DELETE(pNetPacketBundle);
				Disconnect();
			}
		} while (true);
		return(0);
	}
}
