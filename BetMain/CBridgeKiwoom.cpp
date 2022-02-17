#include "framework.h"
#include "CBridgeKiwoom.h"



C_BRIDGE_KIWOOM::C_BRIDGE_KIWOOM(BridgeCallback_PROC _pCallback, LPCWSTR _pPipeRecvName, LPCWSTR _pPipeSendName)
	: C_BRIDGE_BASE(_pCallback, _pPipeRecvName, _pPipeSendName)
{

}

C_BRIDGE_KIWOOM::~C_BRIDGE_KIWOOM()
{
	Destroy();
	DSAFE_DELETE(pPipe);
}

void C_BRIDGE_KIWOOM::Init(LPCSTR _pPath, LPCSTR _pParam)
{
	C_BRIDGE_BASE::Init(_pPath, _pParam);

	for (int 화면인덱스 = 0; 화면인덱스 < _키움_총화면갯수_; 화면인덱스++)   // 2천개를 만들고 0은 안쓴다.
	{
		키움화면포 화면포 = new 키움화면();
		sprintf_s(화면포->szScrNum, "%04d", 화면인덱스);
		화면포->nUseCount = 0;
		vScreenNum.push_back(화면포);
	}
}

void C_BRIDGE_KIWOOM::Destroy()
{
	C_BRIDGE_BASE::Destroy();
	for (auto itr : vScreenNum)
	{
		DSAFE_DELETE(itr);
	}
	vScreenNum.clear();
	vConditions.clear();
	vBankAccountNumber.clear();
	C_THREAD::Terminate();
}

키움화면포 C_BRIDGE_KIWOOM::GetScreenNumber(bool _비어있는거)
{
	키움화면포 화면포 = nullptr;
	try
	{
		for (size_t 화면인덱스 = 1; 화면인덱스 < _키움_총화면갯수_; 화면인덱스++)
		{
			if (_비어있는거)
			{
				if (!vScreenNum[화면인덱스]->nUseCount)
				{
					화면포 = vScreenNum[화면인덱스];
					break;
				}
			}
			else
			{
				if (100 > vScreenNum[화면인덱스]->nUseCount)
				{
					화면포 = vScreenNum[화면인덱스];
					break;
				}
			}
		}
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
	return(화면포);
}

void C_BRIDGE_KIWOOM::키움_조건식등록(LPSTR _조건식모음)
{
	try
	{
#if !defined(_조건식을유맵에보관_)
		키움조건식 조건식;
#endif
		do
		{	// "014^나뭇잎의연화는두번핀다;004^지하에서피는꽃;002^지하에서피는연화;005^의식의흐름;016^형님들저도한입만;003^형님들저도한입만(-담불-증백);"
			LPSTR pResult = ::strrchr(_조건식모음, ';');			// 우측부터 ';'를 찾는다.
			if (!pResult) { break; }								// 찾지 못하면 반복문을 중지
			pResult += 1;											// ';' 를 찾았으면 그 다음 주소로 +1
			if (0 < ::strlen(pResult))								// 만약 파일의 끝에서 ';'를 찾았다면 + 1 이후에 널이다.
			{	// ';' 다음의 문자열 어차피 잘라야하니 지금 자르자
#if defined(_조건식을유맵에보관_)
				int 인덱스 = ::atoi(pResult);						// 검색식 인덱스를 보관하고, '^' 전까지만 숫자로 변환된다.
				LPSTR 조건식명 = ::strrchr(pResult, '^');			// '^' 를 찾는다.
				if (0 < ::strlen(조건식명))
				{
					조건식명 += sizeof(char);
					유맵_키움조건식[인덱스] = 조건식명;				// 검색식 인덱스를 키로해서 검색식 이름만 보관한다. GUI에서 인덱스로 검색식 이름을 포함해서 요청한다.
					//디뷰("(%d) %s", 인덱스, 조건식명);
				}
#else	// 벡터에 보관
				조건식.PushIndex(::atoi(pResult));						// 검색식 인덱스를 보관하고, '^' 전까지만 숫자로 변환된다.
				LPSTR 조건식명 = ::strrchr(pResult, '^');			// '^' 를 찾는다.
				if (0 < ::strlen(조건식명))
				{
					조건식명 += sizeof(char);
					strcpy_s(조건식.szConditionName, 조건식명);
					vConditions.push_back(조건식);
					//디뷰("(%d) %s", 조건식.인덱스, 조건식명);
				}
#endif
				
			}
			*(LPSTR)(pResult - 1) = 0;								// 문자열 앞에 ';' 이었던 바이너리를 0 으로 하면 그 곳이 문자열의 끝이 된다.
		} while (true);
		// 마지막 맨 앞의 문자열을 분리해서 map에 넣는다.
		LPSTR 조건식명 = ::strrchr(_조건식모음, '^');				// '^' 를 찾는다.
		if (0 < ::strlen(조건식명))
		{
			조건식명 += sizeof(char);								// '^' 다음의 조건식명 포인터를 보관하고
#if defined(_조건식을유맵에보관_)
			int 인덱스 = ::atoi(_조건식모음);
			유맵_키움조건식[인덱스] = 조건식명;
#else
			조건식.PushIndex(::atoi(_조건식모음));
			strcpy_s(조건식.szConditionName, 조건식명);
			vConditions.push_back(조건식);
#endif
			//디뷰("(%d) %s end / 조건식갯수: %d", 조건식.인덱스, 조건식명, vConditions.size());
		}
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}
