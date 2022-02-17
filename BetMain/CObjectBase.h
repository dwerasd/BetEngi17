#pragma once


#define _WINSOCKAPI_
#include <Windows.h>



class C_OBJECT_BASE
{
protected:
	char 별명[7] = { 0 };								// 종목코드
	char 아이디[8] = { 0 };								// 크레온코드 ( 주문할 때 필요 )
	char 고유식별코드[13] = { 0 };						// 풀코드
	char 닉네임[(1 << 6)] = { 0 };						// 종목명
	BYTE 직업{ 0 };										// [1]: 코스피, [2]: 코스닥, [3]: K-OTC, [4]: KRX, [5]: KONEX

public:
	C_OBJECT_BASE();
	~C_OBJECT_BASE();

	virtual void Init();

	void SetItemCode(LPCSTR);
	LPCSTR GetItemCode();

	void SetItemCreonCode(LPCSTR);
	LPCSTR GetItemCreonCode();

	void SetItemFullCode(LPCSTR);
	LPCSTR GetItemFullCode();

	void SetItemName(LPCSTR);
	LPCSTR GetItemName();

	void SetItemType(BYTE);
	BYTE GetItemType();
};
