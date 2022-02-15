#pragma once


#include <DNetwork/CIocpServer.h>
#pragma comment(lib, "DNetwork")



namespace net
{
	typedef struct _USER_INFO
	{
		inet::C_SESSION* pSession;

	} USER_INFO, *LPUSER_INFO;

	typedef struct _RECIVE_DATA
	{
		inet::C_SESSION* pSession;
		ULONG ulBytesTransferred;
		BYTE bytBuffer[(1 << 15)];
	} RECIVE_DATA, *LPRECIVE_DATA;

	class C_NET_SERVER
		: public inet::C_IOCP_SERVER
	{
	private:
		virtual void ConnectionCompleted(inet::C_SESSION* _pSocket);
		virtual void DisconnectCompleted(inet::C_SESSION*);
		//virtual void ReadCompleted(inet::C_SESSION* _pSocket, inet::C_IOCP_BUFFER* _pBuffer);
		//virtual void WriteCompleted(inet::C_SESSION* _pSocket, inet::C_IOCP_BUFFER* _pBuffer);

	public:
		C_NET_SERVER(inet::C_IOCP_CALLBACK* _pNetCallback);
		~C_NET_SERVER();

		std::set<inet::C_SESSION*> vSessions;
	};
}