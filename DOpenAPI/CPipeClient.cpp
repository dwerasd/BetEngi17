#include "pch.h"
#include "CPipeClient.h"
#include "DOpenAPI.h"



namespace pipe
{
	C_PIPE_CLIENT::C_PIPE_CLIENT(LPCWSTR _pRecv, LPCWSTR _pSend)
		: bAccept(false)
		, wstrRecv(_pRecv)
		, wstrSend(_pSend)
		, pEventRecv(new dk::C_EVENT())
	{

	}

	C_PIPE_CLIENT::~C_PIPE_CLIENT()
	{
		Destroy();
	}

	int C_PIPE_CLIENT::Recv(LPPACKET_BASE _pData)
	{
		DWORD dwRead = 0;
		if (INVALID_HANDLE_VALUE != hPipeRecv)
		{
			::FlushFileBuffers(hPipeRecv);

			if (!::ReadFile(hPipeRecv, (LPVOID)_pData, sizeof(PACKET_BASE), &dwRead, NULL))
			{
				return -1;
			}
			if (_pData->nPacketSize != (dwRead - sizeof(PACKET_HEADER)))
			{
				DBGPRINT("_pData->nPacketSize: %d / dwRead: %d", _pData->nPacketSize, dwRead);
				return -2;
			}
		}
		return(dwRead);
	}

	int C_PIPE_CLIENT::Send(LPPACKET_BASE _pMessage)
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
		//DBGPRINT("C_PIPE_CLIENT_SERVER::Send() ret");
		return dwWritten;
	}

	int C_PIPE_CLIENT::Send(WORD _dwHeader, LPVOID _pData, WORD _nSize)
	{
		DWORD dwWritten = 0;
		if (INVALID_HANDLE_VALUE != hPipeSend)
		{
			PACKET_BASE netPacket =
			{
				_nSize				// 데이터크기
				, _dwHeader			// 헤더
				, { 0 }				// 보낼 내용
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
		//DBGPRINT("C_PIPE_CLIENT_SERVER::Send() ret");
		return(dwWritten);
	}

	DWORD C_PIPE_CLIENT::ThreadFunc(LPVOID _pParam)
	{
		_pParam;
		DBGPRINT(L"파이프 클라 시작: %s /  %s", wstrRecv.c_str(), wstrSend.c_str());
		do
		{
			if (!bAccept)
			{
				if (dk::C_PIPE::Connect(wstrRecv.c_str(), wstrSend.c_str()))
				{
					DBGPRINT("파이프 접속 완료");
					bAccept = true;
				}
				else
				{
					DBGPRINT("파이프 접속 실패, 프로그램을 종료합니다: %x", ulThreadId);
					//AfxMessageBox("파이프 서버에 접속할 수 없습니다");
					LPPACKET_BASE pNetPacket = new PACKET_BASE();		// 새로 할당받는다.
					pNetPacket->Init();
					pNetPacket->nPacketIndex = _PKT_PIPE_DESTROY_;
					theApp.PushReceivePacket(pNetPacket);				// 패킷을 직접 넣고
					Send(_브릿지패킷_키움_클라이언트_접속해제_);		// 접속 해제를 날린다.

					dk::C_PIPE::Destroy();								// 파이프 종료
					bEndPipe = true;
					break;
				}
				dk::멈춰(200);
			}
			else
			{
				PACKET_BASE NetPacketBuffer = { 0 };
				int nRecvSize = Recv(&NetPacketBuffer);
				if ((-1 < nRecvSize) && pEventRecv->InValid())
				{
					pEventRecv->Set();
					//DBGPRINT("C_PIPE_CLIENT::ThreadFunc(메시지 받음): %d / %d", NetPacketBuffer.nPacketIndex, nRecvSize);
					// 복사해서 큐에 넣기만 한다.
					LPPACKET_BASE 패킷포 = new PACKET_BASE();		// 새로 할당받는다.
					패킷포->Init();
					memcpy_s(패킷포, sizeof(PACKET_BASE), &NetPacketBuffer, nRecvSize);
					theApp.PushReceivePacket(패킷포);
				}
				else
				{	// 여기에 오면 파이프를 초기화하고 재접속 시도를 하게 된다.
					DBGPRINT("nRecvSize: %d or pEventRecv->InValid()", nRecvSize);
					dk::C_PIPE::Destroy();
					bAccept = false;
				}
			}
		} while (!bEndPipe);
		DSAFE_DELETE(pEventRecv);
		dk::C_THREAD::Terminate();							// 스레드 강제 종료를 한번 더 날린다.
		return(0);
	}
}
