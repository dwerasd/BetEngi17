#pragma once


#include <DarkCore/DUtil.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DThread.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include <Defines/DataDef.h>
#include <Defines/NetworkDef.h>

#include "CPipeServer.h"
#include "CBridgeBase.h"


#if (_MSC_VER > 1600) && (__cplusplus >= 201103L)	// vs2012 + c++11 이상
constexpr WORD _키움_총화면갯수_ = 2000;
#else
#define _키움_총화면갯수_	(WORD)2000
#endif
static wchar_t szPipeServerKiwoom[] = { L"PipeServerKiwoom" };
static wchar_t szPipeClientKiwoom[] = { L"PipeClientKiwoom" };

#pragma pack(push, 1)    // 1바이트 크기로 정렬
typedef struct _키움화면
{
	BYTE nUseCount{ 0 };						// 한 화면당 100개가 최고.
	char szScrNum[(1 << 3)] = { 0 };
	void Init() { dk::Init(this, sizeof(_키움화면)); }
} 키움화면, * 키움화면포;

typedef struct _키움조건식
{
	long nConditionIndex{ 0 };
	char szConditionName[(1 << 6)] = { 0 };
	void PushIndex(long _n)
	{	// 인덱스를 새로 넣는거니까 조건식명도 초기화한다.
		nConditionIndex = _n;
		dk::Init(szConditionName, 배열크기(szConditionName));
	}
} 키움조건식, * 키움조건식포;
#pragma pack(pop)			// 정렬 설정을 이전 상태(기본값)로 되돌림

//#define _조건식을유맵에보관_
#if defined(_조건식을유맵에보관_)
typedef std::unordered_map<long, std::string> 유맵_조건식;
#endif
class C_BRIDGE_KIWOOM
	: public dk::C_SINGLETON< C_BRIDGE_KIWOOM>
	, public C_BRIDGE_BASE
{
private:
	std::vector<키움화면포> vScreenNum;
#if defined(_조건식을유맵에보관_)
	유맵_조건식 유맵_키움조건식;
#else
	std::vector<키움조건식> vConditions;
#endif
	

public:
	C_BRIDGE_KIWOOM(BridgeCallback_PROC _pCallback, LPCWSTR _pPipeRecvName, LPCWSTR _pPipeSendName);
	~C_BRIDGE_KIWOOM();

	bool 키움_모의서버{ false };
	std::string 키움_아이디;
	std::string 키움_사용자명;
	std::string 키움_예수금요청화면번호;
	std::vector<std::string> vBankAccountNumber;
	BYTE 키움_선택된계좌인덱스{ 0 };
	키움_예수금상세현황 키움_예수금;

	void Init(LPCSTR _pPath, LPCSTR _pParam = nullptr);
	void Destroy();

	키움화면포 GetScreenNumber(bool _비어있는거 = false);
	void 키움_조건식등록(LPSTR _pConditionList);
};