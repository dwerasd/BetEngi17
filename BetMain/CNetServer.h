#pragma once


#include <DarkCore/DDef.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DSingleton.h>
#pragma comment(lib, "DarkCore")

#include <DKServer/CIocpServer.h>
#pragma comment(lib, "DKServer")

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>
#pragma comment(lib, "DOpenSources")


namespace net
{
	class C_NET_RECEIVER
	{
	public:
		C_NET_RECEIVER() { }
		virtual ~C_NET_RECEIVER() { }

		virtual void PushNetPacket(LPPACKET_BASE _pPacket) = 0;
	};

	typedef struct _USER_INFO
	{
		inet::C_SESSION* pSession;

	} USER_INFO, * LPUSER_INFO;

	typedef struct _RECIVE_DATA
	{
		inet::C_SESSION* pSession;
		ULONG ulBytesTransferred;
		BYTE bytBuffer[(1 << 14)];
	} RECIVE_DATA, * LPRECIVE_DATA;

	class C_NET_SERVER
		: public inet::C_IOCP_CALLBACK
		, public dk::C_THREAD
	{
	private:
		DWORD 아이피{ 0 };
		WORD 포트{ 0 };

		inet::C_IOCP_SERVER* pNet{ nullptr };
		//inet::C_IOCP_SERVER* 서버{ nullptr };

		typedef std::unordered_map<std::string, LPUSER_INFO> UMAP_USERS;
		UMAP_USERS umapUsers;

		moodycamel::BlockingConcurrentQueue<LPNET_PACKET_BUNDLE> queueNetworkPackets;
		DWORD ThreadFunc(LPVOID _pParam);

	public:
		C_NET_SERVER(DWORD _아이피, WORD _포트);
		~C_NET_SERVER();

		void Init();
		void Destroy();

		void inet::C_IOCP_CALLBACK::ReceivePacket(LPNET_PACKET_BUNDLE _pData);

		std::string strBetID, strBetPW;
		//bool Login(BYTE _nClientType, BYTE _nLocale, WORD _nClientVersion, LPCSTR _pUserID = nullptr, LPCSTR _pUserPW = nullptr);
	};
}