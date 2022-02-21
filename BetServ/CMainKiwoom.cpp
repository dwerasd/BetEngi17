#include "framework.h"
#include "CMain.h"



///////////////////////////////////////////////////////////////////////////////
// 브릿지 패킷은 모두 여기에서 처리한다.

static C_MAIN* pMain = nullptr;
static C_ENGINE* pEngine = nullptr;
static C_BRIDGE_BASE* pBridgeKiwoom = nullptr;
LPVOID __stdcall BridgeCallbackKiwoom(ULONG_PTR _dwMessage, LPVOID _pData)
{
	try
	{
		// 여기에서 콜백으로 Main 에 있는 데이터를 바꾸던지, Game으로 넘겨서 처리하던지
		switch (_dwMessage)
		{
		case _PKT_PACKET_INDEX_NONE_:
			break;
		case _브릿지패킷_키움_클라이언트_접속_:
			if (!pMain) { pMain = C_MAIN::GetInstance(); }
			if (!pBridgeKiwoom) { pBridgeKiwoom = pMain->pBridgeKiwoom; }
			if (!pEngine) { pEngine = C_ENGINE::GetInstance(); }
			pBridgeKiwoom->SetStatus(_브릿지_로그인중_);
			do
			{	// 크레온 기다림.
				dk::멈춰();
			} while (pMain->pBridgeCreon->GetStatus() < _브릿지_종목전송완료_);
			pBridgeKiwoom->SendPipe(_브릿지패킷_키움_로그인요청_);
			break;
		case _브릿지패킷_키움_클라이언트_접속해제_:
			디뷰메시지("클라이언트 종료 받음.");
			pBridgeKiwoom->SetStatus(_브릿지_연결끊김_);	// C_BRIDGE_KIWOOM 에서는 이걸 가질 필요가 없으니 이걸 메인으로 빼야함.
			pBridgeKiwoom->Destroy();
			break;
		case _브릿지패킷_키움_로그인요청_:
			break;
		case _브릿지패킷_키움_로그인성공_:
			do
			{
				pBridgeKiwoom->SetStatus(_브릿지_연결됨_);	// C_BRIDGE_KIWOOM 에서는 이걸 가질 필요가 없으니 이걸 메인으로 빼야함.
				LPLOGIN_INFO_KIWOOM 로그인정보 = (LPLOGIN_INFO_KIWOOM)_pData;
				디뷰("키움 로그인 서버: %s", 로그인정보->모의서버 ? "모의서버" : "실서버");
				//디뷰("키움 아이디: %s", 로그인정보->아이디);
				//디뷰("키움 사용자명: %s", 로그인정보->사용자명);
				//디뷰("키움 계좌받음. 갯수: %d", 로그인정보->계좌갯수);
				pBridgeKiwoom->키움_모의서버 = 로그인정보->모의서버;
				pBridgeKiwoom->키움_아이디 = 로그인정보->아이디;
				pBridgeKiwoom->키움_사용자명 = 로그인정보->사용자명;
				for (BYTE i = 0; i < 로그인정보->계좌갯수; i++)
				{
					pBridgeKiwoom->vBankAccountNumber.push_back(로그인정보->계좌번호[i]);
					디뷰("키움 계좌번호: %s", 로그인정보->계좌번호[i]);
				}
				// 조건식을 요청한다. _브릿지패킷_키움_조건식요청_
				pBridgeKiwoom->SendPipe(_브릿지패킷_키움_조건식요청_);
			} while (false);
			break;
		case _브릿지패킷_키움_초기화실패_:
			디뷰("키움 로그인 실패 - 종료 날린다.");
			pBridgeKiwoom->SendPipe(_PKT_PIPE_DESTROY_);
			//UpdateStatus(_브릿지_연결끊김_);
			//pNetEngi->ReleasePipe(_키움_);
			break;
		case _브릿지패킷_키움_조건식요청_:
			break;
		case _브릿지패킷_키움_조건식성공_:
			do
			{	// '^' 하고 ';' 로 분류되어 이어붙여진 상태로 들어온거라 분리해야한다.
				디뷰("조건식 리스트 받음.");
				pBridgeKiwoom->키움_조건식등록((LPSTR)_pData);
				디뷰("이제 종목 리스트를 요청한다.");
				pBridgeKiwoom->SendPipe(_PKT_PIPE_KIWOOM_REQUEST_STOCK_INFO_);
			} while (false);
			break;
		case _브릿지패킷_키움_조건식실패_:
			디뷰("_브릿지패킷_키움_조건식실패_");
			break;
		case _브릿지패킷_키움_조건검색받음_:
			디뷰("_브릿지패킷_키움_조건검색받음_");
			break;
		case _PKT_PIPE_REALTIME_CONDITION_:
			break;
		case _PKT_PIPE_KIWOOM_REQUEST_STOCK_INFO_:
			break;
		case _PKT_PIPE_KIWOOM_RECEIVE_STOCK_INFO_:
			do
			{	// 종목이 날라온거다.
				pBridgeKiwoom->SetStatus(_브릿지_종목전송중_);
				pEngine->UpdateMonster((LPSTOCK_INFO_KIWOOM)_pData);
				//디비()->추가_주식종목(pStockInfo->szStockName, pStockInfo->szStockCode, _키움_);
			} while (false);
			break;
		case _PKT_PIPE_KIWOOM_SUCCEEDED_STOCK_INFO_:
			pBridgeKiwoom->SetStatus(_브릿지_종목전송완료_);
			디뷰("_PKT_PIPE_KIWOOM_SUCCEEDED_STOCK_INFO_: %d", pEngine->vReadyCode.size());
			break;
		case _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_:
			break;
		case _PKT_PIPE_KIWOOM_RECEIVE_TRANSACTION_:
			do
			{
				LPKIWOOM_REALDATA_HEADER pRealDataHeader = (LPKIWOOM_REALDATA_HEADER)_pData;
				switch (pRealDataHeader->nRealType)
				{
				case _키움_주식시세_:
					break;
				case _키움_주식체결_:	// 실시간 체결 데이터라면, 키움은 중계기에서 변환 후 보낸다.
					pEngine->PushTickData((LPKIWOOM_REALDATA_TRANSACTION)_pData);
					break;
				case _키움_주식우선호가_:
					break;
				case _키움_주식호가잔량_:
					pEngine->PushOrderBookData((키움_주식호가잔량포)_pData);
					break;
				case _키움_주식시간외호가_:
					break;
				case _키움_장시작시간_:
					do
					{
						키움_장시작시간포 장시작시간포 = (키움_장시작시간포)_pData;
						디뷰("[장시작시간] 장운영 구분: %s / 예상잔여시간?: %s"
							, 장시작시간포->장운영구분
							, 장시작시간포->장시작예상잔여시간
						);
						/*
						장시작시간 실시간은 장종료시에 15:20 부터 시작하여 분단위로 전송하다가 15:29 부터는 10초단위로 전송하게됩니다.
						그리고 16:00 와 18:00 에도 마감시간 알림으로 전송 됩니다.
						감사합니다.
						*/
					} while (false);
					break;
				}
			} while (false);
			break;
		case _PKT_PIPE_RECIVE_REALTIME_REMAIN_DATA_:
			break;
		case _브릿지패킷_키움_예수금상세현황요청_:
			break;
		case _브릿지패킷_키움_예수금상세현황받음_:
			do
			{
				키움_예수금상세현황포 현황포 = (키움_예수금상세현황포)_pData;
				pBridgeKiwoom->키움_예수금.예수금 = 현황포->예수금;
				pBridgeKiwoom->키움_예수금.출금가능금액 = 현황포->출금가능금액;
				pBridgeKiwoom->키움_예수금.주문가능금액 = 현황포->주문가능금액;

				디뷰("_브릿지패킷_키움_예수금상세현황받음_");
			} while (false);
			break;
		default:
			break;
		}
	}
	catch (...)
	{
		디뷰("%s(%d) - %s, dwMessage: %d", __FILE__, __LINE__, __FUNCSIG__, _dwMessage);
	}
	return(nullptr);
}
