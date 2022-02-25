#include "framework.h"
#include "CZmqBase.h"



C_ZMQ_BASE::C_ZMQ_BASE(int _nSocketType)
{
	pContext = zmq_ctx_new();
	pSocket = zmq_socket(pContext, _nSocketType);
}

C_ZMQ_BASE::~C_ZMQ_BASE()
{
	zmq_close(pSocket);
	zmq_ctx_destroy(pContext);
}

C_ZMQ_RECIVER::C_ZMQ_RECIVER(WORD _nPort, int _nSocketType)
	: C_ZMQ_BASE(_nSocketType)
{	// "tcp://*:5555"
	char 임시버퍼[_MAX_PATH];
	sprintf_s(임시버퍼, "tcp://*:%d", _nPort);
	zmq_bind(pSocket, 임시버퍼);
}

C_ZMQ_RECIVER::~C_ZMQ_RECIVER()
{
	C_ZMQ_BASE::~C_ZMQ_BASE();
}

int C_ZMQ_RECIVER::Recv(LPVOID _pBuffer, int _nBufferSize, int _nFlag)
{
	return zmq_recv(pSocket, _pBuffer, _nBufferSize, _nFlag);
}

C_ZMQ_SENDER::C_ZMQ_SENDER(LPCSTR _pAddr, WORD _nPort, int _nSocketType)
	: C_ZMQ_BASE(_nSocketType)
{	// "tcp://localhost:5555"
	char 임시버퍼[_MAX_PATH];
	sprintf_s(임시버퍼, "tcp://%s:%d", _pAddr, _nPort);
	zmq_connect(pSocket, 임시버퍼);
}

C_ZMQ_SENDER::~C_ZMQ_SENDER()
{
	C_ZMQ_BASE::~C_ZMQ_BASE();
}

int C_ZMQ_SENDER::Send(LPVOID _pBuffer, int _nSendSize, int _nFlag)
{
	return zmq_send(pSocket, _pBuffer, _nSendSize, _nFlag);
}

int C_ZMQ_SENDER::Send(WORD _nIndex, LPVOID _pBuffer, WORD _nSendSize, int _nFlag)
{
	PACKET_BASE packet{};
	packet.nPacketSize = _nSendSize;
	packet.nPacketIndex = _nIndex;
	::memcpy_s(packet.bytBuffer, sizeof(packet.bytBuffer), _pBuffer, _nSendSize);
	return zmq_send(pSocket, &packet, sizeof(PACKET_HEADER) + _nSendSize, _nFlag);
}

C_ZMQ::C_ZMQ()
{
	pReciver = new C_ZMQ_BASE(ZMQ_PULL);
	pSender = new C_ZMQ_BASE(ZMQ_PUSH);
}
C_ZMQ::~C_ZMQ()
{
	DSAFE_DELETE(pReciver);
	DSAFE_DELETE(pSender);
}

void C_ZMQ::bind(WORD _nPort)
{
	char 임시버퍼[_MAX_PATH];
	sprintf_s(임시버퍼, "tcp://*:%d", _nPort);
	zmq_bind(pReciver->pSocket, 임시버퍼);
}

void C_ZMQ::connect(LPCSTR _pAddr, WORD _nPort)
{
	char 임시버퍼[_MAX_PATH];
	sprintf_s(임시버퍼, "tcp://%s:%d", _pAddr, _nPort);
	zmq_connect(pSender->pSocket, 임시버퍼);
}

int C_ZMQ::Recv(LPVOID _pBuffer, int _nBufferSize, int _nFlag)
{
	return zmq_recv(pReciver->pSocket, _pBuffer, _nBufferSize, _nFlag);
}

int C_ZMQ::Send(LPVOID _pBuffer, int _nBufferSize, int _nFlag)
{
	return zmq_send(pSender->pSocket, _pBuffer, (_nBufferSize - 1), _nFlag);
}