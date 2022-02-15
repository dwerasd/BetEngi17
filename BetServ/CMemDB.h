#pragma once


#include <DarkCore/DSingleton.h>



class C_MEMDB
	: public dk::C_SINGLETON<C_MEMDB>
{
private:
	

public:
	C_MEMDB();
	~C_MEMDB();

	ULONG 날짜{ 0 };
	ULONG 장시작시간{ 0 }, 장마감시간{ 0 };

};