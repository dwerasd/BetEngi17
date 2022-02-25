#pragma once


#include <DarkCore/DUtil.h>

#define _USE_ZEROMQ_
#if defined(_USE_ZEROMQ_)
#include <libzmq/include/zmq.h>
#pragma comment(lib, "libzmq")
#endif

#include <Defines/NetworkDef.h>



class C_ZMQ_BASE
{
public:
	C_ZMQ_BASE(int _nSocketType);
	~C_ZMQ_BASE();

	LPVOID pContext{ nullptr }, pSocket{ nullptr };
};

class C_ZMQ_RECIVER
	: public C_ZMQ_BASE
{
public:
	C_ZMQ_RECIVER(WORD _nPort, int _nSocketType = ZMQ_PULL);
	~C_ZMQ_RECIVER();

	int Recv(LPVOID _pBuffer, int _nBufferSize, int _nFlag = ZMQ_DONTWAIT);
};

class C_ZMQ_SENDER
	: public C_ZMQ_BASE
{
public:
	C_ZMQ_SENDER(LPCSTR _pAddr, WORD _nPort, int _nSocketType = ZMQ_PUSH);		
	~C_ZMQ_SENDER();

	int Send(LPVOID _pBuffer, int _nSendSize, int _nFlag = ZMQ_DONTWAIT);
	int Send(WORD _nIndex, LPVOID _pBuffer, WORD _nSendSize, int _nFlag = ZMQ_DONTWAIT);
};

class C_ZMQ
{
private:
	C_ZMQ_BASE* pReciver{ nullptr };
	C_ZMQ_BASE* pSender{ nullptr };

public:
	C_ZMQ();
	~C_ZMQ();

	void bind(WORD _nPort);
	void connect(LPCSTR _pAddr, WORD _nPort);

	int Recv(LPVOID _pBuffer, int _nBufferSize, int _nFlag = ZMQ_DONTWAIT);
	int Send(LPVOID _pBuffer, int _nBufferSize, int _nFlag = ZMQ_DONTWAIT);
};
