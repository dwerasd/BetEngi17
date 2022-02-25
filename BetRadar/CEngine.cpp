#include "framework.h"
#include "CEngine.h"



C_ENGINE::C_ENGINE()
{

}

C_ENGINE::~C_ENGINE()
{
	힙해제(pTickBuffer);
}

void C_ENGINE::Init()
{
	//C_BRIDGE_KIWOOM::Init();
	nCountAccrueTick = 0;

	힙해제(pTickBuffer);
	pTickBuffer = pTickBufferPtr = (LPBYTE)힙할당(sizeof(TICK_DATA) * 40000000);
	디뷰("sizeof(체결틱): %d / 힙크기(pTickBuffer): %zd", sizeof(TICK_DATA), 힙크기(pTickBuffer));
}

void C_ENGINE::PushTickData(LPTICK_DATAEX _pData)
{	// 종목이 있든 없든 체결데이터 저장을 위해서 메모리에 적재한다.
	LPTICK_DATAEX pTick = AppendTickEx(_pData);
	//HandlerTick(pTick);
}

LPTICK_DATAEX C_ENGINE::AppendTickEx(LPTICK_DATAEX _pData)
{
	LPTICK_DATAEX pTick = (LPTICK_DATAEX)pTickBufferPtr;
	::memset(pTick, 0, sizeof(TICK_DATAEX));

	::memcpy_s(pTick, sizeof(TICK_DATAEX), _pData, sizeof(TICK_DATAEX));
	pTick->nSequence = nCountAccrueTick++;

	pTickBufferPtr += sizeof(TICK_DATAEX);	// 채운만큼 포인터 이동

	return(pTick);
}
