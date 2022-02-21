#include "framework.h"
#include "CMain.h"



///////////////////////////////////////////////////////////////////////////////
// 브릿지 패킷은 모두 여기에서 처리한다.
enum __MIDL___MIDL_itf_CpUtil_0001_0070_0001
{
	CPC_MARKET_NULL = 0,
	CPC_MARKET_KOSPI = 1,
	CPC_MARKET_KOSDAQ = 2,
	CPC_MARKET_FREEBOARD = 3,
	CPC_MARKET_KRX = 4,
	CPC_MARKET_KONEX = 5
};

enum __MIDL___MIDL_itf_CpUtil_0001_0070_0007
{
	CPC_KSE_SECTION_KIND_NULL = 0,
	CPC_KSE_SECTION_KIND_ST = 1,
	CPC_KSE_SECTION_KIND_MF = 2,
	CPC_KSE_SECTION_KIND_RT = 3,
	CPC_KSE_SECTION_KIND_SC = 4,
	CPC_KSE_SECTION_KIND_IF = 5,
	CPC_KSE_SECTION_KIND_DR = 6,
	CPC_KSE_SECTION_KIND_SW = 7,
	CPC_KSE_SECTION_KIND_SR = 8,
	CPC_KSE_SECTION_KIND_ELW = 9,
	CPC_KSE_SECTION_KIND_ETF = 10,
	CPC_KSE_SECTION_KIND_BC = 11,
	CPC_KSE_SECTION_KIND_FETF = 12,
	CPC_KSE_SECTION_KIND_FOREIGN = 13,
	CPC_KSE_SECTION_KIND_FU = 14,
	CPC_KSE_SECTION_KIND_OP = 15,
	CPC_KSE_SECTION_KIND_KN = 16,
	CPC_KSE_SECTION_KIND_ETN = 17
};
static C_MAIN* pMain = nullptr;
static C_ENGINE* pEngine = nullptr;
static C_BRIDGE_BASE* pBridgeCreon = nullptr;
LPVOID __stdcall BridgeCallbackCreon(ULONG_PTR _dwMessage, LPVOID _pData)
{
	try
	{
		switch (_dwMessage)
		{
		case _PKT_PIPE_CONNECTED_CREON_:	// 장 시작시간 마감시간과 종목리스트를 요청한다.
			if (!pMain) { pMain = C_MAIN::GetInstance(); }
			if (!pBridgeCreon) { pBridgeCreon = pMain->pBridgeCreon; }
			if (!pEngine) { pEngine = C_ENGINE::GetInstance(); }
			디뷰("_PKT_PIPE_CONNECTED_CREON_ %x", pBridgeCreon);
			pBridgeCreon->SetStatus(_브릿지_로그인중_);
			pBridgeCreon->SendPipe(_PKT_BRIDGE_REQUEST_INIT_CREON_);
			break;
		case _PKT_PIPE_DISCONNECTED_CREON_:
			디뷰메시지("클라이언트 종료 받음.");
			pBridgeCreon->SetStatus(_브릿지_연결끊김_);
			pBridgeCreon->Destroy();
			break;
		case _PKT_BRIDGE_SUCCESSED_INIT_CREON_:			// 장 시간이 온거
			설정()->장시작시간 = *(LPDWORD)_pData;
			설정()->장마감시간 = *(LPDWORD)(((LPDWORD)_pData) + sizeof(DWORD));
			디뷰("BridgeCallbackCreon() - 시작시간: %d(32400 = 09:00), 마감시간: %d(55800 = 15:30)", 설정()->장시작시간, 설정()->장마감시간);
			디뷰("BridgeCallbackCreon() - 시작시간: %d, 마감시간: %d", _HM_TO_SEC_(9, 00), _HM_TO_SEC_(15, 30));
			디뷰("장 시작시간: %d, 마감시간: %d", 설정()->장시작시간, 설정()->장마감시간);
			pBridgeCreon->SetStatus(_브릿지_연결됨_);
			pBridgeCreon->SendPipe(_PKT_BRIDGE_REQUEST_STOCK_INFO_CREON_);
			break;
		case _PKT_BRIDGE_RECEIVE_STOCK_INFO_CREON_:
			pBridgeCreon->SetStatus(_브릿지_종목전송중_);
			do
			{
				LPSTOCK_INFO_CREON pStockInfo = (LPSTOCK_INFO_CREON)_pData;
				bool 문제없음 = 디비()->체크_주식종목(pStockInfo);
				if (문제없음)
				{	// 디비에 넣고 블랙리스트가 아닌 종목을 받는다.
					if ((CPC_MARKET_KOSPI == pStockInfo->장구분 || CPC_MARKET_KOSDAQ == pStockInfo->장구분))	// [1]: 코스피, [2]: 코스닥, [3]: K-OTC, [4]: KRX, [5]: KONEX
					{
						do
						{
							if (::strstr(pStockInfo->종목명, "KODEX") || ::strstr(pStockInfo->종목명, "TIGER"))
							{	// 코덱스와 티거는 우선 추가한다
								pEngine->InitMonster(pStockInfo);
								//종목추가(pStockInfo);
								break;
							}
							if (CPC_KSE_SECTION_KIND_ST != pStockInfo->nKseSectionKind)
							{
								break;
							}
							if (0 == pStockInfo->상태					// 거래중지나 거래중단을 거르고
								//&& 0 == pStockInfo->nSupervisionType	// 관리종목 거르고
								//&& 0 == pStockInfo->nControlType		// 정상 외 거르고
								&& 0 == pStockInfo->스팩주				// 스팩 거르고
								&& 0 < pStockInfo->전일종가				// 전일종가 없는건 아예 없는 STOCK_INFO.
								)	// 거래할 종목만 메모리에 적재한다.
							{	// 현물만

								//if (!strcmp(&pStockInfo->종목명[::strlen(pStockInfo->종목명) - 2], "우"))
								//{
								//	//디뷰("우 거름 체크해보자: [%s] %s", pStockInfo->종목코드, pStockInfo->종목명);
								//	break;
								//}
								if (::strstr(pStockInfo->종목명, "우B")
									|| ::strstr(pStockInfo->종목명, "3우C")
									|| ::strstr(pStockInfo->종목명, "(전환)")
									|| ::strstr(pStockInfo->종목명, "G3우")
									|| ::strstr(pStockInfo->종목명, "1우")
									|| ::strstr(pStockInfo->종목명, "홀딩스")
									// 지수 관련주 필터
									//|| ::strstr(pStockInfo->종목명, "KOSEF")
									//|| ::strstr(pStockInfo->종목명, "KBSTAR")
									//|| ::strstr(pStockInfo->종목명, "KINDEX")
									//|| ::strstr(pStockInfo->종목명, "TREX")
									// 지수 관련주 필터2
									//|| ::strstr(pStockInfo->종목명, "ARIRANG")
									//|| ::strstr(pStockInfo->종목명, "SMART")
									//|| ::strstr(pStockInfo->종목명, "FOCUS")
									//|| ::strstr(pStockInfo->종목명, "HANARO")
									//|| ::strstr(pStockInfo->종목명, "TIMEFOLIO")
									//|| ::strstr(pStockInfo->종목명, "네비게이터")
									)
								{
									break;
								}
								//if (10 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
								//if (12 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
								//if (14 < pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN

								if (17 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
								//종목추가(pStockInfo);
								pEngine->InitMonster(pStockInfo);
							}
						} while (false);
					}
				}
				/*
				if ((CPC_MARKET_KOSPI == pStockInfo->장구분 || CPC_MARKET_KOSDAQ == pStockInfo->장구분))	// [1]: 코스피, [2]: 코스닥, [3]: K-OTC, [4]: KRX, [5]: KONEX
				{
					do
					{
						if (::strstr(pStockInfo->종목명, "KODEX") || ::strstr(pStockInfo->종목명, "TIGER"))
						{	// 위 리스트는 먼저 추가하고 빠져나간다.
							//디뷰("멤디비추가: [%s] %s", pStockInfo->종목코드, pStockInfo->종목명);
							pEngine->종목추가_크레온(pStockInfo->장구분, &pStockInfo->크레온코드[1], pStockInfo->크레온코드, pStockInfo->풀코드, pStockInfo->종목명, pStockInfo->전일종가);
							break;
						}
						if (CPC_KSE_SECTION_KIND_ST != pStockInfo->nKseSectionKind)
						{
							break;
						}
						if (0 == pStockInfo->상태					// 거래중지나 거래중단을 거르고
							&& 0 == pStockInfo->nSupervisionType	// 관리종목 거르고
							&& 0 == pStockInfo->nControlType		// 정상 외 거르고
							&& 0 == pStockInfo->스팩주				// 스팩 거르고
							&& 0 < pStockInfo->전일종가				// 전일종가 없는건 아예 없는 STOCK_INFO.
							)	// 거래할 종목만 메모리에 적재한다.
						{	// 현물만
							if (!strcmp(&pStockInfo->종목명[::strlen(pStockInfo->종목명) - 2], "우"))
							{
								//디뷰("우 거름 체크해보자: [%s] %s", pStockInfo->종목코드, pStockInfo->종목명);
								break;
							}
							if (::strstr(pStockInfo->종목명, "우B")
								|| ::strstr(pStockInfo->종목명, "3우C")
								|| ::strstr(pStockInfo->종목명, "(전환)")
								//|| ::strstr(pStockInfo->종목명, "G3우")
								//|| ::strstr(pStockInfo->종목명, "1우")
								|| ::strstr(pStockInfo->종목명, "홀딩스")
								// 지수 관련주 필터
								//|| ::strstr(pStockInfo->종목명, "TIGER")
								|| ::strstr(pStockInfo->종목명, "KOSEF")
								|| ::strstr(pStockInfo->종목명, "KBSTAR")
								//|| ::strstr(pStockInfo->종목명, "KODEX")
								|| ::strstr(pStockInfo->종목명, "KINDEX")
								|| ::strstr(pStockInfo->종목명, "TREX")
								// 지수 관련주 필터2
								|| ::strstr(pStockInfo->종목명, "ARIRANG")
								|| ::strstr(pStockInfo->종목명, "SMART")
								|| ::strstr(pStockInfo->종목명, "FOCUS")
								|| ::strstr(pStockInfo->종목명, "HANARO")
								|| ::strstr(pStockInfo->종목명, "TIMEFOLIO")
								|| ::strstr(pStockInfo->종목명, "네비게이터")
								)
							{
								break;
							}

							//if (9 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
							if (10 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
							if (12 == pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
							if (14 < pStockInfo->nKseSectionKind) { break; }			// [9]: ELW, [10]: ETF, [11]: 수익증권, [12]: 해외ETF, [13]: 외국주권, [14]: 선물, [15]: 옵션, [16]: KONEX, [17]: ETN
							//if (0 == pStockInfo->전일종가) { break; }					// 전일종가 없는건 아예 없는 STOCK_INFO. ( 사라진 ELW 등 )
							//디뷰("종목코드: [%s] 크레온코드: [%s]", &pStockInfo->종목코드[1], pStockInfo->종목코드);
							//if (!strcmp(&pStockInfo->종목코드[1], "323410"))
							//{
							//	디뷰("종목받음: %d %s %s %s %s %d", pStockInfo->장구분, &pStockInfo->종목코드[1], pStockInfo->종목코드, pStockInfo->풀코드, pStockInfo->종목명, pStockInfo->전일종가);
							//}
							pEngine->종목추가_크레온(pStockInfo->장구분, &pStockInfo->크레온코드[1], pStockInfo->크레온코드, pStockInfo->풀코드, pStockInfo->종목명, pStockInfo->전일종가);
						}
					} while (false);
				}
				*/
			} while (false);
			break;
		case _PKT_BRIDGE_SUCCEEDED_STOCK_INFO_CREON_:
			pBridgeCreon->SetStatus(_브릿지_종목전송완료_);
			//pEngine->PreReadySticks();
			디뷰("BridgeCallbackCreon(_PKT_BRIDGE_SUCCEEDED_STOCK_INFO_CREON_) - 다 받음: %d", pEngine->vReadyCode.size());
			break;
		case _PKT_PIPE_INFO_STOCK_OHLCV_:
			디뷰("BridgeCallbackCreon(_PKT_PIPE_INFO_STOCK_OHLCV_)");
			break;
		default:
			디뷰("BridgeCallbackCreon(default)");
			break;
		}
	}
	catch (...)
	{
		디뷰("%s(%d) - %s, dwMessage: %d", __FILE__, __LINE__, __FUNCSIG__, _dwMessage);
	}
	return(nullptr);
}
