#pragma once


#define _WINSOCKAPI_
#include <tchar.h>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi")

#include <Psapi.h>
#pragma comment(lib, "psapi")

#include <deque>
#include <vector>
#include <unordered_map>

#include <DarkCore/DDef.h>
#include <DarkCore/DMemory.h>
#include <DarkCore/DTimer.h>
#include <DarkCore/DSingleton.h>
#include <DarkCore/DThread.h>
#include <DarkCore/DUtil.h>
#include <DarkCore/DDeque.h>
#include <DarkCore/DFile.h>

#include <DX9Wrapp/CImGui.h>

#include <ta-lib/ta_libc.h>

#include <DOpenSources/readerwriterqueue.h>
#include <DOpenSources/blockingconcurrentqueue.h>

#include <Defines/DataDef.h>
#include <Defines/XingDef.h>

#include "CConfig.h"
#include "GameDef.h"

#include "CObjectMonster.h"



typedef std::unordered_map<std::string, std::string> UMAP_STRING_TO_STRING; 
typedef std::unordered_map<std::string, C_OBJECT_MONSTER*> UMAP_MONSTERS;

typedef std::unordered_map<std::string, 동적버퍼_캔들> 유맵_동적캔들;
class C_GAME
	: public dk::C_SINGLETON<C_GAME>
	, public dk::C_THREAD
{
private:
	// 체결데이터 저장할 버퍼
	ULONG 총누적호가잔량카운트{ 0 };
	// 모든 종목이 사용할 데이터는 여기에서 가지고 있기로 한다.
	LPBYTE 대용량임시버퍼{ nullptr }, 대용량임시버퍼포{ nullptr };

	ULONG 오늘날짜{ 0 };
	dk::C_TIMER_QP 퍼포먼스타이머[10];

	DWORD ThreadFunc(LPVOID _pParam);

	
	
public:
	C_GAME();
	~C_GAME();

	// 디비 호출은 느리니까 처음에 전부 가져와서 가지고 있는다.
	std::vector<std::string> 벡터_모든_종목코드;

	std::vector<std::string> 벡터_감시할_종목코드;
	std::vector<std::string> 벡터_감시중인_종목코드;

	UMAP_MONSTERS umObjectMonsters;
	void InitMonster(LPSTOCK_INFO_CREON _종목정보);
	C_OBJECT_MONSTER* FindMonster(LPCSTR _종목코드);
	void UpdateMonster(LPSTOCK_INFO_KIWOOM _종목정보);

	size_t nCountRealObjects{ 0 };		// 감시중인 종목 수		( size() 호출 비용을 줄인다 )
	ULONG nCountAccrueTick{ 0 };		// 들어온 체결틱 개수	
	LPBYTE pTickBuffer{ nullptr }, pTickBufferPtr{ nullptr };	// 이건 저장용 틱버퍼인데, 여기에선 저장하지 않기로 한다.

	LPTICK_DATA AppendTick(LPKIWOOM_REALDATA_TRANSACTION _pData);
	void PushTickData(LPKIWOOM_REALDATA_TRANSACTION _pData);
	// 저장된 틱을 파일에서 한개씩 넣는거다.
	LPTICK_DATA AppendTick(LPTICK_DATA _pData);
	void PushTickData(LPTICK_DATA _pData);

	LPTICK_DATA AppendTickKiwoom(LPKIWOOM_REALDATA_TRANSACTION _pData);

	void HandlerTick(LPTICK_DATA _pData);

	long LoadSticks(LPARRAY_STICK_BASE _캔들포);
	void PreReadySticks();

	char 선택된종목코드[(1 << 5)] = {"005930"};

	


	//동적버퍼_더블 보조지표버퍼[_보조지표버퍼수_];

	void 초기화();
	
	
	//void 종목추가_크레온(BYTE _장구분, LPCSTR _종목코드, LPCSTR _크레온코드, LPCSTR _풀코드, LPCSTR _종목명, ULONG _전일종가 = 0);
	
	
};
