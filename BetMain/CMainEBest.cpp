#include "framework.h"
#include "CMain.h"



///////////////////////////////////////////////////////////////////////////////
// 브릿지 패킷은 모두 여기에서 처리한다.

static C_MAIN* pMain = nullptr;
static C_GAME* pGame = nullptr;
static C_BRIDGE_BASE* pBridgeEBest = nullptr;
LPVOID __stdcall BridgeCallbackEBest(ULONG_PTR _dwMessage, LPVOID _pData)
{
	try
	{
		//if (!pMain) { pMain = C_MAIN::GetInstance(); }
		switch (_dwMessage)
		{
		case _PKT_PIPE_CONNECTED_XINGAPI_:
			if (!pMain) { pMain = C_MAIN::GetInstance(); }
			if (!pBridgeEBest) { pBridgeEBest = pMain->pBridgeEBest; }
			if (!pGame) { pGame = C_GAME::GetInstance(); }
			pBridgeEBest->SetStatus(_브릿지_로그인중_);
			pBridgeEBest->SendPipe(_PKT_PIPE_INIT_XINGAPI_);
			break;
		case _PKT_PIPE_INIT_COMPLETED_:
			pBridgeEBest->SetStatus(_브릿지_연결됨_);
			break;
		case _PKT_PIPE_INIT_FAILED_:
			디뷰메시지("로그인 실패");
			pBridgeEBest->SendPipe(_PKT_PIPE_DESTROY_);
			break;
		case _PKT_PIPE_DISCONNECTED_XINGAPI_:
			디뷰메시지("클라이언트 종료 받음.");
			pBridgeEBest->SetStatus(_브릿지_연결끊김_);
			pBridgeEBest->Destroy();
			break;
		case _PKT_PIPE_INFO_STOCKS_XINGAPI_:
			pBridgeEBest->SetStatus(_브릿지_종목전송중_);
			break;
		case _PKT_PIPE_INIT_FINISH_XINGAPI_:
			pBridgeEBest->SetStatus(_브릿지_종목전송완료_);
			break;
		case _PKT_PIPE_RECEIVE_REALTIME_DATA_:	// 실시간 데이터 날라옴.
			switch (*(LPBYTE)_pData)
			{
			case _이베_예외처리_:
				break;
			case _이베_주식체결_:
				//ConvertTickEBsest((주식체결_이베스트포)&_pPipePacket->bytBuffer[1]);
				break;
			case _이베_주식호가잔량_:
				//PushHogaEBest((주식호가잔량_이베스트포)&_pPipePacket->bytBuffer[1]);
				break;
			}
			break;
		case _PKT_PIPE_RECEIVE_TR_DATA_:		// tr 데이터를 받음. (RequestData 요청에 대한 응답)

			break;
		}
	}
	catch (...)
	{
		디뷰("%s(%d) - %s, dwMessage: %d", __FILE__, __LINE__, __FUNCSIG__, _dwMessage);
	}
	return(nullptr);
}
