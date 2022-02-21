#pragma once


#include <DarkCore/DDef.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DPipe.h>
#include <DarkCore/DEvent.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DLocale.h>

#include <Defines/NetworkDef.h>



namespace pipe
{
	class C_PIPE_CLIENT
		: public dk::C_PIPE
		, public dk::C_THREAD
	{
	private:
		bool bAccept{ false };
		dk::C_EVENT* pEventRecv;

		std::wstring wstrRecv;
		std::wstring wstrSend;

		DWORD ThreadFunc(LPVOID _pParam);

	public:
		C_PIPE_CLIENT(LPCWSTR _pRecv, LPCWSTR _pSend);
		C_PIPE_CLIENT(LPCSTR _pRecv, LPCSTR _pSend);
		~C_PIPE_CLIENT();

		int Recv(LPPACKET_BASE _pData);
		int Send(LPPACKET_BASE _pData);
		int Send(WORD _dwHeader, LPVOID _pData = nullptr, WORD _nSize = 0);
	};
}