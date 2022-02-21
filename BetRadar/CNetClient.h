#pragma once


#include <DarkCore/DDef.h>
#include <DarkCore/DSocket.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DEvent.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DTimer.h>

#include <Defines/NetworkDef.h>



namespace net
{
	class C_NET_HANDLER
		: public dk::C_SINGLETON<C_NET_HANDLER>
		, public inet::C_IOCP_CALLBACK
		, public dk::C_THREAD
	{
	private:
		DWORD ThreadFunc(LPVOID _lpParam);

	public:

	};

	class C_NET_CLIENT
		: public dk::C_SINGLETON<C_NET_CLIENT>
		//, public inet::C_IOCP_CALLBACK
		, public dk::C_THREAD
	{
	private:
		bool bAccept{ false };
		SOCKET nSocket{ INVALID_SOCKET };

		WORD nPort{ 0 };
		char szRemoteAddress[INET_ADDRSTRLEN] = { 0 };

		LPNET_PACKET_BUNDLE pNetPacketBundle;
		PACKET_BASE netPacket;

		inet::C_IOCP_CALLBACK* pCallback{ nullptr };

		DWORD ThreadFunc(LPVOID _lpParam);

	public:
		C_NET_CLIENT(inet::C_IOCP_CALLBACK* _pCallback, LPCSTR _pAddress = nullptr, WORD _nPort = 20000);
		~C_NET_CLIENT();

		dk::C_EVENT eventConnect;

		long Connect(LPCSTR _pStrIP, WORD _nPort);
		void Send(WORD _nIndex, LPBYTE _pData = nullptr, WORD _nSize = 0);
		void Send(LPPACKET_BASE _pMessage);
		void Disconnect();

		bool IsConnect() { return(INVALID_SOCKET != nSocket); }
	};
}