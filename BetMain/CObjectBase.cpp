#include "framework.h"
#include "CObjectBase.h"



C_OBJECT_BASE::C_OBJECT_BASE()
{
	Init();
}

C_OBJECT_BASE::~C_OBJECT_BASE()
{

}

void C_OBJECT_BASE::Init()
{
	ZeroMemory(별명, _countof(별명));
	ZeroMemory(닉네임, _countof(닉네임));
	ZeroMemory(아이디, _countof(아이디));
	ZeroMemory(고유식별코드, _countof(고유식별코드));
}

void C_OBJECT_BASE::SetItemCode(LPCSTR _별명)
{
	if (_별명) { strcpy_s(별명, _별명); }
}

LPCSTR C_OBJECT_BASE::GetItemCode()
{
	return(별명[0] ? 별명 : nullptr);
}

void C_OBJECT_BASE::SetItemCreonCode(LPCSTR _아이디)
{
	if (_아이디) { strcpy_s(아이디, _아이디); }
}

LPCSTR C_OBJECT_BASE::GetItemCreonCode()
{
	return(아이디[0] ? 아이디 : nullptr);
}

void C_OBJECT_BASE::SetItemFullCode(LPCSTR _고유식별코드)
{
	if (_고유식별코드) { strcpy_s(고유식별코드, _고유식별코드); }
}

LPCSTR C_OBJECT_BASE::GetItemFullCode()
{
	return(고유식별코드[0] ? 고유식별코드 : nullptr);
}

void C_OBJECT_BASE::SetItemName(LPCSTR _닉네임)
{
	if (_닉네임) { strcpy_s(닉네임, _닉네임); }
}

LPCSTR C_OBJECT_BASE::GetItemName()
{
	return(닉네임[0] ? 닉네임 : nullptr);
}

void C_OBJECT_BASE::SetItemType(BYTE _직업)
{
	직업 = _직업;
}

BYTE C_OBJECT_BASE::GetItemType()
{
	return(직업);
}

