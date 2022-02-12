#include "framework.h"
#include "CPipeClient.h"
#include "CMain.h"



namespace pipe
{
	static C_MAIN* pMain = nullptr;
	C_PIPE_CLIENT::C_PIPE_CLIENT(LPCWSTR _pRecv, LPCWSTR _pSend)
		: bAccept(false)
		, bThread(false)
		, wstrRecv(_pRecv)
		, wstrSend(_pSend)
	{
		pMain = C_MAIN::GetInstance();
		//wstrRecv = _pRecv;
		//wstrSend = _pSend;
		디뷰(L"C_PIPE_CLIENT::C_PIPE_CLIENT(): %x / %s / %s", this, _pRecv, _pSend);
	}

	C_PIPE_CLIENT::~C_PIPE_CLIENT()
	{
		//Destroy();
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
				DBGPRINT(" _pData->nPacketSize: %d / dwRead: %d", _pData->nPacketSize, dwRead);
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
			//DBGPRINT("C_PIPE_CLIENT::Send() - %d / %d", _pMessage->nPacketSize, _pMessage->nPacketIndex);
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
			//DBGPRINT("C_PIPE_CLIENT::Send() - %d / %d", _nSize, _dwHeader);
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
		디뷰("C_PIPE_CLIENT::ThreadFunc() - %x / %x", pMain, _pParam);
		_pParam;
		//디뷰(L"파이프 클라 시작: %s /  %s", wstrRecv.c_str(), wstrSend.c_str());
		bThread = true;
		do
		{
			if (!bAccept)
			{
				if (!Connect(wstrRecv.c_str(), wstrSend.c_str()))
				{
					if (!dk::프로세스체크(L"BetMain.exe"))
					{
						DBGPRINT("서버가 실행중이 아닙니다");
					}
					DBGPRINT("파이프 접속 실패, 프로그램을 종료합니다");
					LPPACKET_BASE pNetPacket = new PACKET_BASE();		// 새로 할당받는다.
					pNetPacket->Init();

					pNetPacket->nPacketIndex = _PKT_PIPE_DESTROY_;
					pMain->PushData(pNetPacket);
					break; 
				}
				else
				{
					DBGPRINT("파이프 접속 완료");
					bAccept = true;
				}
				dk::멈춰(200);
			}
			else
			{
				PACKET_BASE NetPacketBuffer = { 0 };
				int nRecvSize = Recv(&NetPacketBuffer);
				if ((-1 < nRecvSize) && eventRecv.InValid())
				{
					eventRecv.Set();
					//DBGPRINT("C_PIPE_CLIENT::ThreadFunc(메시지 받음): %d / %d", NetPacketBuffer.nPacketIndex, nRecvSize);
					// 복사해서 큐에 넣기만 한다.
					LPPACKET_BASE pNetPacket = new PACKET_BASE();		// 새로 할당받는다.
					pNetPacket->Init();

					memcpy_s(pNetPacket, sizeof(PACKET_BASE), &NetPacketBuffer, nRecvSize);
					pMain->PushData(pNetPacket);
				}
				else
				{
					DBGPRINT("nRecvSize: %d or eventRecv.InValid()", nRecvSize);
					Destroy();
					bAccept = false;
				}
			}
		} while (true);
		eventRecv.Destroy();
		bThread = false;
		return(0);
	}
}
