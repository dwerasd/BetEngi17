#include "framework.h"
#include "CXingAPI.h"
#include "CMain.h"


extern C_MAIN* pMain;
bool C_XING_API::OnWindowMessage(UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
	/*
	if (XM_DEFAULT_VALUE < _nMessage && XM_MAX > _nMessage)
	{
		DBGPRINT("C_XING_API::OnWindowMessage: %d, %d, %d", _nMessage, _wParam, _lParam);
	}
	*/
	// 실시간 데이터인지 먼저 확인한다.
	switch (_nMessage)
	{
	case XM_RECEIVE_REAL_DATA:					// 실시간 데이터
		//DBGPRINT("AdviseData 요청에 대한 응답");
		// 여기에선 큐에 넣고 바로 종료해야 하는데, 여기에서 바로 Send 해도 될거 같은?
		do
		{
			PACKET_BASE 패킷 =
			{
				0
				, _PKT_PIPE_RECEIVE_REALTIME_DATA_
				, { 0 }
			};
			//실시간_데이터_이베스트포 데이터포 = (실시간_데이터_이베스트포)&패킷.bytBuffer[0];
			LPRECV_REAL_PACKET 패킷포 = (LPRECV_REAL_PACKET)_lParam;
			//디뷰("C_XING_API::OnWindowMessage(XM_RECEIVE_REAL_DATA) - %s", 패킷포->szTrCode);
			if ('_' == 패킷포->szTrCode[2])
			{
				if (('S' == 패킷포->szTrCode[0] && '3' == 패킷포->szTrCode[1])		// "S3_" KOSPI체결
					|| ('K' == 패킷포->szTrCode[0] && '3' == 패킷포->szTrCode[1]))	// "K3_" KOSDAQ체결
				{	// 주식체결
					패킷.bytBuffer[0] = _이베_주식체결_;
				}
				if (('H' == 패킷포->szTrCode[0] && '1' == 패킷포->szTrCode[1])		// "S3_" KOSPI호가잔량
					|| ('H' == 패킷포->szTrCode[0] && 'A' == 패킷포->szTrCode[1]))	// "SA_" KOSDAQ호가잔량
				{	// 호가잔량
					패킷.bytBuffer[0] = _이베_주식호가잔량_;
				}
				if (0 == 패킷.bytBuffer[0])
				{
					//디뷰("아직안만듬: (%d) %x, %x, %x", 패킷포->nDataLength, 패킷포, 패킷포->pszData, &패킷포->pszData);
					패킷.bytBuffer[0] = _이베_만들어야함_;
				}
				memcpy_s(&패킷.bytBuffer[1], (1 << 12), 패킷포->pszData, 패킷포->nDataLength);
				패킷.nPacketSize = (WORD)(sizeof(BYTE) + 패킷포->nDataLength);
			}
			//디뷰("[%d/%d] %s", pRealPacket->nDataLength, ::strlen((LPCSTR)파이프패킷.bytBuffer), (LPCSTR)파이프패킷.bytBuffer);
			pMain->파이프전송(&패킷);
			//디뷰("[%d] %s >> %s", pRealPacket->nDataLength, pRealPacket->szTrCode, pRealPacket->pszData);
		} while (false);
		return(true);
	case XM_RECEIVE_REAL_DATA_CHART:			// 지표실시간 데이터를 받았을 때
		DBGPRINT("실시간 자동 등록한 후 차트 조회 시, 지표실시간 데이터를 받았을 때");

		return(true);
	case XM_RECEIVE_REAL_DATA_SEARCH:			// 종목검색 실시간 데이터를 받았을 때
		DBGPRINT("종목검색 실시간 데이터를 받았을 때");

		return(true);
	}
	// 실시간 데이터가 아니었다면 다른 XingAPI 메시지인지 확인한다.
	switch (_nMessage)
	{
	case XM_DISCONNECT:
		DBGPRINT("이베스트 서버 접속 끊김");

		return(true);
	case XM_RECEIVE_DATA:						// tr 요청
		DBGPRINT("RequestData 요청에 대한 응답");
		do
		{
			PACKET_BASE 패킷 =
			{
				0
				, _PKT_PIPE_RECEIVE_TR_DATA_
				, { 0 }
			};
			LPRECV_PACKET 패킷포 = (LPRECV_PACKET)_lParam;
			memcpy_s(&패킷.bytBuffer[1], (1 << 12), 패킷포->lpData, 패킷포->nDataLength);
			패킷.nPacketSize = (WORD)(sizeof(BYTE) + 패킷포->nDataLength);
			pMain->파이프전송(&패킷);
		} while (false);
		return(true);
	case XM_LOGIN:								// 로그인 결과
		if (!::atoi((LPCSTR)_wParam))
		{	// 로그인 성공
			pMain->파이프전송(_PKT_PIPE_INIT_COMPLETED_);
			BOOL bResult = FALSE;
			// 실시간 등록
			bResult = AdviseRealData("SC0", "", 0);	// 주식주문접수
			bResult = AdviseRealData("SC1", "", 0); // 주식주문체결
			bResult = AdviseRealData("SC2", "", 0); // 주식주문정정
			bResult = AdviseRealData("SC3", "", 0); // 주식주문취소
			bResult = AdviseRealData("SC4", "", 0); // 주식주문거부

			bResult = AdviseRealData("IJ_", "001", 3);	// 코스피지수
			bResult = AdviseRealData("IJ_", "101", 3);	// 코스피200
			bResult = AdviseRealData("IJ_", "201", 3);	// 코스닥지수

			DBGPRINT("XM_LOGIN  %s / %s", (LPCSTR)_wParam, (LPCSTR)_lParam);
		}
		else
		{	// 로그인 실패
			pMain->파이프전송(_PKT_PIPE_INIT_FAILED_);
			DBGPRINT("XM_LOGIN %s / %s", (LPCSTR)_wParam, (LPCSTR)_lParam);
		}
		return(true);
	case XM_LOGOUT:								// 로그아웃 통보 받음
		DBGPRINT("로그아웃");

		return(true);
	case XM_TIMEOUT_DATA:						// tr 요청 지연
		DBGPRINT("RequestData 요청에 timeout 발생");

		return(true);
	case XM_RECEIVE_LINK_DATA:					// HTS 에서 연동 데이터가 발생했을 때
		DBGPRINT("HTS 에서 연동 데이터가 발생했을 때");

		return(true);
	};
	return(false);
}
