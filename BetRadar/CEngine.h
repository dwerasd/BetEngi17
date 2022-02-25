#pragma once


#include <DarkCore/DSingleton.h>
#include <DarkCore/DMemory.h>

#include <Defines/DataDef.h>
#include <Defines/NetworkDef.h>


class C_ENGINE
	: public dk::C_SINGLETON<C_ENGINE>
{
private:

	public:
	C_ENGINE();
	~C_ENGINE();

	ULONG nCountAccrueTick{ 0 };		// 들어온 체결틱 개수	
	LPBYTE pTickBuffer{ nullptr }, pTickBufferPtr{ nullptr };	// 이건 저장용 틱버퍼인데, 여기에선 저장하지 않기로 한다.

	void Init();

	LPTICK_DATAEX AppendTickEx(LPTICK_DATAEX _pData);
	void PushTickData(LPTICK_DATAEX _pData);
};