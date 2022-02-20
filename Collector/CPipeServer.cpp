#include "framework.h"
#include "CPipeServer.h"
//#include "CNetEngine.h"



namespace pipe
{
	C_PIPE_SERVER::C_PIPE_SERVER(LPCWSTR _pRecv, LPCWSTR _pSend, C_PIPE_RECEIVER* _pReceiver)
		: bAccept(false)
		, wstrRecv(_pRecv)
		, wstrSend(_pSend)
		, pReceiver(_pReceiver)
		//, pPipeHandler(nullptr)
	{
		//wstrRecv = { L"CTOS" };
		//wstrSend = { L"STOC" };

		//pPipeHandler = new C_PIPE_SERVER_HANDLER();
		//pPipeHandler->SetPipe(this);
		//pPipeHandler->ThreadStart();
	}

	C_PIPE_SERVER::~C_PIPE_SERVER()
	{
		dk::C_THREAD::Terminate();
		dk::C_PIPE::Destroy();
	}

	int C_PIPE_SERVER::Recv(LPPACKET_BASE _pMessage)
	{
		DWORD dwRead = 0;
		if (INVALID_HANDLE_VALUE != hPipeRecv)
		{
			//DBGPRINT("C_PIPE_SERVER::Recv() - FlushFileBuffers()");
			::FlushFileBuffers(hPipeRecv);
			//DBGPRINT("C_PIPE_SERVER::Recv() - ReadFile(1)");
			if (!::ReadFile(hPipeRecv, (LPVOID)_pMessage, sizeof(PACKET_BASE), &dwRead, NULL))
			{
				//DBGPRINT("C_PIPE_SERVER::Recv() - ReadFile ret: -1");
				return -1;
			}
			if (_pMessage->nPacketSize != (dwRead - sizeof(PACKET_HEADER)))
			{
				DBGPRINT("_pMessage->nPacketSize: %d / dwRead: %d", _pMessage->nPacketSize, dwRead);
				return -2;
			}
			//DBGPRINT("C_PIPE_SERVER::Recv() - ReadFile(2)");
		}
		//DBGPRINT("C_PIPE_SERVER::Recv() - ReadFile ret: %d", dwRead);
		return(dwRead);
	}

	int C_PIPE_SERVER::Send(LPPACKET_BASE _pMessage)
	{
		DWORD dwWritten = 0;
		if (INVALID_HANDLE_VALUE != hPipeSend)
		{
			/*
			PACKET_BASE netPacket =
			{
				_pMessage->nPacketSize			// 데이터크기
				, _pMessage->nPacketIndex		// 헤더
				, { 0 }							// 보낼 내용
			};
			if (0 < _pMessage->nPacketSize)		// 헤더만 보낼 수도 있으니까
			{
				ZeroMemory(netPacket.bytBuffer, _countof(netPacket.bytBuffer));
				memcpy_s(netPacket.bytBuffer, _countof(netPacket.bytBuffer), _pMessage->bytBuffer, _pMessage->nPacketSize);
			}
			*/
			//DBGPRINT("C_PIPE_SERVER::Send: %d / %d / %d", _bUnicode, _nSize, _dwHeader);
			BOOL bResult = ::WriteFile(hPipeSend, (LPVOID)_pMessage, sizeof(PACKET_HEADER) + _pMessage->nPacketSize, &dwWritten, NULL);
			::FlushFileBuffers(hPipeSend);
			if (!bResult)
			{
				return -1;
			}
			if ((DWORD)(sizeof(PACKET_HEADER) + _pMessage->nPacketSize) != dwWritten)
			{
				return -2;
			}
		}
		////DBGPRINT("C_PIPE_SERVER::Send() ret");
		return dwWritten;
	}

	int C_PIPE_SERVER::Send(WORD _dwHeader, LPVOID _pData, WORD _nSize)
	{
		DWORD dwWritten = 0;
		if (INVALID_HANDLE_VALUE != hPipeSend)
		{
			PACKET_BASE netPacket =
			{
				_nSize			// 데이터크기
				, _dwHeader		// 헤더
				, { 0 }			// 보낼 내용
			};
			if (0 < _nSize)
			{
				ZeroMemory(netPacket.bytBuffer, _countof(netPacket.bytBuffer));
				memcpy_s(netPacket.bytBuffer, _countof(netPacket.bytBuffer), _pData, _nSize);
			}
			//DBGPRINT("C_PIPE_SERVER::Send: %d / %d / %d", _bUnicode, _nSize, _dwHeader);
			BOOL bResult = ::WriteFile(hPipeSend, (LPVOID)&netPacket, sizeof(PACKET_HEADER) + _nSize, &dwWritten, NULL);
			::FlushFileBuffers(hPipeSend);
			if (!bResult)
			{
				return -1;
			}
			if ((DWORD)(sizeof(PACKET_HEADER) + _nSize) != dwWritten)
			{
				return -2;
			}
		}
		//DBGPRINT("C_PIPE_SERVER::Send() ret");
		return(dwWritten);
	}

	DWORD C_PIPE_SERVER::ThreadFunc(LPVOID _pParam)
	{
		_pParam;
		DBGPRINT(L"C_PIPE_SERVER::ThreadFunc() - 파이프 서버 시작: %s /  %s / %x", wstrRecv.c_str(), wstrSend.c_str(), ::GetCurrentThreadId());
		dk::C_PIPE::Accept(wstrRecv.c_str(), wstrSend.c_str());
		do
		{
			if (!bAccept)
			{
				if (::ConnectNamedPipe(hPipeRecv, NULL))
				{
					DBGPRINT("C_PIPE_SERVER::ThreadFunc() - 파이프 연결 됨, _PKT_PIPE_CONNECTED_ 발송");
					Send(_PKT_PIPE_CONNECTED_);
					bAccept = true;
				}
				//else { DBGPRINT(__TEXT("ConnectNamedPipe 실패"));  }
				dk::멈춰(200);
			}
			else
			{
				PACKET_BASE NetPacketBuffer = { 0 };
				int nRecvSize = Recv(&NetPacketBuffer);
				//DBGPRINT(__TEXT("C_PIPE_SERVER::ThreadFunc(메시지 받음): %d / %d"), NetPacketBuffer.nPacketIndex, nRecvSize);
				if ((-1 < nRecvSize) && eventRecv.InValid())
				{
					eventRecv.Set();

					// 복사해서 큐에 넣기만 한다.
					LPPACKET_BASE pNetPacket = new PACKET_BASE();		// 새로 할당받는다.
					::memset(pNetPacket, 0, sizeof(PACKET_BASE));
					dk::CopyMem(pNetPacket, sizeof(PACKET_BASE), &NetPacketBuffer, nRecvSize);

					pReceiver->PushPipePacket(pNetPacket);
				}
				else
				{
					디뷰("C_PIPE_SERVER::ThreadFunc() - nRecvSize: %d or eventRecv.InValid()", nRecvSize);

					LPPACKET_BASE pNetPacket = new PACKET_BASE();		// 새로 할당받는다.
					::memset(pNetPacket, 0, sizeof(PACKET_BASE));
					//pNetPacket->nPacketIndex = _PKT_PIPE_DISCONNECTED_;
					//wcsncpy_s((wchar_t*)&pNetPacket->bytBuffer[0], 배열크기(pNetPacket->bytBuffer), wstrRecv.c_str(), wstrRecv.length());
					//pNetPacket->nPacketSize = wstrRecv.length() * 2;
					pNetPacket->nPacketIndex = _PKT_PIPE_DISCONNECTED_;
					pReceiver->PushPipePacket(pNetPacket);
					dk::C_PIPE::Destroy();
					dk::C_PIPE::Accept(wstrRecv.c_str(), wstrSend.c_str());
					bAccept = false;
				}
			}
		} while (true);
		eventRecv.Destroy();
		return(0);
	}
}
