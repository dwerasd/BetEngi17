#pragma once


#include <unordered_map>

#include <DarkCore/DMemory.h>
#include <DarkCore/DLocale.h>

#include "KHOpenAPIDef.h"

#include "CPipeClient.h"
#include "CNetClient.h"



class C_KH_OPEN_API
	: public CWnd
{
private:
	pipe::C_PIPE_CLIENT* pPipe{ nullptr };				// 받은 메시지를 최소한의 처리 후 전송만 한다.
	net::C_NET_CLIENT* pNet{ nullptr };

protected:
	DECLARE_DYNCREATE(C_KH_OPEN_API)

public:
	void SetHandler(pipe::C_PIPE_CLIENT* _pHandler);
	void SetNetHandler(net::C_NET_CLIENT* _pHandler);

	CLSID const& GetClsid();
	virtual BOOL Create(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* _pContext = nullptr);

	BOOL Create(LPCSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CFile* pPersist = nullptr, BOOL bStorage = FALSE, BSTR bstrLicKey = nullptr);

public:
	long CommConnect();
	void CommTerminate();
	long CommRqData(LPCSTR sRQName, LPCSTR sTrCode, long nPrevNext, LPCSTR sScreenNo);
	CStringA GetLoginInfo(LPCSTR sTag);
	long SendOrder(LPCSTR sRQName, LPCSTR sScreenNo, LPCSTR sAccNo, long nOrderType, LPCSTR sCode, long nQty, long nPrice, LPCSTR sHogaGb, LPCSTR sOrgOrderNo);
	// 이전 샘플에서 추출
	long SendOrderFO(LPCSTR sScreenNo, LPCSTR sAccNo, LPCSTR sPw, LPCSTR sCode, long nOrderType, long nOrderCount, long nOrderPrice, LPCSTR sOrderDTCD, long nCndtDTCD, LPCSTR sOrgOrderNo);
	// 최근 샘플 함수
	long SendOrderFO(LPCSTR sRQName, LPCSTR sScreenNo, LPCSTR sAccNo, LPCSTR sCode, long lOrdKind, LPCSTR sSlbyTp, LPCSTR sOrdTp, long lQty, LPCSTR sPrice, LPCSTR sOrgOrdNo);
	void SetInputValue(LPCSTR sID, LPCSTR sValue);
	long SetOutputFID(LPCSTR sID);
	CStringA CommGetData(LPCSTR sJongmokCode, LPCSTR sRealType, LPCSTR sFieldName, long nIndex, LPCSTR sInnerFieldName);
	void DisconnectRealData(LPCSTR sScnNo);
	long GetRepeatCnt(LPCSTR sTrCode, LPCSTR sRecordName);
	long CommKwRqData(LPCSTR sArrCode, long bNext, long nCodeCount, long nTypeFlag, LPCSTR sRQName, LPCSTR sScreenNo);
	CStringA GetAPIModulePath();
	CStringA GetCodeListByMarket(LPCSTR sMarket);
	long GetConnectState();
	CStringA GetMasterCodeName(LPCSTR sTrCode);
	long GetMasterListedStockCnt(LPCSTR sTrCode);
	CStringA GetMasterConstruction(LPCSTR sTrCode);
	CStringA GetMasterListedStockDate(LPCSTR sTrCode);
	CStringA GetMasterLastPrice(LPCSTR sTrCode);
	CStringA GetMasterStockState(LPCSTR sTrCode);
	long GetDataCount(LPCSTR strRecordName);
	CStringA GetOutputValue(LPCSTR strRecordName, long nRepeatIdx, long nItemIdx);
	CStringA GetCommData(LPCSTR strTrCode, LPCSTR strRecordName, long nIndex, LPCSTR strItemName);
	CStringA GetCommRealData(LPCSTR sTrCode, long nFid);
	CStringA GetChejanData(long nFid);
	// 이 아래는 새로 추가된 함수들이다.
	CStringA GetThemeGroupList(long nType);
	CStringA GetThemeGroupCode(LPCSTR strThemeCode);
	CStringA GetFutureList();
	CStringA GetFutureCodeByIndex(long nIndex);
	CStringA GetActPriceList();
	CStringA GetMonthList();
	CStringA GetOptionCode(LPCSTR strActPrice, long nCp, LPCSTR strMonth);
	CStringA GetOptionCodeByMonth(LPCSTR sTrCode, long nCp, LPCSTR strMonth);
	CStringA GetOptionCodeByActPrice(LPCSTR sTrCode, long nCp, long nTick);
	CStringA GetSFutureList(LPCSTR strBaseAssetCode);
	CStringA GetSFutureCodeByIndex(LPCSTR strBaseAssetCode, long nIndex);
	CStringA GetSActPriceList(LPCSTR strBaseAssetGb);
	CStringA GetSMonthList(LPCSTR strBaseAssetGb);
	CStringA GetSOptionCode(LPCSTR strBaseAssetGb, LPCSTR strActPrice, long nCp, LPCSTR strMonth);
	CStringA GetSOptionCodeByMonth(LPCSTR strBaseAssetGb, LPCSTR sTrCode, long nCp, LPCSTR strMonth);
	CStringA GetSOptionCodeByActPrice(LPCSTR strBaseAssetGb, LPCSTR sTrCode, long nCp, long nTick);
	CStringA GetSFOBasisAssetList();
	CStringA GetOptionATM();
	CStringA GetSOptionATM(LPCSTR strBaseAssetGb);
	CStringA GetBranchCodeName();
	long CommInvestRqData(LPCSTR sMarketGb, LPCSTR sRQName, LPCSTR sScreenNo);
	long SendOrderCredit(LPCSTR sRQName, LPCSTR sScreenNo, LPCSTR sAccNo, long nOrderType, LPCSTR sCode, long nQty, long nPrice, LPCSTR sHogaGb, LPCSTR sCreditGb, LPCSTR sLoanDate, LPCSTR sOrgOrderNo);
	CStringA KOA_Functions(LPCSTR sFunctionName, LPCSTR sParam);
	long SetInfoData(LPCSTR sInfoData);
	long SetRealReg(LPCSTR strScreenNo, LPCSTR strCodeList, LPCSTR strFidList, LPCSTR strOptType);
	long GetConditionLoad();
	CStringA GetConditionNameList();
	BOOL SendCondition(LPCSTR strScrNo, LPCSTR strConditionName, int nIndex, int nSearch);
	void SendConditionStop(LPCSTR strScrNo, LPCSTR strConditionName, int nIndex);
	COleVariant GetCommDataEx(LPCSTR strTrCode, LPCSTR strRecordName);
	void SetRealRemove(LPCSTR strScrNo, LPCSTR strDelCode);
	long GetMarketType(LPCSTR strCode);
	
	DECLARE_EVENTSINK_MAP()
	void OnEventConnect(long _nErrCode);
	void OnReceiveTrData(LPCSTR _화면번호, LPCSTR sRQName, LPCSTR sTrCode, LPCSTR sRecordName, LPCSTR sPrevNext, long nDataLength, LPCSTR sErrorCode, LPCSTR sMessage, LPCSTR sSplmMsg);
	void OnReceiveRealData(LPCSTR sRealKey, LPCSTR sRealType, LPCSTR sRealData);
	void OnReceiveChejanData(LPCSTR sGubun, long nItemCnt, LPCSTR sFIdList);
	void OnReceiveConditionVer(long lRet, LPCSTR sMsg);
	void OnReceiveTrCondition(LPCSTR _화면번호, LPCSTR strCodeList, LPCSTR strConditionName, long nIndex, long nNext);
	void OnReceiveRealCondition(LPCSTR sTrCode, LPCSTR strType, LPCSTR strConditionName, LPCSTR strConditionIndex);
};
