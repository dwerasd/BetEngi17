#include "framework.h"
#include "CNetServer.h"
#include "CMain.h"



namespace net
{
	C_NET_SERVER::C_NET_SERVER(inet::C_IOCP_CALLBACK* _pNetCallback)
		: inet::C_IOCP_SERVER(
			_pNetCallback
			, true
			, 3
			, INADDR_ANY
			, 20000
		)
	{

	}

	C_NET_SERVER::~C_NET_SERVER()
	{

	}
	void C_NET_SERVER::ConnectionCompleted(inet::C_SESSION* _pSession)
	{
		inet::C_IOCP_SERVER::ConnectionCompleted(_pSession);
		vSessions.insert(_pSession);
		디뷰("vSessions.size(): %d", vSessions.size());
		if (0 < vSessions.size())
		{
			//C_MAIN::GetInstance()->bHighPerformance = true;
		}
	}
	void C_NET_SERVER::DisconnectCompleted(inet::C_SESSION* _pSession)
	{
		inet::C_IOCP_SERVER::DisconnectCompleted(_pSession);
		vSessions.erase(_pSession);
		if (0 == vSessions.size())
		{
			C_MAIN::GetInstance()->bHighPerformance = false;
		}
	}

}