#include "framework.h"
#include "CGameEngine.h"
#include "CMain.h"
#include "CConfig.h"



C_GAME::C_GAME()
{
	//if (!pNetEngi) { pNetEngi = 넷(); }
	
	::time_t tToday = ::time(0);		// 오늘 날짜를 구함
	char 날짜버퍼[(1 << 5)] = { 0 };
	::tm t;
	::localtime_s(&t, &tToday);
	::strftime(날짜버퍼, _countof(날짜버퍼), "%Y%m%d", &t);		// "20201215", 오늘 날짜 저장용
	오늘날짜 = ::atoi(날짜버퍼);									// 20201215 가 그대로 들어간다.

	if (INVALID_HANDLE_VALUE == GetCurrentThreadHandle())
	{
		this->ThreadStart();
	}
}

C_GAME::~C_GAME()
{
	힙해제(pTickBuffer);
	힙해제(대용량임시버퍼);
	dk::C_THREAD::Terminate();
}

void C_GAME::초기화()
{
	//C_BRIDGE_KIWOOM::Init();
	nCountAccrueTick = 0;

	힙해제(pTickBuffer);
	pTickBuffer = pTickBufferPtr = (LPBYTE)힙할당(sizeof(TICK_DATA) * 40000000);
	디뷰("sizeof(체결틱): %d / 힙크기(pTickBuffer): %zd", sizeof(TICK_DATA), 힙크기(pTickBuffer));

	힙해제(대용량임시버퍼);
	대용량임시버퍼 = 대용량임시버퍼포 = (LPBYTE)힙할당(sizeof(캔들_기본) * 2000000);
	디뷰("sizeof(캔들_기본): %d / 힙크기(대용량임시버퍼) = %zd", sizeof(캔들_기본), 힙크기(대용량임시버퍼));
	// 파일 읽기용 버퍼 초기화. 할당은 읽을때마다 새로 한다.
	
	size_t 코드갯수 = 벡터_감시할_종목코드.size();
	for (size_t 인덱스 = 0; 인덱스 < 코드갯수; 인덱스++)
	{
		DSAFE_DELETE(umObjectMonsters[벡터_감시할_종목코드[인덱스]]);
	}
	벡터_감시할_종목코드.clear();
	umObjectMonsters.clear();

	//char 임시버퍼[(1 << 10)] = { 0 };
	//::GetPrivateProfileStringA("path", "sticks", "", 임시버퍼, 배열크기(임시버퍼), 설정()->경로_설정파일());
}

C_OBJECT_MONSTER* C_GAME::FindMonster(LPCSTR _종목코드)
{
	UMAP_MONSTERS::iterator itr = umObjectMonsters.find(_종목코드);
	if (umObjectMonsters.end() != itr)
	{	// 찾음
		return(itr->second);
	}
	return(nullptr);
}

void C_GAME::InitMonster(LPSTOCK_INFO_CREON _종목정보)
{	// 있으믄 그냥 지우고 새로 할당받는거다.
	DSAFE_DELETE(umObjectMonsters[_종목정보->종목코드]);
	C_OBJECT_MONSTER* pStock = umObjectMonsters[_종목정보->종목코드] = new C_OBJECT_MONSTER();
	벡터_감시할_종목코드.push_back(_종목정보->종목코드);
	if (!_종목정보->장구분)
	{	// 여기 들어오면 시뮬레이션인거다.
		
	}
	else
	{
		pStock->C_OBJECT_BASE::SetItemType(_종목정보->장구분);
		pStock->C_OBJECT_BASE::SetItemCode(_종목정보->종목코드);
		pStock->C_OBJECT_BASE::SetItemCreonCode(_종목정보->크레온코드);
		pStock->C_OBJECT_BASE::SetItemFullCode(_종목정보->풀코드);
		pStock->C_OBJECT_BASE::SetItemName(_종목정보->종목명);

		pStock->상태 = _종목정보->상태;
		pStock->상태_크레온 = _종목정보->nControlType;
		pStock->섹션 = _종목정보->nKseSectionKind;
		pStock->관리종목 = _종목정보->nSupervisionType;
		pStock->스팩주 = _종목정보->스팩주;
		pStock->전일종가 = (double)_종목정보->전일종가;
	}
	char 캔들파일경로[_MAX_PATH];
	for (size_t 인덱스 = 0; 인덱스 < 배열크기(nMinuteLengths); 인덱스++)
	{
		pStock->분봉[인덱스].path = 설정()->MakePathSticks(캔들파일경로, 배열크기(캔들파일경로), 0, nMinuteLengths[인덱스], _종목정보->크레온코드, ".bet");
	}
	pStock->일봉.path = 설정()->MakePathSticks(캔들파일경로, 배열크기(캔들파일경로), 1, 1, _종목정보->크레온코드, ".bet");
	pStock->주봉.path = 설정()->MakePathSticks(캔들파일경로, 배열크기(캔들파일경로), 2, 1, _종목정보->크레온코드, ".bet");
	pStock->월봉.path = 설정()->MakePathSticks(캔들파일경로, 배열크기(캔들파일경로), 3, 1, _종목정보->크레온코드, ".bet");

	//디뷰("[%s] %s - 전일종가: %0.0f, %s"
	//	, pStock->C_OBJECT_BASE::GetItemCode()
	//	, pStock->일봉.경로.c_str()
	//	, pStock->전일종가
	//	, pStock->C_OBJECT_BASE::GetItemName()
	//);
}

void C_GAME::UpdateMonster(LPSTOCK_INFO_KIWOOM _종목정보)
{
	try
	{
		C_OBJECT_MONSTER* pStock = FindMonster(_종목정보->szStockCode);
		if (pStock)
		{
			//디뷰("찾음: %s, %s", _종목정보->szStockCode, pStock->C_OBJECT_BASE::GetItemCreonCode());
			pStock->키움_거래정지 = _종목정보->거래정지;
			pStock->키움_관리종목 = _종목정보->관리종목;
			pStock->키움_담보대출 = _종목정보->담보대출;
			pStock->키움_신용가능 = _종목정보->신용가능;
			pStock->키움_뮤추얼펀드 = _종목정보->뮤추얼펀드;
			pStock->키움_신주인수권 = _종목정보->신주인수권;
			pStock->키움_리츠 = _종목정보->리츠;
			pStock->키움_ETF = _종목정보->bETF;
			pStock->키움_ELW = _종목정보->bELW;
			pStock->키움_하이일드펀드 = _종목정보->하이일드펀드;
			pStock->키움_ELW = _종목정보->bELW;
			pStock->키움_K_OTC = _종목정보->K_OTC;
			pStock->키움_코넥스 = _종목정보->코넥스;
			pStock->키움_감리구분 = _종목정보->감리구분;
			pStock->키움_증거금 = _종목정보->증거금;
		}
		//else
		//{
		//	디뷰("크레온에서 걸렀는지 키움에서 못찾음: [%s] %s", _종목정보->szStockCode, _종목정보->szStockName);
		//}
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

LPTICK_DATA C_GAME::AppendTick(LPKIWOOM_REALDATA_TRANSACTION _pData)
{	// 일단 틱 데이터를 연속된 메모리에 변환해서 쌓는다.
	LPTICK_DATA pTickData = nullptr;
	if (pTickBuffer)
	{
		pTickData = (LPTICK_DATA)pTickBufferPtr;
		::memset(pTickData, 0, sizeof(TICK_DATA));

		pTickData->nSequence = nCountAccrueTick++;	// 더하고 증가, 순서는 0부터 기록한다.
		// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
		strncpy_s(pTickData->szCode, 배열크기(pTickData->szCode) * sizeof(char), _pData->szStockCode, 6);
		pTickData->szCode[6] = 0;
		// 이 함수가 호출된 시스템 시간을 얻어온다.
		dk::DLOCAL_TIME currentTime;
		// 밀리초를 더한다. ( 시간 변환은 여기서 저장용으로 한번만 한다.
		char szTime[(1 << 4)] = { 0 };
		// 아직 저장할게 아니니까 endian 변환은 하지 않는다.
		//sprintf_s(szTime, "%08x", (::atoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		//pTickData->nTime = ::ntohl(::strtol(szTime, NULL, 16));
		sprintf_s(szTime, "%d", (::atoi(_pData->szTime) * 1000) + currentTime.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pTickData->nTime = ::atoi(szTime);

		// 매수 매도인지 여기서 판단 후에 종목에 넘겨주도록 하자.
		if ('+' == _pData->szVolume[0]) { pTickData->nTransType = 1; }		// 매수 체결
		else if ('-' == _pData->szVolume[0]) { pTickData->nTransType = 2; }	// 매도체결
		else
		{	
			{
				pTickData->nTransType = 3;	// 전일종가와 동일할 경우
			}
		}
		// 부호가 붙어 있으면 무시하고 숫자만 변환
		pTickData->fPrice = (float)dk::atoi_s(_pData->szClose);
		pTickData->nTransVolume = dk::atoi_s(_pData->szVolume);
		pTickData->nAccrueVolume = dk::atoi_s(_pData->szAccrueVolume);
		pTickData->최우선매도호가 = (float)dk::atoi_s(_pData->szPriceSell);
		pTickData->최우선매수호가 = (float)dk::atoi_s(_pData->szPriceBuy);
		//디뷰("%d / %d", pTickData->nTransVolume, pTickData->nAccrueVolume);	// 왜 이걸 안쓰면 누락이 발생하는거지
		pTickBufferPtr += sizeof(TICK_DATA);	// 채운만큼 포인터 이동
	}
	return(pTickData);
}

LPTICK_DATA C_GAME::AppendTick(LPTICK_DATA _pData)
{
	LPTICK_DATA pTick = (LPTICK_DATA)pTickBufferPtr;
	::memset(pTick, 0, sizeof(TICK_DATA));

	::memcpy_s(pTick, sizeof(TICK_DATA), _pData, sizeof(TICK_DATA));
	pTick->nSequence = nCountAccrueTick++;

	pTickBufferPtr += sizeof(TICK_DATA);	// 채운만큼 포인터 이동

	return(pTick);
}

LPTICK_DATA C_GAME::AppendTickKiwoom(LPKIWOOM_REALDATA_TRANSACTION _pData)
{	// 일단 틱 데이터를 연속된 메모리에 변환해서 쌓는다.
	LPTICK_DATA pTickData = nullptr;
	if (pTickBuffer)
	{
		pTickData = (LPTICK_DATA)pTickBufferPtr;
		::memset(pTickData, 0, sizeof(TICK_DATA));

		pTickData->nSequence = nCountAccrueTick++;	// 더하고 증가, 순서는 0부터 기록한다.
		// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
		strncpy_s(pTickData->szCode, 배열크기(pTickData->szCode) * sizeof(char), _pData->szStockCode, 6);
		pTickData->szCode[6] = 0;
		// 이 함수가 호출된 시스템 시간을 얻어온다.
		dk::DLOCAL_TIME currentTime;
		// 밀리초를 더한다. ( 시간 변환은 여기서 저장용으로 한번만 한다.
		char szTime[(1 << 4)] = { 0 };
		// 아직 저장할게 아니니까 endian 변환은 하지 않는다.
		//sprintf_s(szTime, "%08x", (::atoi(_szTime) * 1000) + cur_time.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		//pTickData->nTime = ::ntohl(::strtol(szTime, NULL, 16));
		sprintf_s(szTime, "%d", (::atoi(_pData->szTime) * 1000) + currentTime.wMilliseconds);		// 9000000, 11034600, 12593100, 15300000
		pTickData->nTime = ::atoi(szTime);

		// 매수 매도인지 여기서 판단 후에 종목에 넘겨주도록 하자.
		if ('+' == _pData->szVolume[0]) { pTickData->nTransType = 1; }		// 매수 체결
		else if ('-' == _pData->szVolume[0]) { pTickData->nTransType = 2; }	// 매도체결
		else
		{	/*
			// 단일가 매매의 경우 전일 종가와 비교해 부호가 붙는다.
			if ('+' == _szClose[0])
			{
				pTickData->nTransType = 4;
			}
			else if ('-' == _szClose[0])
			{
				pTickData->nTransType = 5;
			}
			else
			*/
			{
				pTickData->nTransType = 3;	// 전일종가와 동일할 경우
			}
		}
		// 부호가 붙어 있으면 무시하고 숫자만 변환
		pTickData->fPrice = (float)dk::atoi_s(_pData->szClose);
		pTickData->nTransVolume = dk::atoi_s(_pData->szVolume);
		pTickData->nAccrueVolume = dk::atoi_s(_pData->szAccrueVolume);
		pTickData->최우선매도호가 = (float)dk::atoi_s(_pData->szPriceSell);
		pTickData->최우선매수호가 = (float)dk::atoi_s(_pData->szPriceBuy);
		//디뷰("%d / %d", pTickData->nTransVolume, pTickData->nAccrueVolume);	// 왜 이걸 안쓰면 누락이 발생하는거지
		pTickBufferPtr += sizeof(TICK_DATA);	// 채운만큼 포인터 이동
	}
	return(pTickData);
}

void C_GAME::PushTickData(LPKIWOOM_REALDATA_TRANSACTION _pTick)
{	// 여기로 키움 체결 데이터가 들어오는거다.
	LPTICK_DATA pTick = AppendTick(_pTick);
	//HandlerTick(pTick);
}

void C_GAME::PushTickData(LPTICK_DATA _pData)
{	// 종목이 있든 없든 체결데이터 저장을 위해서 메모리에 적재한다.
	LPTICK_DATA pTick = AppendTick(_pData);
	HandlerTick(pTick);
}

void C_GAME::HandlerTick(LPTICK_DATA _pTick)
{	///////////////////////////////////////////////////////////////////////////////
	// 여기에서 틱 처리 하자.
	///////////////////////////////////////////////////////////////////////////////
	C_OBJECT_MONSTER* pStock = umObjectMonsters[_pTick->szCode];
	if (!pStock)
	{	// 여기는 체결데이터 밀어넣을떄나 들어올꺼다.
		pStock = umObjectMonsters[_pTick->szCode] = new C_OBJECT_MONSTER();
		pStock->Init(_pTick->szCode);
		//벡터_감시중인_종목코드.push_back(_pTick->szCode);
	}
	if (!pStock->누적체결건수++)	// 종목당 체결 카운트
	{	// 데이터가 처음들어오는거면 카운트를 증가시킨다.
		벡터_감시중인_종목코드.push_back(_pTick->szCode);
		nCountRealObjects++;	// 체결로 들어오는 종목수를 체크하기 위한 변수.
		pStock->vTransactionTicks.clear();
	}
	if (0 < _pTick->nTransVolume)
	{
		//디뷰("[%s] 체결량: %d", _pTick->szCode, _pTick->nTransVolume);	// 왜 이 로그를 남기면 누락이 발생하지 않는거지???
		pStock->PushTick(_pTick);						// 해당 체결 데이터를 가지고 
	}
	//종목포->Update();
//종목정보_확장포 종목포 = 종목찾기(_체결변환포->종목코드);
//if (종목포)
//{	

	// 이제 종목별 데이터를 업데이트 한다
//	종목포->Update(_체결변환포);
//	종목포->연산();
//}
}

long C_GAME::LoadSticks(LPARRAY_STICK_BASE _캔들포)
{
	long nResult = 0;
	if (1 == dk::파일체크(_캔들포->path.c_str()))
	{
		dk::C_FILE 파일(_캔들포->path.c_str());
		do
		{
			LPBYTE pFileData = (LPBYTE)파일.시작포인터();
			베트파일헤더포 헤더포 = (베트파일헤더포)pFileData;
			if (0x0000E90C == 헤더포->nFileType)
			{
				ULONG 확장캔들갯수 = (파일.파일크기() - 크기(베트파일헤더)) / 크기(확장캔들);
				ULONG 읽을캔들갯수 = (확장캔들갯수 < _MAX_ARRAY_BUFFER_SIZE_) ? 확장캔들갯수 : _MAX_ARRAY_BUFFER_SIZE_;
				ULONG 현재위치 = 파일.파일크기() - (크기(확장캔들) * 읽을캔들갯수);	// 위치 
				for (size_t 인덱스 = 0; 인덱스 < 읽을캔들갯수; 인덱스++)
				{	// 최근 데이터를 _MAX_ARRAY_BUFFER_SIZE_ 개만 가져온다.
					확장캔들포 파일캔들포 = (확장캔들포)(pFileData + 현재위치);
					_캔들포->push_back(
						::ntohl(파일캔들포->nDate)
						, ::ntohl(파일캔들포->nTime)
						, 파일캔들포->nTotalVolume
						, (double)파일캔들포->fOpen
						, (double)파일캔들포->fHigh
						, (double)파일캔들포->fLow
						, (double)파일캔들포->fClose
					);
					현재위치 += sizeof(확장캔들);
				}
			}
		} while (false);
		파일.닫기();
	}
	return(nResult);
}

void C_GAME::PreReadySticks()
{
	퍼포먼스타이머[0].시작();
	size_t 세팅된종목수 = 0;
	size_t 종목총갯수 = 벡터_감시할_종목코드.size();
	for (size_t 종목인덱스 = 0; 종목인덱스 < 종목총갯수; 종목인덱스++)
	{
		C_OBJECT_MONSTER* pStock = FindMonster(벡터_감시할_종목코드[종목인덱스].c_str());
		if (pStock)
		{
			퍼포먼스타이머[2].시작();
			pStock->InitSticks();
			for (size_t 캔들인덱스 = 0; 캔들인덱스 < 배열크기(nMinuteLengths); 캔들인덱스++)
			{
				LoadSticks(&pStock->분봉[캔들인덱스]);
			}
			LoadSticks(&pStock->일봉);
			LoadSticks(&pStock->주봉);
			LoadSticks(&pStock->월봉);
			세팅된종목수++;
			디뷰("C_GAME::PreReadySticks() - %s 완료 / 경과된시간: %0.6f", 벡터_감시할_종목코드[종목인덱스].c_str(), 퍼포먼스타이머[2].경과된시간());
		}
	}
	디뷰("C_GAME::PreReadySticks() - 세팅된종목수: %d, 경과된시간: %0.6f", 세팅된종목수, 퍼포먼스타이머[0].경과된시간());
}
#if 0
void C_GAME::PushTickData(주식체결변환포 _체결변환포)
{	// 이건 저장된 체결 데이터를 밀어넣고 테스트 하는거다.
	// 틱 데이터를 우선 메모리에 적재한다.
	LPTICK_DATA pTickData = (LPTICK_DATA)pTickBufferPtr;
	//pTickData->nSequence = nCountAccrueTick;	// 더하고 증가, 순서는 0부터 기록한다.
	// 코드는 복사, 7바이트 문자배열이라 제로 메모리는 굳이 안쓴다.
	strncpy_s(pTickData->szCode, 배열크기(pTickData->szCode) * sizeof(char), _체결변환포->종목코드, 6);
	pTickData->szCode[6] = 0;
	pTickData->nTransType = _체결변환포->체결구분;
	pTickData->nTime = _체결변환포->체결시간;
	pTickData->nTransVolume = _체결변환포->거래량;
	pTickData->fPrice = (float)_체결변환포->현재가;
	pTickData->최우선매도호가 = _체결변환포->최우선매도호가;
	pTickData->최우선매수호가 = _체결변환포->최우선매수호가;
	pTickBufferPtr += sizeof(TICK_DATA);

	//종목정보_확장포 종목포 = 종목찾기(_체결변환포->종목코드);
	//if (종목포)
	//{	

		// 이제 종목별 데이터를 업데이트 한다
	//	종목포->Update(_체결변환포);
	//	종목포->연산();
	//}
}
#endif

DWORD C_GAME::ThreadFunc(LPVOID _파라미터)
{
	_파라미터;
	return(0);
	/*
	디뷰("C_GAME::ThreadFunc() - %x", ::GetCurrentThreadId());
	
	LPPACKET_BASE pPipePacket = nullptr;
	do
	{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에 
		try
		{
			
			pPipePacket = pNetEngi->증권사패킷꺼내기();
			if (pPipePacket)
			{
				if (!pPipePacket->nPacketIndex)
				{
					디뷰("C_GAME::ThreadFunc() - 받음(_PKT_INDEX_NONE) 인덱스가 없음");
					//pLog->Write("파이프 받음(_PKT_INDEX_NONE) 인덱스가 없음");
				}
				else
				{
					//디뷰("받음(): %d / %d / %d", pPipePacket->nPacketIndex, pPipePacket->bytClientType, pPipePacket->nClientVersion);
					if (_PKT_PIPE_CREON_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_CREON_MAX_ > pPipePacket->nPacketIndex)
					{	// 크레온 메시지
						PktHandlerCreon(pPipePacket);
					}
					else if (_PKT_PIPE_XINGAPI_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_XINGAPI_MAX_ > pPipePacket->nPacketIndex)
					{	// 이베스트
						PktHandlerEbest(pPipePacket);
					}
					else if (_PKT_PIPE_KOREA_MIN_ < pPipePacket->nPacketIndex && _PKT_PIPE_KOREA_MAX_ > pPipePacket->nPacketIndex)
					{	// 한국투자증권
						//패킷_처리기_한국투자(pPipePacket);
					}
				}
			}
			
		}
		catch (...)
		{
			디뷰메시지("Unexpected exception");
		}
		//디뷰("처리한 패킷 할당 해제");
		DSAFE_DELETE(pPipePacket);
	} while (true);
	
	return(0);
	*/
}

#if 0
void C_GAME::시뮬레이션_캔들세팅(LPCSTR _파일경로, 동적버퍼_캔들포 _캔들포)
{
	if (1 == dk::파일체크(_파일경로))
	{	// 이미 확인 했겠지만, 혹시 모를 상황을 위해 한번 더 확인하기로 한다.
		퍼포먼스타이머[3].시작();
		dk::C_FILE 파일(_파일경로);
		LPBYTE 파일포 = (LPBYTE)파일.시작포인터();
		베트파일헤더포 헤더포 = (베트파일헤더포)파일포;
		if (0x0000E90C == 헤더포->nFileType)
		{
			ULONG 총캔들수 = (파일.파일크기() - 크기(베트파일헤더)) / 크기(확장캔들);
			ULONG 파일위치 = 크기(베트파일헤더);	// 헤더 건너띄고
			_캔들포->할당(총캔들수);				// 복사할 캔들수만큼 메모리 할당
			for (size_t 캔들인덱스 = 0; 캔들인덱스 < 총캔들수; 캔들인덱스++)
			{	// 할당된 동적 배열에 데이터를 채운다.
				확장캔들포 캔들포 = (확장캔들포)(파일포 + 파일위치);
				_캔들포->캔들배열[캔들인덱스].날짜 = ::ntohl(캔들포->nDate);
				_캔들포->캔들배열[캔들인덱스].시간 = (::ntohl(캔들포->nTime) / 10000);
				_캔들포->캔들배열[캔들인덱스].시가 = (double)캔들포->fOpen;
				_캔들포->캔들배열[캔들인덱스].고가 = (double)캔들포->fHigh;
				_캔들포->캔들배열[캔들인덱스].저가 = (double)캔들포->fLow;
				_캔들포->캔들배열[캔들인덱스].종가 = (double)캔들포->fClose;
				_캔들포->캔들배열[캔들인덱스].거래량 = 캔들포->nTotalVolume;
				_캔들포->적재된캔들수++;
				파일위치 += 크기(확장캔들);
			}
			디뷰("[%s]  읽은캔들수: (%d/%d) 경과된시간: %0.6f", _파일경로, _캔들포->적재된캔들수, 총캔들수, 퍼포먼스타이머[3].경과된시간());
		}
		파일.닫기();
	}
}

void C_GAME::시뮬레이션_체결데이터(주식체결변환포 _체결변환포)
{
	종목정보_확장포 종목포 = 종목찾기(_체결변환포->종목코드);
	if (종목포)
	{	// 여기에서 종목별 데이터를 업데이트 한다
		종목포->Update(_체결변환포);
		종목포->연산();
	}
}

void C_GAME::시뮬레이션_분봉()
{
	디뷰("C_GAME::시뮬레이션_분봉(start)");
	퍼포먼스타이머[0].시작();
	char 시간[(1 << 4)], 날짜시간[(1 << 5)];
	//size_t 카운트 = 0;
	//bool 분봉추가[11] = { false };

	for (; 일봉인덱스 < 임시캔들_일봉.적재된캔들수; 일봉인덱스++)
	{	// 남은 일봉을 돌린다.
		ULONG 진행할날짜 = 임시캔들_일봉.캔들배열[일봉인덱스].날짜;
		시뮬종목포->일봉.추가(진행할날짜, 0);
		///////////////////////////////////////////////////////////////////////////////
		// 장운영시간 세팅
		ULONG 초로변환된_장시작시간 = _HM_TO_SEC_(9, 0);
		ULONG 초로변환된_동시호가시간 = _HM_TO_SEC_(15, 20);
		ULONG 초로변환된_장마감시간 = _HM_TO_SEC_(15, 30);
		bool 찾음 = false;
		for (ULONG 초로변환된시간 = 초로변환된_장시작시간; 초로변환된시간 <= _HM_TO_SEC_(9, 55); 초로변환된시간 += 60)
		{	// 1분봉으로 10시 이전에 캔들이 있는지 확인한다.
			ULONG 현재_시 = ((초로변환된시간 / 60) / 60);
			ULONG 현재_분 = ((초로변환된시간 / 60) % 60);
			sprintf_s(날짜시간, "%08d/%02d%02d00", 진행할날짜, 현재_시, 현재_분);
			유맵_기본캔들::iterator itr = 시뮬캔들_분봉[0].find(날짜시간);
			if (시뮬캔들_분봉[0].end() == itr)
			{	// 찾으면 장시작시간은 9시인 것.
				찾음 = true;
				break;
			}
		}
		if (!찾음)
		{
			초로변환된_장시작시간 += 3600;
			초로변환된_동시호가시간 += 3600;
			초로변환된_장마감시간 += 3600;
		}
		디뷰("[%d] 장시작시간: %02d%02d / 장동시호가시간: %02d%02d / 장마감시간: %02d%02d"
			, 진행할날짜
			, ((초로변환된_장시작시간 / 60) / 60), ((초로변환된_장시작시간 / 60) % 60)
			, ((초로변환된_동시호가시간 / 60) / 60), ((초로변환된_동시호가시간 / 60) % 60)
			, ((초로변환된_장마감시간 / 60) / 60), ((초로변환된_장마감시간 / 60) % 60)
		);

		for (ULONG 초로변환된시간 = 초로변환된_장시작시간; 초로변환된시간 <= 초로변환된_장마감시간; 초로변환된시간 += 60)
		{
			ULONG 현재_시 = ((초로변환된시간 / 60) / 60);
			ULONG 현재_분 = ((초로변환된시간 / 60) % 60);
			if (!(초로변환된시간 % 60))
			{	// 1분에 한번씩 들어온다.
				sprintf_s(시간, "%02d%02d00", 현재_시, 현재_분);
				sprintf_s(날짜시간, "%08d/%02d%02d00", 진행할날짜, 현재_시, 현재_분);
				if (!(초로변환된시간 % 분봉연산용[0]) || 초로변환된시간 == 초로변환된_장마감시간)
				{	// 1분봉을 갱신할꺼고
					유맵_기본캔들::iterator itr = 시뮬캔들_분봉[0].find(날짜시간);
					if (시뮬캔들_분봉[0].end() != itr)
					{	// 먼저 1분봉을 갱신 그리고 추가한다.
						if (초로변환된시간 < 초로변환된_동시호가시간)
						{	// 15시 19분까지 갱신하고 15시 20분을 생성한다. 
							//디뷰("[%d/%d] 갱신 %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[0], itr->second.종가);
							// 기존 봉을 갱신하고 새 빈봉을 추가한다.
							시뮬종목포->분봉[0].갱신(itr->second.시간, itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
							시뮬종목포->분봉[0].연산();
							//디뷰("[%d/%04d] %d분봉 - 캔들 채움", 시뮬종목포->분봉[i].마지막날짜(), 시뮬종목포->분봉[i].마지막시간(), 분봉의길이들[i]);
							// 0901 으로 새로 생성한다.
							//디뷰("[%d/%02d%02d00] 빈봉생성 - %d분봉", 진행할날짜, 현재_시, 현재_분, 분봉의길이들[0]);
							시뮬종목포->분봉[0].추가(진행할날짜, ::atoi(시간));
						}
						else if (초로변환된시간 == 초로변환된_동시호가시간)
						{	// 15시 20분을 갱신하고 연산한다.
							//디뷰("[%d/%d] 동시호가  %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[0], itr->second.종가);
							시뮬종목포->분봉[0].갱신(itr->second.시간, itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
							시뮬종목포->분봉[0].연산();
						}
						else if (초로변환된시간 == 초로변환된_장마감시간)
						{	// 15시 30분 추가하고 연산한다.
							시뮬종목포->분봉[0].추가(itr->second.날짜, itr->second.시간, itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
							시뮬종목포->분봉[0].연산();
							//디뷰("[%d/%d] 장마감  %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[0], itr->second.종가);
						}
						for (short i = 1; i < 배열크기(분봉의길이들); i++)
						{	// 이제 다른 봉들을 1분봉의 데이터로 갱신한다.
							if (초로변환된시간 < 초로변환된_동시호가시간)
							{	// 여기서는 1분봉의 종가 갱신만 하고 추가는 하지 않는다.
								//디뷰("[%d/%d] 갱신 %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[i], itr->second.종가);
								시뮬종목포->분봉[i].갱신(itr->second.거래량, itr->second.종가, itr->second.시간);
								시뮬종목포->분봉[i].연산();
							}
							else if (초로변환된시간 == 초로변환된_동시호가시간)
							{	// 15시 20분을 갱신하고 연산한다.
								//디뷰("[%d/%d] 동시호가  %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[i], itr->second.종가);
								시뮬종목포->분봉[i].갱신(itr->second.시간, itr->second.거래량, itr->second.고가, itr->second.저가, itr->second.종가, itr->second.시가);
								시뮬종목포->분봉[i].연산();
							}
						}
						시뮬종목포->일봉.갱신(itr->second.시간, itr->second.거래량, itr->second.고가, itr->second.저가, itr->second.종가, itr->second.시가);
						시뮬종목포->일봉.연산();
						//디뷰("(%d) %0.2f %0.2f %0.2f %0.2f"
						//	, 시뮬종목포->일봉.날짜[시뮬종목포->일봉.캔들인덱스]
						//	, 시뮬종목포->일봉.시가[시뮬종목포->일봉.캔들인덱스]
						//	, 시뮬종목포->일봉.고가[시뮬종목포->일봉.캔들인덱스]
						//	, 시뮬종목포->일봉.저가[시뮬종목포->일봉.캔들인덱스]
						//	, 시뮬종목포->일봉.종가[시뮬종목포->일봉.캔들인덱스]
						//);
					}
					else if (초로변환된시간 == 초로변환된_장시작시간)
					{	// 첫 봉 생성을 위해 한번만 들어온다.
						//디뷰("[%d/%02d%02d00] 생성- %d분봉(장시작)", 진행할날짜, 현재_시, 현재_분, 분봉의길이들[0]);
						시뮬종목포->분봉[0].추가(진행할날짜, ::atoi(시간)
							, 시뮬종목포->분봉[0].거래량[시뮬종목포->분봉[0].캔들인덱스]
							, 시뮬종목포->분봉[0].시가[시뮬종목포->분봉[0].캔들인덱스]
							, 시뮬종목포->분봉[0].고가[시뮬종목포->분봉[0].캔들인덱스]
							, 시뮬종목포->분봉[0].저가[시뮬종목포->분봉[0].캔들인덱스]
							, 시뮬종목포->분봉[0].종가[시뮬종목포->분봉[0].캔들인덱스]
						);
						시뮬종목포->분봉[0].연산();
					}
				}
				for (short i = 1; i < 배열크기(분봉의길이들); i++)
				{
					if (!(초로변환된시간 % 분봉연산용[i]) || 초로변환된시간 == 초로변환된_장마감시간)
					{
						유맵_기본캔들::iterator itr = 시뮬캔들_분봉[i].find(날짜시간);
						if (시뮬캔들_분봉[i].end() != itr)
						{
							if (초로변환된시간 < 초로변환된_동시호가시간)
							{	// 15시 19분까지 갱신하고 15시 20분을 생성한다. 
								//디뷰("[%d/%d] 갱신 %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[i], itr->second.종가);
								// 기존 봉을 갱신하고 새 빈봉을 추가한다.
								시뮬종목포->분봉[i].갱신(itr->second.시간, itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
								시뮬종목포->분봉[i].연산();
								//디뷰("[%d/%04d] %d분봉 - 캔들 채움", 시뮬종목포->분봉[i].마지막날짜(), 시뮬종목포->분봉[i].마지막시간(), 분봉의길이들[i]);
								// 0901 으로 새로 생성한다.
								//디뷰("[%d/%02d%02d00] 생성 - %d분봉", 진행할날짜, 현재_시, 현재_분, 분봉의길이들[i]);
								시뮬종목포->분봉[i].추가(진행할날짜, ::atoi(시간), itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
								시뮬종목포->분봉[i].연산();
							}
							else if (초로변환된시간 == 초로변환된_동시호가시간)
							{	// 15시 20분을 갱신하고 연산한다.
								//디뷰("[%d/%d] 동시호가  %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[i], itr->second.종가);
								시뮬종목포->분봉[i].갱신(itr->second.시간, itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
								시뮬종목포->분봉[i].연산();
							}
							else if (초로변환된시간 == 초로변환된_장마감시간)
							{	// 15시 30분 추가하고 연산한다.
								시뮬종목포->분봉[i].추가(itr->second.날짜, itr->second.시간, itr->second.거래량, itr->second.시가, itr->second.고가, itr->second.저가, itr->second.종가);
								시뮬종목포->분봉[i].연산();
								//디뷰("[%d/%d] 장마감  %d분봉 %0.2f", itr->second.날짜, itr->second.시간, 분봉의길이들[i], itr->second.종가);
							}
						}
						else if (초로변환된시간 == 초로변환된_장시작시간)
						{	// 첫 봉 생성을 위해 한번만 들어온다.
							//디뷰("[%d/%02d%02d00] 생성- %d분봉(장시작)", 진행할날짜, 현재_시, 현재_분, 분봉의길이들[i]);
							시뮬종목포->분봉[i].추가(진행할날짜, ::atoi(시간)
								, 시뮬종목포->분봉[i].거래량[시뮬종목포->분봉[i].캔들인덱스]
								, 시뮬종목포->분봉[i].시가[시뮬종목포->분봉[i].캔들인덱스]
								, 시뮬종목포->분봉[i].고가[시뮬종목포->분봉[i].캔들인덱스]
								, 시뮬종목포->분봉[i].저가[시뮬종목포->분봉[i].캔들인덱스]
								, 시뮬종목포->분봉[i].종가[시뮬종목포->분봉[i].캔들인덱스]
							);
							시뮬종목포->분봉[i].연산();
						}
					}
				}
			}
		}
		//if (1 < 카운트++) { break; }
	}
	디뷰("C_GAME::시뮬레이션_분봉(end) - %0.6f", 퍼포먼스타이머[0].경과된시간());
}

void C_GAME::시뮬레이션_준비(LPCSTR _종목코드)
{
	디뷰("C_GAME::시뮬레이션_준비(start) - 호출한 스레드: %x", ::GetCurrentThreadId());

	시뮬종목포 = nullptr;
	초기화_동적버퍼();

	퍼포먼스타이머[0].시작();
	종목정보_확장포 종목포 = 종목찾기(_종목코드);
	if (!종목포)
	{
		STOCK_INFO_CREON 종목정보 = { 0 };
		strcpy_s(종목정보.종목코드, _종목코드);
		if (!strcmp(_종목코드, "352700")) { strcpy_s(종목정보.종목명, "씨앤투스성진"); }
		if (!strcmp(_종목코드, "005930")) { strcpy_s(종목정보.종목명, "삼성전자"); }
		if (!strcmp(_종목코드, "323410")) { strcpy_s(종목정보.종목명, "카카오뱅크"); }
		if (!strcmp(_종목코드, "036570")) { strcpy_s(종목정보.종목명, "엔씨소프트"); }
		if (!strcmp(_종목코드, "037440")) { strcpy_s(종목정보.종목명, "희림"); }
		종목추가(&종목정보);

		설정()->UpdateStatus(_크레온_, _브릿지_종목전송완료_);	// 여긴 시뮬레이션
		종목포 = 종목찾기(_종목코드);
	}
	if (종목포)
	{
		// 임시 메모리에 캔들 세팅.
		//디뷰("월봉에 경로세팅");
		시뮬레이션_캔들세팅(종목포->월봉.경로.c_str(), &임시캔들_월봉);
		//디뷰("주봉에 경로세팅");
		시뮬레이션_캔들세팅(종목포->주봉.경로.c_str(), &임시캔들_주봉);
		//디뷰("일봉에 경로세팅");
		시뮬레이션_캔들세팅(종목포->일봉.경로.c_str(), &임시캔들_일봉);
		//디뷰("분봉에 경로세팅");
		for (size_t 캔들인덱스 = 0; 캔들인덱스 < 배열크기(분봉의길이들); 캔들인덱스++)
		{
			시뮬레이션_캔들세팅(종목포->분봉[캔들인덱스].경로.c_str(), &임시캔들_분봉[캔들인덱스]);
		}
		//디뷰("월봉 배열에 등록");
		퍼포먼스타이머[2].시작();
		char 날짜[(1 << 4)];
		char 임시버퍼[(1 << 4)];
		sprintf_s(임시버퍼, "%04d%02d00", 월봉_년_끝, 월봉_월_끝);
		ULONG 월봉_끝 = (ULONG)::atoi(임시버퍼);
		size_t 카운트 = 0;
		for (size_t 월봉인덱스 = 0; 월봉인덱스 < 임시캔들_월봉.적재된캔들수; 월봉인덱스++)
		{	// 월봉꺼 변환
			sprintf_s(날짜, "%08d", 임시캔들_월봉.캔들배열[월봉인덱스].날짜);
			WORD 월봉_년 = (((날짜[0] - 0x30) * 1000) + ((날짜[1] - 0x30) * 100) + ((날짜[2] - 0x30) * 10) + (날짜[3] - 0x30));
			WORD 월봉_월 = (((날짜[4] - 0x30) * 10) + (날짜[5] - 0x30));
			if (월봉_년_시작 > 월봉_년 || (월봉_년_시작 == 월봉_년 && 월봉_월_시작 > 월봉_월))
			{
				continue;
			}
			if ((월봉_년_끝 == 월봉_년 && 월봉_월_끝 < 월봉_월) || 월봉_끝 < 임시캔들_월봉.캔들배열[월봉인덱스].날짜)
			{
				break;
			}
			//디뷰("(%08d/%08d) - 월봉(%04d/%02d)", 임시캔들_월봉.캔들배열[월봉인덱스].날짜, 임시캔들_월봉.캔들배열[월봉인덱스].시간, 월봉_년, 월봉_월);
			종목포->월봉.추가(
				임시캔들_월봉.캔들배열[월봉인덱스].날짜
				, 임시캔들_월봉.캔들배열[월봉인덱스].시간
				, 임시캔들_월봉.캔들배열[월봉인덱스].거래량
				, 임시캔들_월봉.캔들배열[월봉인덱스].시가
				, 임시캔들_월봉.캔들배열[월봉인덱스].고가
				, 임시캔들_월봉.캔들배열[월봉인덱스].저가
				, 임시캔들_월봉.캔들배열[월봉인덱스].종가
			);
			종목포->월봉.연산();
			카운트++;
			if ((월봉_년_끝 == 월봉_년 && 월봉_월_끝 == 월봉_월) || 월봉_끝 == 임시캔들_월봉.캔들배열[월봉인덱스].날짜)
			{
				break;
			}
		}
		디뷰("월봉 %d개 읽음 - %0.6f", 카운트, 퍼포먼스타이머[2].경과된시간());
		// 일봉으로 월봉 주봉을 만들어서 세팅하도록 하자.
		//디뷰("일봉 배열에 등록");
		퍼포먼스타이머[2].시작();
		카운트 = 0;
		sprintf_s(임시버퍼, "%04d%02d%02d", 일봉_년_끝, 일봉_월_끝, 일봉_일_끝);
		ULONG 일봉_끝 = (ULONG)::atoi(임시버퍼);
		for (; 일봉인덱스 < 임시캔들_일봉.적재된캔들수; 일봉인덱스++)
		{	// 일봉꺼 변환
			sprintf_s(날짜, "%08d", 임시캔들_일봉.캔들배열[일봉인덱스].날짜);
			WORD 일봉_년 = (((날짜[0] - 0x30) * 1000) + ((날짜[1] - 0x30) * 100) + ((날짜[2] - 0x30) * 10) + (날짜[3] - 0x30));
			WORD 일봉_월 = (((날짜[4] - 0x30) * 10) + (날짜[5] - 0x30));
			WORD 일봉_일 = (((날짜[6] - 0x30) * 10) + (날짜[7] - 0x30));
			//WORD 일봉_요일 = dk::GetDayOfWeek(일봉_년, 일봉_월, 일봉_일);
			//WORD 일봉_주 = (dk::GetWeekOfMonth(일봉_요일, 일봉_일) * 10);
			if (일봉_년_시작 > 일봉_년 || (일봉_년_시작 == 일봉_년 && 일봉_월_시작 > 일봉_월) || (일봉_년_시작 == 일봉_년 && 일봉_월_시작 == 일봉_월 && 일봉_일_시작 > 일봉_일))
			{
				continue;
			}
			if ((일봉_년_끝 == 일봉_년 && 일봉_월_끝 == 일봉_월 && 일봉_일_끝 < 일봉_일) || 일봉_끝 < 임시캔들_일봉.캔들배열[일봉인덱스].날짜)
			{
				디뷰("추가전 %d %d", (ULONG)::atoi(임시버퍼), 임시캔들_일봉.캔들배열[일봉인덱스].날짜);
				break;
			}
			//디뷰("(%08d/%08d) - 일봉(%04d/%02d/%02d) 요일: %d 주: %d", 임시캔들_일봉.캔들배열[일봉인덱스].날짜, 임시캔들_일봉.캔들배열[일봉인덱스].시간, 일봉_년, 일봉_월, 일봉_일, 일봉_요일, 일봉_주);
			종목포->일봉.추가(
				임시캔들_일봉.캔들배열[일봉인덱스].날짜
				, 임시캔들_일봉.캔들배열[일봉인덱스].시간
				, 임시캔들_일봉.캔들배열[일봉인덱스].거래량
				, 임시캔들_일봉.캔들배열[일봉인덱스].시가
				, 임시캔들_일봉.캔들배열[일봉인덱스].고가
				, 임시캔들_일봉.캔들배열[일봉인덱스].저가
				, 임시캔들_일봉.캔들배열[일봉인덱스].종가
			);
			종목포->일봉.연산();
			카운트++;
			if ((일봉_년_끝 == 일봉_년 && 일봉_월_끝 == 일봉_월 && 일봉_일_끝 == 일봉_일) || 일봉_끝 == 임시캔들_일봉.캔들배열[일봉인덱스].날짜)
			{
				디뷰("추가후 %d %d", (ULONG)::atoi(임시버퍼), 임시캔들_일봉.캔들배열[일봉인덱스].날짜);
				break;
			}
		}
		디뷰("일봉 %d개 읽음 - %0.6f", 카운트, 퍼포먼스타이머[2].경과된시간());
		일봉인덱스++;
		//디뷰("분봉 배열에 등록");
		sprintf_s(임시버퍼, "%04d%02d%02d", 분봉_년_끝, 분봉_월_끝, 분봉_일_끝);
		ULONG 분봉_끝 = (ULONG)::atoi(임시버퍼);
		for (short 분봉길이인덱스 = (배열크기(분봉의길이들) - 1); 0 <= 분봉길이인덱스; 분봉길이인덱스--)
		{	// 긴 시간대의 분봉부터 1분봉까지 차례로 돈다.
			퍼포먼스타이머[2].시작();
			카운트 = 0;
			for (; 분봉인덱스[분봉길이인덱스] < 임시캔들_분봉[분봉길이인덱스].적재된캔들수; 분봉인덱스[분봉길이인덱스]++)
			{
				sprintf_s(날짜, "%08d", 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].날짜);
				WORD 분봉_년 = (((날짜[0] - 0x30) * 1000) + ((날짜[1] - 0x30) * 100) + ((날짜[2] - 0x30) * 10) + (날짜[3] - 0x30));
				WORD 분봉_월 = (((날짜[4] - 0x30) * 10) + (날짜[5] - 0x30));
				WORD 분봉_일 = (((날짜[6] - 0x30) * 10) + (날짜[7] - 0x30));
				//char 시간[(1 << 4)];
				//sprintf_s(시간, "%06d", 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].시간);
				//ULONG 시 = ((((시간[0] - 0x30) * 10) + ((시간[1] - 0x30))) * 3600);
				//ULONG 분 = ((((시간[2] - 0x30) * 10) + ((시간[3] - 0x30))) * 60);
				if (분봉_년_시작 > 분봉_년 || (분봉_년_시작 == 분봉_년 && 분봉_월_시작 > 분봉_월) || (분봉_년_시작 == 분봉_년 && 분봉_월_시작 == 분봉_월 && 분봉_일_시작 > 분봉_일))
				{
					continue;
				}
				if ((분봉_년_끝 == 분봉_년 && 분봉_월_끝 == 분봉_월 && 분봉_일_끝 < 분봉_일) || 분봉_끝 < 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].날짜)
				{
					break;
				}
				종목포->분봉[분봉길이인덱스].추가(
					임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].날짜
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].시간
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].거래량
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].시가
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].고가
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].저가
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].종가
				);
				종목포->분봉[분봉길이인덱스].연산();
				카운트++;
				//디뷰("(%d/%d) - %d분봉"
				//	, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].날짜
				//	, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].시간
				//	, 분봉의길이들[분봉길이인덱스]
				//);
				//if ((분봉_년_끝 == 분봉_년 && 분봉_월_끝 == 분봉_월 && 분봉_일_끝 == 분봉_일) || 분봉_끝 == 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].날짜)
				//{	// 이게 있으면 당일꺼까지 준비가 안됨.
				//	break;
				//}
			}
			디뷰("%d분봉 %d개 읽음 - %0.6f", 분봉의길이들[분봉길이인덱스], 카운트, 퍼포먼스타이머[2].경과된시간());
			// 남은 분봉을 전부 유맵에 넣는다.
			for (; 분봉인덱스[분봉길이인덱스] < 임시캔들_분봉[분봉길이인덱스].적재된캔들수; 분봉인덱스[분봉길이인덱스]++)
			{
				char 날짜시간[(1 << 5)];
				sprintf_s(날짜시간, "%08d/%06d"
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].날짜
					, 임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]].시간
				);
				//디뷰("유맵에 넣음, %d분봉 - %s / %x", 분봉의길이들[분봉길이인덱스], 날짜시간, &임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]]);
				캔들_기본포 캔들포 = &시뮬캔들_분봉[분봉길이인덱스][날짜시간];
				memcpy_s(캔들포, sizeof(캔들_기본), &임시캔들_분봉[분봉길이인덱스].캔들배열[분봉인덱스[분봉길이인덱스]], sizeof(캔들_기본));
			}
			디뷰("%d분봉 %d개 유맵에 준비 완료", 분봉의길이들[분봉길이인덱스], 시뮬캔들_분봉[분봉길이인덱스].size());
		}
		시뮬종목포 = 종목포;
		디뷰("C_GAME::시뮬레이션_준비(%s) 파일 읽기 완료 - %0.6f", _종목코드, 퍼포먼스타이머[0].경과된시간());

	}
	else
	{
		디뷰("C_GAME::시뮬레이션_준비(%s) 종목이 없음 - %0.6f", _종목코드, 퍼포먼스타이머[0].경과된시간());
	}
}
#endif