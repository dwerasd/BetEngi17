#include "pch.h"
#include "framework.h"
#include "DOpenAPI.h"
#include "DOpenAPIDlg.h"



void COpenAPI::PushReceivePacket(LPPACKET_BASE _pData)
{	// 여기로 온전한 패킷 한개가 들어온다.
	받은패킷큐.enqueue(_pData);
}

DWORD COpenAPI::ThreadFunc(LPVOID _pParam)
{
	_pParam;

	if (!pNetClient)
	{
		pNetClient = new net::C_NET_CLIENT("127.0.0.1");
		pNetClient->ThreadStart();
	}
	if (!키움OCX) { 키움OCX = new C_KH_OPEN_API(); }

	pPipe = new pipe::C_PIPE_CLIENT(L"PipeClientKiwoom", L"PipeServerKiwoom");
	if (!pPipe)
	{
		디뷰메시지("파이프 접속 실패");
		return(0);
	}
	pPipe->ThreadStart();
	키움OCX->SetHandler(pPipe);
	키움OCX->SetNetHandler(pNetClient);

	LPPACKET_BASE pPipePacket = nullptr;
	do
	{	// 받은 패킷을 분류하는 스레드는 1개다. 여기에서 받아서 처리 후에
		try
		{
			pPipePacket = nullptr;
			받은패킷큐.wait_dequeue(pPipePacket);
			if (pPipePacket)
			{
				switch (pPipePacket->nPacketIndex)
				{
				case _PKT_STOCK_PACKET_INDEX_NONE_:
					DBGPRINT("받음(_PKT_INDEX_NONE): %d", pPipePacket->nPacketIndex);
					break;
				case _PKT_PIPE_DESTROY_:
					((COpenAPIDlg*)(m_pMainWnd))->Destroy();
					break;
				case _PKT_PIPE_CONNECTED_:
					DBGPRINT("받음(_PKT_PIPE_CONNECTED): %s", pPipePacket->bytBuffer);
					pPipe->Send(_브릿지패킷_키움_클라이언트_접속_);
					break;
				case _브릿지패킷_키움_로그인요청_:	// 로그인에 성공하면 OnEventConnect 함수로 콜백이 온다.
					if (!키움OCX->CommConnect())				// 키움 필수
					{	// 로그인을 한다.
						DBGPRINT("키움 로그인 팝업 호출 성공");
					}
					else
					{
						DBGPRINT("실패: 키움 로그인 팝업");
						AfxMessageBox("로그인창 호출 실패");
						pPipe->Send(_브릿지패킷_키움_초기화실패_);
					}
					break;
				case _브릿지패킷_키움_조건식요청_:
					// 조건검색을 사용하려면 이 함수를 최소 한번은 호출해야함.
					// 조건식을 System 폴더에 아이디_NewSaveIndex.dat 파일로 저장한다. OCX 가 종료되면 삭제된다.
					// 영웅문에서 사용자 조건을 수정 및 추가하였을 경우에도 최신의 사용자 조건을 받고 싶으면 다시 조회해야한다.
					키움OCX->GetConditionLoad();	// 사용자 조건식을 파일로 임시 저장.
					break;
				case _PKT_PIPE_KIWOOM_REQUEST_STOCK_INFO_:
					// 종목 리스트 요청이다.
					do
					{	// 종목 리스트 요청이 온거임.
						// [0]:장내, [3]: ELW, [4]: 뮤추얼펀드, [5]: 신주인수권, [6]: 리츠,	[8]: ETF, [9]: 하이일드펀드, [10]: 코스닥, [30]: K - OTC, [50]: 코넥스(KONEX)
						CStringA strStockCode0 = 키움OCX->GetCodeListByMarket("0") + 키움OCX->GetCodeListByMarket("10");
						//CStringA strStockCode0 = GetCodeListByMarket("3");
						// CStringA 문자열의 중간 바이너리를 0으로 만들면 어떻게 동작하는지 확인할 수 없으므로 데이터를 복사함.
						size_t nLengthStr = strStockCode0.GetLength() * sizeof(char);	// 유니코드 빌드라면 길이를 2배 증가
						//char* pBuffer = new char[nLengthStr + sizeof(char)];		// 문자열의 끝인 0을 포함하기 위해 + sizeof(char)
						LPSTR pBuffer = (LPSTR)힙할당(nLengthStr + sizeof(char));		// 문자열의 끝인 0을 포함하기 위해 + sizeof(char) - 힙할당으로 하면 0x1000 에 맞춰서 한 페이지가 할당됨.
						::memcpy_s(pBuffer, nLengthStr, strStockCode0, nLengthStr);		// 결과를 pBuffer 에 복사

						std::list<std::string> listStockCodes;
						do
						{
							LPSTR pResult = ::strrchr(pBuffer, ';');		// 우측부터 ;를 찾는다.
							if (!pResult) { break; }						// 찾지 못하면 반복문을 중지
							pResult += 1;									// ';' 를 찾았으면 그 다음 주소로 +1
							if (0 < ::strlen(pResult))
							{
								//DBGPRINT("%i / %s", strlen(pResult), pResult);
								listStockCodes.push_back(pResult);
							}
							*(LPSTR)(pResult - 1) = 0;							// 문자열 앞에 ';' 이었던 바이너리를 0 으로 하면 그 곳이 문자열의 끝이 된다.
						} while (true);
						//DBGPRINT("마지막 %i / %s", strlen(pBuffer), pBuffer);
						listStockCodes.push_back(pBuffer);
						// 오름차순 정렬 000020 ~  
						listStockCodes.sort();

						DBGPRINT("전체 종목 수: %i", listStockCodes.size());
						//for (auto& itr : listStockCodes)
						//{
						//	DBGPRINT("[%s] %s", itr.c_str(), GetMasterCodeName(itr.c_str()).GetBuffer());
						//}
						CStringA strStockCodeELW = 키움OCX->GetCodeListByMarket("3");	// ELW 리스트
						CStringA strExclude1 = 키움OCX->GetCodeListByMarket("4");		// 뮤추얼펀드 리스트
						CStringA strExclude2 = 키움OCX->GetCodeListByMarket("5");		// 신주인수권 리스트
						CStringA strExclude3 = 키움OCX->GetCodeListByMarket("6");		// 리츠 리스트
						CStringA strExclude4 = 키움OCX->GetCodeListByMarket("8");		// ETF 리스트
						CStringA strExclude5 = 키움OCX->GetCodeListByMarket("9");		// 하이일드펀드 리스트
						CStringA strExclude6 = 키움OCX->GetCodeListByMarket("30");		// K-OTC 리스트
						CStringA strExclude7 = 키움OCX->GetCodeListByMarket("50");		// 코넥스(KONEX) 리스트

						//std::list<std::string> listCleanStocks;
						for (auto& itr : listStockCodes)
						{
							PACKET_BASE netPacket =
							{
								sizeof(STOCK_INFO_KIWOOM)				// 데이터크기
								, _PKT_PIPE_KIWOOM_RECEIVE_STOCK_INFO_	// 헤더
								, { 0 }									// 보낼 내용
							};
							LPSTOCK_INFO_KIWOOM pStockInfo = (LPSTOCK_INFO_KIWOOM)&netPacket.bytBuffer[0];
							//pStockInfo->clear();

							::strcpy_s(pStockInfo->szStockCode, itr.c_str());
							::strcpy_s(pStockInfo->szStockName, 키움OCX->GetMasterCodeName(itr.c_str()).GetBuffer());

							//DBGPRINT("pStockInfo: %s / %s", pStockInfo->종목코드, pStockInfo->종목명);
							char szState[(1 << 8)] = { 0 }, szGamri[(1 << 8)] = { 0 };
							::strcpy_s(szState, 키움OCX->GetMasterStockState(itr.c_str()).GetBuffer());
							::strcpy_s(szGamri, 키움OCX->GetMasterConstruction(itr.c_str()).GetBuffer());

							if (::strstr(szState, "거래정지")) { pStockInfo->거래정지 = true; }
							if (::strstr(szState, "관리종목")) { pStockInfo->관리종목 = true; }
							if (!pStockInfo->거래정지 && !pStockInfo->관리종목)
							{	// 거래정지랑 관리종목이 아니라면 증거금% 하고, 담보대출, 신용가능, 감리구분을 세팅 해준다.
								if (::strstr(szState, "담보대출")) { pStockInfo->담보대출 = true; }
								if (::strstr(szState, "신용가능")) { pStockInfo->신용가능 = true; }
								pStockInfo->증거금 = (WORD)_ttoi(&szState[6]);
								// "정상"이 가장 많으니까 잦은 비교를 줄이기 위해 "정상"도 굳이 써준다.
								if (!::strcmp(szGamri, "정상")) { pStockInfo->감리구분 = 0; }
								else if (!::strcmp(szGamri, "투자주의환기종목")) { pStockInfo->감리구분 = 4; }
								else if (!::strcmp(szGamri, "투자위험")) { pStockInfo->감리구분 = 3; }
								else if (!::strcmp(szGamri, "투자경고")) { pStockInfo->감리구분 = 2; }
								else if (!::strcmp(szGamri, "투자주의")) { pStockInfo->감리구분 = 1; }
							}
							pStockInfo->nPreClose = ::atoi(키움OCX->GetMasterLastPrice(itr.c_str()));				// 전일종가
							if (-1 != strStockCodeELW.Find(itr.c_str())) { pStockInfo->bELW = true; }	// 뮤추얼펀드
							if (-1 != strExclude1.Find(itr.c_str())) { pStockInfo->뮤추얼펀드 = true; }	// 뮤추얼펀드
							if (-1 != strExclude2.Find(itr.c_str())) { pStockInfo->신주인수권 = true; }		// 신주인수권
							if (-1 != strExclude3.Find(itr.c_str())) { pStockInfo->리츠 = true; }		// 리츠
							if (-1 != strExclude4.Find(itr.c_str())) { pStockInfo->bETF = true; }		// ETF
							if (-1 != strExclude5.Find(itr.c_str())) { pStockInfo->하이일드펀드 = true; }		// ETF
							if (-1 != strExclude6.Find(itr.c_str())) { pStockInfo->K_OTC = true; }		// ETF
							if (-1 != strExclude7.Find(itr.c_str())) { pStockInfo->코넥스 = true; }		// ETF

							//DBGPRINT("sizeof(PACKET_BASE): %d, sizeof(PipePacket.bytBuffer): %d", sizeof(PACKET_BASE), netPacket.nPacketSize);

							pPipe->Send(&netPacket);
							//if (pNetClient->IsConnect())
							//{
							//	pNetClient->Send(_PKT_NET_RECEIVE_STOCKINFO_KIWOOM_, (LPBYTE)netPacket.bytBuffer, sizeof(STOCK_INFO_KIWOOM));
							//}
						}
						pPipe->Send(_PKT_PIPE_KIWOOM_SUCCEEDED_STOCK_INFO_);
						listStockCodes.clear();
						// 할당받은 메모리 제거
						//DSAFE_DELETE_ARRAY(pBuffer);
						힙해제(pBuffer);
					} while (false);
					break;
				case _PKT_PIPE_KIWOOM_REQUEST_TRANSACTION_:
					do
					{	// 실시간 체결 데이터 요청이다.
						LPREQUEST_REALTIME_DATA pRequestRealtimeData = (LPREQUEST_REALTIME_DATA)pPipePacket->bytBuffer;

						키움OCX->SetRealReg(
							pRequestRealtimeData->szScr
							, pRequestRealtimeData->szStocks
							// [10]: 현재가, [12]: 등락율, [13]: 누적거래량, [14]: 누적거래대금, [15]: 거래량, [16]: 시가, [17]: 고가, [18]: 저가,
							// [20]: 체결시간, [27]: 최우선매도호가, [28]: 최우선매수호가, [30]: 전일거래량대비(비율)
							// [41]: 매도호가1, [61]: 매도호가수량1, [71]: 매수호가수량1, [81]:매도호가직전대비1
							// [214]: 장시작예상잔여시간, [215]: 장운영구분, [228]: 체결강도
							// [1221]: VI 발동가격, [9203]: 주문번호, [397]: 파생상품거래단위, [990]: 당일실현손익(유가)
							, "10;12;13;14;15;16;17;18;20;27;28;30;41;61;121;125;228;139;311;851;131;141;214;1221;9203;397;990;"
							, "0"
						);
						DBGPRINT("SetRealReg(%s): %s", pRequestRealtimeData->szScr, pRequestRealtimeData->szStocks);
					} while (false);
					break;
				case _브릿지패킷_키움_예수금상세현황요청_:	// 예수금상세현황요청.
					do
					{
						LPREQUEST_TR_OPW00001 요청포 = (LPREQUEST_TR_OPW00001)pPipePacket->bytBuffer;
						DBGPRINT("_브릿지패킷_키움_예수금상세현황요청_() - 요청포->계좌번호: %s, 요청포->조회구분: %s", 요청포->계좌번호, 요청포->조회구분);
						키움OCX->SetInputValue("계좌번호", 요청포->계좌번호);		// 계좌번호 = 전문 조회할 보유계좌번호
						//키움OCX->SetInputValue("비밀번호", "");					// 비밀번호 = 사용안함(공백)
						//키움OCX->SetInputValue("비밀번호입력매체구분", "00");		// 비밀번호입력매체구분 = 00
						키움OCX->SetInputValue("조회구분", 요청포->조회구분);		// "1": 추정조회, "2": 일반조회
						long nResult = 키움OCX->CommRqData("예수금상세현황요청", _TR_OPW00001_, 0, 요청포->szScrNum);
						if (nResult)
						{
							DBGPRINT("주식기본정보요청 에러 [%s][%d]", _TR_OPW00001_, nResult);
						};
					} while (false);

					break;
				default:
					DBGPRINT("받음(default): %d", pPipePacket->nPacketIndex);
					break;
				}
			}
		}
		catch (...)
		{
			디뷰메시지("Unexpected exception");
			((COpenAPIDlg*)(m_pMainWnd))->Destroy();
		}
		//DBGPRINT("처리한 패킷 할당 해제");
		DSAFE_DELETE(pPipePacket);
	} while (true);
}
