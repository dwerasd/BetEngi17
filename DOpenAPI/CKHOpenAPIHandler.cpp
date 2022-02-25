#include "pch.h"
#include "CKHOpenAPICtrl.h"
#include "resource.h"
#include "DOpenAPI.h"
#include "DOpenAPIDlg.h"


/////////////////////////////////////////////////////////////////////////////
// C_KH_OPEN_API

IMPLEMENT_DYNCREATE(C_KH_OPEN_API, CWnd)

// C_KH_OPEN_API 속성입니다.

// C_KH_OPEN_API 작업입니다.
BEGIN_EVENTSINK_MAP(C_KH_OPEN_API, CWnd)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 5, C_KH_OPEN_API::OnEventConnect, VTS_I4)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 1, C_KH_OPEN_API::OnReceiveTrData, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 2, C_KH_OPEN_API::OnReceiveRealData, VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 4, C_KH_OPEN_API::OnReceiveChejanData, VTS_BSTR VTS_I4 VTS_BSTR)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 9, C_KH_OPEN_API::OnReceiveConditionVer, VTS_I4 VTS_BSTR)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 8, C_KH_OPEN_API::OnReceiveTrCondition, VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_I4)
	ON_EVENT(C_KH_OPEN_API, IDC_KHOPENAPICTRL, 7, C_KH_OPEN_API::OnReceiveRealCondition, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
END_EVENTSINK_MAP()


void C_KH_OPEN_API::SetHandler(pipe::C_PIPE_CLIENT* _pHandler)
{
	pPipe = _pHandler;
}

void C_KH_OPEN_API::SetHandler(C_ZMQ_SENDER* _pHandler)
{
	pZmqSender = _pHandler;
}

void C_KH_OPEN_API::OnEventConnect(long _nErrCode)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	try
	{
		PACKET_BASE packet =
		{
			0
			, _브릿지패킷_키움_초기화실패_
			, { 0 }
		};
		if (!_nErrCode)
		{
			// 로그인 팝업에서 이것의 바로 아래 오브젝트가 순서대로 [아이디, 비번, 인증비번, 로그인버튼]
			//HWND pWnd = ::FindWindow(NULL, "Open API Login");

			DBGPRINT("OnEventConnect() - 로그인 성공");
			LPLOGIN_INFO_KIWOOM 로그인정보포 = (LPLOGIN_INFO_KIWOOM)&packet.bytBuffer[0];
			로그인정보포->모의서버 = (bool)::atoi(KOA_Functions("GetServerGubun", "").GetBuffer());

			// 계좌 비밀번호 입력창을 출력한다.
			//KOA_Functions("ShowAccountWindow"), "");
			//LONG nLoginState = GetConnectState();
			//if (nLoginState)
			{
				//HWND pWnd = ::FindWindow(NULL, "계좌비밀번호 입력 (버전: 4.33)");
				// "" Edit
				// "등록" Button
				// "전체계좌에 등록" Button
				// "AUTO (자동로그인. 공용PC에서 사용하지 마십시요.)" Button
				// "닫기" Button
				// "" ComboBox
				// "비밀번호" Static 에서 "전체계좌에 등록"을 누르면 "등록" 으로 변경된다. 그러면 닫기.
			}
			::strcpy_s(로그인정보포->아이디, GetLoginInfo("USER_ID").GetBuffer());
			::strcpy_s(로그인정보포->사용자명, GetLoginInfo("USER_NAME").GetBuffer());

			로그인정보포->계좌갯수 = (BYTE)::atoi(GetLoginInfo("ACCOUNT_CNT").GetBuffer());
			CStringA ACCLIST = GetLoginInfo("ACCLIST");
			// CStringA 문자열의 중간 바이너리를 0으로 만들면 어떻게 동작하는지 확인할 수 없으므로 데이터를 복사함.
			size_t nLengthStr = ACCLIST.GetLength() * sizeof(char);			// 유니코드 빌드라면 길이를 2배 증가
			LPSTR pBuffer = new char[nLengthStr + sizeof(char)];			// 문자열의 끝인 0을 포함하기 위해 + sizeof(_TCHAR)
			::memcpy_s(pBuffer, nLengthStr, ACCLIST.GetBuffer(), nLengthStr);		// 결과를 pBuffer 에 복사

			BYTE 계좌카운트 = 0;
			do
			{
				char* pResult = ::strrchr(pBuffer, ';');		// 우측부터 ;를 찾는다.
				if (nullptr == pResult) { break; }						// 찾지 못하면 반복문을 중지

				pResult += 1;											// ';' 를 찾았으면 그 다음 주소로 +1
				if (0 < ::strlen(pResult))
				{
					//DBGPRINT("%i / %s", strlen(pResult), pResult);
					::strcpy_s(로그인정보포->계좌번호[계좌카운트++], pResult);
				}
				*(LPSTR)(pResult - 1) = 0;							// 문자열 앞에 ';' 이었던 바이너리를 0 으로 하면 그 곳이 문자열의 끝이 된다.
			} while (true);
			//DBGPRINT("마지막 %i / %s", strlen(pBuffer), pBuffer);
			::strcpy_s(로그인정보포->계좌번호[계좌카운트++], pBuffer);
			// 할당받은 메모리 제거
			DSAFE_DELETE_ARRAY(pBuffer);
			//for (size_t i = 0; i < 계좌카운트; i++)
			//{
			//	DBGPRINT("개자버너: %s", 로그인정보포->계좌번호[i]);
			//}
			packet.nPacketSize = sizeof(LOGIN_INFO_KIWOOM);
			packet.nPacketIndex = _브릿지패킷_키움_로그인성공_;
			if (pPipe) { pPipe->Send(&packet); }	// 성공은 헤더만 날린다.
		}
		else
		{	// 로그인 실패
			DBGPRINT("OnEventConnect() - 로그인 실패: %d", _nErrCode);
			packet.nPacketSize = sizeof(PLONG);
			packet.nPacketIndex = _브릿지패킷_키움_초기화실패_;
			*(PLONG)packet.bytBuffer = _nErrCode;		// 실패
			if (pPipe) { pPipe->Send(&packet); }
		}
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

void C_KH_OPEN_API::OnReceiveTrData(LPCSTR _sScrNum, LPCSTR sRQName, LPCSTR sTrCode, LPCSTR sRecordName, LPCSTR sPrevNext, long nDataLength, LPCSTR sErrorCode, LPCSTR sMessage, LPCSTR sSplmMsg)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	_sScrNum;
	sRQName;
	sRecordName;
	sPrevNext;
	nDataLength;
	sErrorCode;
	sMessage;
	sSplmMsg;
	try
	{
		
		if (!::strcmp(_TR_OPW00001_, sTrCode))
		{	// 예수금상세현황요청
			PACKET_BASE packet =
			{	// 메모리 할당과 복사를 한번만 하기 위해 여기에서 PACKET_BASE 를 할당한다.
				sizeof(키움_예수금상세현황)
				, _브릿지패킷_키움_예수금상세현황받음_
				, { 0 }
			};
			키움_예수금상세현황포 현황포 = (키움_예수금상세현황포)packet.bytBuffer;
			char 임시버퍼[(1 << 4)] = { 0 };
			현황포->예수금 = ::_atoi64(GetCommDataEx(임시버퍼, _countof(임시버퍼), sTrCode, "예수금상세현황", 0, "예수금"));

			::memset(임시버퍼, 0, _countof(임시버퍼));
			현황포->출금가능금액 = ::_atoi64(GetCommDataEx(임시버퍼, _countof(임시버퍼), sTrCode, "예수금상세현황", 0, "출금가능금액"));

			::memset(임시버퍼, 0, _countof(임시버퍼));
			현황포->주문가능금액 = ::_atoi64(GetCommDataEx(임시버퍼, _countof(임시버퍼), sTrCode, "예수금상세현황", 0, "주문가능금액"));
			
			if (pPipe) { pPipe->Send(&packet); }
		}
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

// 파라미터: 종목코드, "주식체결", 내용
void C_KH_OPEN_API::OnReceiveRealData(LPCSTR _sRealKey, LPCSTR _sRealType, LPCSTR _sRealData)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	_sRealData;
	//char szBuffer[(1 << 10)];
	//sprintf(szBuffer, "%s/%s/%s", _sRealKey, _sRealType, _sRealData);
	//pMainLog->Write((LPVOID)szBuffer, szBuffer);
	try
	{
		if (!::strcmp(_sRealType, "주식체결"))
		{	// 보낼 데이터를 문자열로 뽑는다
			PACKET_BASE packet =
			{	// 메모리 할당과 복사를 한번만 하기 위해 여기에서 PACKET_BASE 를 할당한다.
				sizeof(KIWOOM_REALDATA_TRANSACTION)
				, _PKT_PIPE_KIWOOM_RECEIVE_TRANSACTION_
				, { 0 }
			};
			LPKIWOOM_REALDATA_TRANSACTION pData = (LPKIWOOM_REALDATA_TRANSACTION)&packet.bytBuffer[0];
			pData->nRealType = _키움_주식체결_;
			::strcpy_s(pData->종목코드, _countof(pData->종목코드), _sRealKey);										// 종목코드

			GetCommRealDataEx(pData->체결가, _countof(pData->체결가), _sRealKey, 10);								// 체결가
			GetCommRealDataEx(pData->등락율, _countof(pData->등락율), _sRealKey, 12);								// 등락율
			GetCommRealDataEx(pData->누적거래량, _countof(pData->누적거래량), _sRealKey, 13);						// 누적거래량
			GetCommRealDataEx(pData->누적거래대금, _countof(pData->누적거래대금), _sRealKey, 14);					// 누적거래대금
			GetCommRealDataEx(pData->체결량, _countof(pData->체결량), _sRealKey, 15);								// 체결량

			GetCommRealDataEx(pData->szOpen, _countof(pData->szOpen), _sRealKey, 16);								// 당일시가
			GetCommRealDataEx(pData->szHigh, _countof(pData->szHigh), _sRealKey, 17);								// 당일고가
			GetCommRealDataEx(pData->szLow, _countof(pData->szLow), _sRealKey, 18);									// 당일저가

			GetCommRealDataEx(pData->체결시간, _countof(pData->체결시간), _sRealKey, 20);							// 체결시간
			GetCommRealDataEx(pData->최우선매도호가, _countof(pData->최우선매도호가), _sRealKey, 27);				// 최우선매도호가
			GetCommRealDataEx(pData->최우선매수호가, _countof(pData->최우선매수호가), _sRealKey, 28);				// 최우선매수호가
			GetCommRealDataEx(pData->전일거래량대비, _countof(pData->전일거래량대비), _sRealKey, 30);				// 전일거래량대비

			GetCommRealDataEx(pData->거래회전율, _countof(pData->거래회전율), _sRealKey, 31);						// 거래회전율
			GetCommRealDataEx(pData->체결강도, _countof(pData->체결강도), _sRealKey, 228);							// 체결강도
			GetCommRealDataEx(pData->시가총액, _countof(pData->시가총액), _sRealKey, 311);							// 시가총액
			GetCommRealDataEx(pData->전일동시간거래량비율, _countof(pData->전일동시간거래량비율), _sRealKey, 851);	// 전일동시간거래량비율

			pPipe->Send(&packet);
			pZmqSender->Send(&packet, sizeof(PACKET_HEADER) + sizeof(KIWOOM_REALDATA_TRANSACTION), ZMQ_DONTWAIT);
		}
		else if (!strcmp(_sRealType, "주식호가잔량"))
		{
			// [139] 매도비율
			// [121] 매수호가총잔량
			// [125] 매도호가총잔량
			PACKET_BASE packet =
			{	// 메모리 할당과 복사를 한번만 하기 위해 여기에서 PACKET_BASE 를 할당한다.
				sizeof(키움_주식호가잔량)
				, _PKT_PIPE_KIWOOM_RECEIVE_TRANSACTION_
				, { 0 }
			};
			키움_주식호가잔량포 pData = (키움_주식호가잔량포)&packet.bytBuffer[0];
			pData->nRealType = _키움_주식호가잔량_;
			::strcpy_s(pData->종목코드, _countof(pData->종목코드), _sRealKey);								// 종목코드
			
			GetCommRealDataEx(pData->호가시간, _countof(pData->호가시간), _sRealKey, 21);					// 호가 상태를 업데이트만 할꺼라서 시간은 필요없긴함
			GetCommRealDataEx(pData->매도호가총잔량, _countof(pData->매도호가총잔량), _sRealKey, 121);
			GetCommRealDataEx(pData->매수호가총잔량, _countof(pData->매수호가총잔량), _sRealKey, 125);
			GetCommRealDataEx(pData->매도비율, _countof(pData->매도비율), _sRealKey, 139);
			
			pPipe->Send(&packet);
			pZmqSender->Send(&packet, sizeof(PACKET_HEADER) + sizeof(키움_주식호가잔량), ZMQ_DONTWAIT);
		}
		/*
		else if (!strcmp(_sRealType, "주식시세"))
		{
			패킷헤더포->nRealType = _키움_주식시세_;
			//DBGPRINT("OnReceiveRealData() %s / %s / %s", _sRealKey, _sRealType, _sRealData);

		}
		else if (!strcmp(_sRealType, "주식우선호가"))
		{
			패킷헤더포->nRealType = _키움_주식우선호가_;
			//DBGPRINT("OnReceiveRealData() %s / %s / %s", _sRealKey, _sRealType, _sRealData);
		}
		else if (!strcmp(_sRealType, "주식시간외호가"))
		{
			패킷헤더포->nRealType = _키움_주식시간외호가_;
			//DBGPRINT("OnReceiveRealData() %s / %s / %s", _sRealKey, _sRealType, _sRealData);
		}
		*/
		else if (!strcmp(_sRealType, "장시작시간"))
		{
			PACKET_BASE packet =
			{	// 메모리 할당과 복사를 한번만 하기 위해 여기에서 PACKET_BASE 를 할당한다.
				sizeof(키움_장시작시간)
				, _브릿지패킷_키움_장시작시간_
				, { 0 }
			};
			키움_장시작시간포 pData = (키움_장시작시간포)&packet.bytBuffer[0];
			pData->nRealType = _키움_장시작시간_;

			//	0 : 장시작전
			//	2 : 장마감전 동시호가
			//	3 : 장시작
			//	4 : 장종료 예상지수종료
			//	8 : 장마감
			//	9 : 장종료 - 시간외종료
			//	a : 시간외 종가매매 시작
			//	b : 시간외 종가매매 종료
			//	c : 시간외 단일가 매매시작
			//	d : 시간외 단일가 매매종료
			//	s : 선옵 장마감전 동시호가 시작
			//	e : 선옵 장마감전 동시호가 종료

			// 구분, 장운영구분(0:장시작전, 2:장종료전 동시호가, 3:장시작, 4: 장종료 예상지수 종료, 8:장종료, 9:장마감)
			::strcpy_s(pData->장운영구분, sizeof(pData->장운영구분), GetCommRealData(_sRealKey, 215).GetBuffer());
			// 남은시간, 장시작 예상잔여시간
			::strcpy_s(pData->장시작예상잔여시간, sizeof(pData->장시작예상잔여시간), GetCommRealData(_sRealKey, 214).GetBuffer());

			packet.nPacketSize = sizeof(키움_장시작시간);
			pPipe->Send(&packet);
			//pZmqSender->Send(&packet, sizeof(PACKET_HEADER) + sizeof(키움_장시작시간포), ZMQ_DONTWAIT);
			//DBGPRINT("장시작시간 도착 %s / %s", strType, strRemainTime);
		}
		/*
		else
		{
			DBGPRINT("[%s] %s >> %s"
				, _sRealKey
				, _sRealType
				, _sRealData
			);
		}
		*/
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

void C_KH_OPEN_API::OnReceiveChejanData(	// 이건 내 주문의 체결에 대한 내용인듯 하다.
	LPCSTR sGubun
	, long nItemCnt
	, LPCSTR sFIdList
)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	try
	{
		DBGPRINT("C_KH_OPEN_API::OnReceiveChejanData() - %d / %s / %s", nItemCnt, sGubun, sFIdList);
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

// 키움 사용자 조건검색식 요청해서 받은거다.
void C_KH_OPEN_API::OnReceiveConditionVer(long _lRet, LPCSTR _sMsg)	// 조건식을 문자열로 받음.
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	//DBGPRINT("C_KH_OPEN_API::OnReceiveConditionVer");
	try
	{	// 받은 그대로 
		PACKET_BASE packet =
		{
			0
			, _브릿지패킷_키움_조건식성공_
			, { 0 }
		};
		if (1 != _lRet)
		{	// 사용자 조건검색식 리스트를 얻어오지 못한 경우
			char szBuffer[(1 << 10)] = { 0 };
			::sprintf_s(szBuffer, "ErrorCode: %d / Message: %s", _lRet, _sMsg);

			packet.nPacketIndex = _브릿지패킷_키움_조건식실패_;
			packet.nPacketSize = (WORD)(strlen(szBuffer) * sizeof(char));
			// 버퍼에 복사하고
			::memcpy_s(packet.bytBuffer, _countof(packet.bytBuffer), (LPVOID)szBuffer, packet.nPacketSize);	// 에러 메시지를 큐에 넣는다.
		}
		else
		{	// 조건식 리스트를 잘 얻어온 경우
			CStringA strResult = GetConditionNameList();
			packet.nPacketIndex = _브릿지패킷_키움_조건식성공_;
			packet.nPacketSize = (WORD)(strResult.GetLength() * sizeof(char));
			// 그대로 복사해서 날린다.
			::memcpy_s(packet.bytBuffer, _countof(packet.bytBuffer), (LPVOID)strResult.GetBuffer(), packet.nPacketSize);
		}
		if (pPipe) { pPipe->Send(&packet); }
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

// SendCondition 조건검색의 결과 첫 콜백인듯.
void C_KH_OPEN_API::OnReceiveTrCondition(
	LPCSTR _화면번호			// 화면번호
	, LPCSTR strCodeList		// 종목리스트(“;”로 구분), 조회된 종목리스트(ex:039490;005930;036570;…;)
	, LPCSTR strConditionName	// 조회된 조건명
	, long nIndex				// 조회된 조건명 인덱스
	, long nNext				// 연속조회(2:연속조회, 0:연속조회없음), nNext 가 2로 들어오면 종목이 더 있는것
)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	try
	{
		PACKET_BASE packet =
		{
			0
			, _브릿지패킷_키움_조건검색받음_
			, { 0 }
		};
		packet.nPacketIndex = _브릿지패킷_키움_조건검색받음_;	// 조건검색 데이터 받음
		packet.nPacketSize = sizeof(RECIVE_TR_CONDITION_DATA);

		LPRECIVE_TR_CONDITION_DATA pData = (LPRECIVE_TR_CONDITION_DATA)&packet.bytBuffer[0];

		pData->nConditionIndex = nIndex;
		pData->nCountNextData = nNext;

		::strcpy_s(pData->szScreenNumber, sizeof(pData->szScreenNumber), _화면번호);
		::strcpy_s(pData->szConditionName, sizeof(pData->szConditionName), strConditionName);
		::strcpy_s(pData->szCodeList, sizeof(pData->szCodeList), strCodeList);

		if (pPipe) { pPipe->Send(&packet); }
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
}

void C_KH_OPEN_API::OnReceiveRealCondition(
	LPCSTR sTrCode
	, LPCSTR strType
	, LPCSTR strConditionName
	, LPCSTR strConditionIndex
)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	try
	{
		PACKET_BASE packet =
		{
			0
			, _브릿지패킷_키움_조건검색받음_
			, { 0 }
		};

		packet.nPacketIndex = _NET_MSG_KIWOOM_RECIVE_REALTIME_CONDITION_;	// 실시간 조건검색 데이터 받음
		packet.nPacketSize = sizeof(RECIVE_REALTIME_CONDITION_DATA);

		LPRECIVE_REALTIME_CONDITION_DATA pData = (LPRECIVE_REALTIME_CONDITION_DATA)&packet.bytBuffer[0];

		//DBGPRINT("OnReceiveRealCondition() %s / %s / %s / %s", sTrCode, strType, strConditionName, strConditionIndex);

		pData->bInput = ('I' == strType[0]);		// 편입 "I", 이탈 "D", 0x49, 0x44
		pData->nConditionIndex = ::_ttoi(strConditionIndex);
		::strcpy_s(pData->szStockCode, sizeof(pData->szStockCode), sTrCode);
		::strcpy_s(pData->szConditionName, sizeof(pData->szConditionName), strConditionName);

		if (pPipe) { pPipe->Send(&packet); }
	}
	catch (...)
	{	// 큐에 넣지 못했는데 메모리가 터졌다면 
		디뷰메시지("Unexpected exception");
	}
}
