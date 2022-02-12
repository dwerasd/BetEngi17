// KHOpenAPI.cpp  : Microsoft Visual C++로 만든 ActiveX 컨트롤 래퍼 클래스의 정의입니다.


#include "pch.h"
#include "CKHOpenAPICtrl.h"
#include "resource.h"
#include "DOpenAPI.h"
#include "DOpenAPIDlg.h"



CLSID const& C_KH_OPEN_API::GetClsid()
{
	static CLSID const clsid= { 0xA1574A0D, 0x6BFA, 0x4BD7, { 0x90, 0x20, 0xDE, 0xD8, 0x87, 0x11, 0x81, 0x8D } };
	return clsid;
}

BOOL C_KH_OPEN_API::Create(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* _pContext)
{
	lpszClassName;
	_pContext;
	return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID);
}

BOOL C_KH_OPEN_API::Create(LPCSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CFile* pPersist, BOOL bStorage, BSTR bstrLicKey)
{
	return CreateControl(GetClsid(), lpszWindowName, dwStyle, rect, pParentWnd, nID, pPersist, bStorage, bstrLicKey);
}

long C_KH_OPEN_API::CommConnect()
{
	long result = 0;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return(result);
}
void C_KH_OPEN_API::CommTerminate()
{
	InvokeHelper(0x2, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
long C_KH_OPEN_API::CommRqData(LPCSTR sRQName, LPCSTR sTrCode, long nPrevNext, LPCSTR sScreenNo)
{
	long result = 0;
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sRQName, sTrCode, nPrevNext, sScreenNo);
	return(result);
}
CStringA C_KH_OPEN_API::GetLoginInfo(LPCSTR sTag)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTag);
	return(result);
}
long C_KH_OPEN_API::SendOrder(LPCSTR sRQName, LPCSTR sScreenNo, LPCSTR sAccNo, long nOrderType, LPCSTR sCode, long nQty, long nPrice, LPCSTR sHogaGb, LPCSTR sOrgOrderNo)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR;
	long result = 0;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sRQName, sScreenNo, sAccNo, nOrderType, sCode, nQty, nPrice, sHogaGb, sOrgOrderNo);
	return(result);
}
// 이전 샘플에서 추출
long C_KH_OPEN_API::SendOrderFO(LPCSTR sScreenNo, LPCSTR sAccNo, LPCSTR sPw, LPCSTR sCode, long nOrderType, long nOrderCount, long nOrderPrice, LPCSTR sOrderDTCD, long nCndtDTCD, LPCSTR sOrgOrderNo)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_I4 VTS_BSTR;
	long result = 0;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sScreenNo, sAccNo, sPw, sCode, nOrderType, nOrderCount, nOrderPrice, sOrderDTCD, nCndtDTCD, sOrgOrderNo);
	return(result);
}
// 최근 샘플 함수
long C_KH_OPEN_API::SendOrderFO(LPCSTR sRQName, LPCSTR sScreenNo, LPCSTR sAccNo, LPCSTR sCode, long lOrdKind, LPCSTR sSlbyTp, LPCSTR sOrdTp, long lQty, LPCSTR sPrice, LPCSTR sOrgOrdNo)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR;
	long result = 0;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sRQName, sScreenNo, sAccNo, sCode, lOrdKind, sSlbyTp, sOrdTp, lQty, sPrice, sOrgOrdNo);
	return(result);
}
void C_KH_OPEN_API::SetInputValue(LPCSTR sID, LPCSTR sValue)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, parms, sID, sValue);
}
long C_KH_OPEN_API::SetOutputFID(LPCSTR sID)
{
	static BYTE parms[] = VTS_BSTR;
	long result = 0;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sID);
	return(result);
}
CStringA C_KH_OPEN_API::CommGetData(LPCSTR sJongmokCode, LPCSTR sRealType, LPCSTR sFieldName, long nIndex, LPCSTR sInnerFieldName)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR;
	CStringA result;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sJongmokCode, sRealType, sFieldName, nIndex, sInnerFieldName);
	return(result);
}
void C_KH_OPEN_API::DisconnectRealData(LPCSTR sScnNo)
{
	static BYTE parms[] = VTS_BSTR;
	InvokeHelper(0xa, DISPATCH_METHOD, VT_EMPTY, NULL, parms, sScnNo);
}
long C_KH_OPEN_API::GetRepeatCnt(LPCSTR sTrCode, LPCSTR sRecordName)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR;
	long result = 0;
	InvokeHelper(0xb, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sTrCode, sRecordName);
	return(result);
}
long C_KH_OPEN_API::CommKwRqData(LPCSTR sArrCode, long bNext, long nCodeCount, long nTypeFlag, LPCSTR sRQName, LPCSTR sScreenNo)
{
	static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR;
	long result = 0;
	InvokeHelper(0xc, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sArrCode, bNext, nCodeCount, nTypeFlag, sRQName, sScreenNo);
	return(result);
}
CStringA C_KH_OPEN_API::GetAPIModulePath()
{
	CStringA result;
	InvokeHelper(0xd, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetCodeListByMarket(LPCSTR sMarket)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0xe, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sMarket);
	return(result);
}
long C_KH_OPEN_API::GetConnectState()
{
	long result = 0;
	InvokeHelper(0xf, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetMasterCodeName(LPCSTR sTrCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x10, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode);
	return(result);
}
long C_KH_OPEN_API::GetMasterListedStockCnt(LPCSTR sTrCode)
{
	static BYTE parms[] = VTS_BSTR; 
	long result = 0;
	InvokeHelper(0x11, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sTrCode);
	return(result);
}
CStringA C_KH_OPEN_API::GetMasterConstruction(LPCSTR sTrCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x12, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode);
	return(result);
}

CStringA C_KH_OPEN_API::GetMasterListedStockDate(LPCSTR sTrCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x13, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode);
	return(result);
}
CStringA C_KH_OPEN_API::GetMasterLastPrice(LPCSTR sTrCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x14, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode);
	return(result);
}
CStringA C_KH_OPEN_API::GetMasterStockState(LPCSTR sTrCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x15, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode);
	return(result);
}
long C_KH_OPEN_API::GetDataCount(LPCSTR strRecordName)
{
	static BYTE parms[] = VTS_BSTR;
	long result = 0;
	InvokeHelper(0x16, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strRecordName);
	return(result);
}
CStringA C_KH_OPEN_API::GetOutputValue(LPCSTR strRecordName, long nRepeatIdx, long nItemIdx)
{
	static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4;
	CStringA result;
	InvokeHelper(0x17, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strRecordName, nRepeatIdx, nItemIdx);
	return(result);
}
CStringA C_KH_OPEN_API::GetCommData(LPCSTR strTrCode, LPCSTR strRecordName, long nIndex, LPCSTR strItemName)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR;
	CStringA result;
	InvokeHelper(0x18, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strTrCode, strRecordName, nIndex, strItemName);
	return(result);
}
CStringA C_KH_OPEN_API::GetCommRealData(LPCSTR sTrCode, long nFid)
{
	static BYTE parms[] = VTS_BSTR VTS_I4;
	CStringA result;
	InvokeHelper(0x19, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode, nFid);
	return(result);
}
CStringA C_KH_OPEN_API::GetChejanData(long nFid)
{
	static BYTE parms[] = VTS_I4;
	CStringA result;
	InvokeHelper(0x1a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, nFid);
	return(result);
}
// 이 아래는 새로 추가된 함수들이다.
CStringA C_KH_OPEN_API::GetThemeGroupList(long nType)
{
	static BYTE parms[] = VTS_I4; 
	CStringA result;
	InvokeHelper(0x1b, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, nType);
	return(result);
}
CStringA C_KH_OPEN_API::GetThemeGroupCode(LPCSTR strThemeCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x1c, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strThemeCode);
	return(result);
}
CStringA C_KH_OPEN_API::GetFutureList()
{
	CStringA result;
	InvokeHelper(0x1d, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetFutureCodeByIndex(long nIndex)
{
	static BYTE parms[] = VTS_I4;
	CStringA result;
	InvokeHelper(0x1e, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, nIndex);
	return(result);
}
CStringA C_KH_OPEN_API::GetActPriceList()
{
	CStringA result;
	InvokeHelper(0x1f, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetMonthList()
{
	CStringA result;
	InvokeHelper(0x20, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetOptionCode(LPCSTR strActPrice, long nCp, LPCSTR strMonth)
{
	static BYTE parms[] = VTS_BSTR VTS_I4 VTS_BSTR;
	CStringA result;
	InvokeHelper(0x21, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strActPrice, nCp, strMonth);
	return(result);
}
CStringA C_KH_OPEN_API::GetOptionCodeByMonth(LPCSTR sTrCode, long nCp, LPCSTR strMonth)
{
	static BYTE parms[] = VTS_BSTR VTS_I4 VTS_BSTR;
	CStringA result;
	InvokeHelper(0x22, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode, nCp, strMonth);
	return(result);
}
CStringA C_KH_OPEN_API::GetOptionCodeByActPrice(LPCSTR sTrCode, long nCp, long nTick)
{
	static BYTE parms[] = VTS_BSTR VTS_I4 VTS_I4;
	CStringA result;
	InvokeHelper(0x23, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sTrCode, nCp, nTick);
	return(result);
}
CStringA C_KH_OPEN_API::GetSFutureList(LPCSTR strBaseAssetCode)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x24, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetCode);
	return(result);
}
CStringA C_KH_OPEN_API::GetSFutureCodeByIndex(LPCSTR strBaseAssetCode, long nIndex)
{
	static BYTE parms[] = VTS_BSTR VTS_I4;
	CStringA result;
	InvokeHelper(0x25, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetCode, nIndex);
	return(result);
}
CStringA C_KH_OPEN_API::GetSActPriceList(LPCSTR strBaseAssetGb)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x26, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetGb);
	return(result);
}
CStringA C_KH_OPEN_API::GetSMonthList(LPCSTR strBaseAssetGb)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x27, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetGb);
	return(result);
}
CStringA C_KH_OPEN_API::GetSOptionCode(LPCSTR strBaseAssetGb, LPCSTR strActPrice, long nCp, LPCSTR strMonth)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR;
	CStringA result;
	InvokeHelper(0x28, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetGb, strActPrice, nCp, strMonth);
	return(result);
}
CStringA C_KH_OPEN_API::GetSOptionCodeByMonth(LPCSTR strBaseAssetGb, LPCSTR sTrCode, long nCp, LPCSTR strMonth)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR; 
	CStringA result;
	InvokeHelper(0x29, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetGb, sTrCode, nCp, strMonth);
	return(result);
}
CStringA C_KH_OPEN_API::GetSOptionCodeByActPrice(LPCSTR strBaseAssetGb, LPCSTR sTrCode, long nCp, long nTick)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I4 VTS_I4;
	CStringA result;
	InvokeHelper(0x2a, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetGb, sTrCode, nCp, nTick);
	return(result);
}
CStringA C_KH_OPEN_API::GetSFOBasisAssetList()
{
	CStringA result;
	InvokeHelper(0x2b, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetOptionATM()
{
	CStringA result;
	InvokeHelper(0x2c, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetSOptionATM(LPCSTR strBaseAssetGb)
{
	static BYTE parms[] = VTS_BSTR;
	CStringA result;
	InvokeHelper(0x2d, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, strBaseAssetGb);
	return(result);
}
CStringA C_KH_OPEN_API::GetBranchCodeName()
{
	CStringA result;
	InvokeHelper(0x2e, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
long C_KH_OPEN_API::CommInvestRqData(LPCSTR sMarketGb, LPCSTR sRQName, LPCSTR sScreenNo)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR; 
	long result = 0;
	InvokeHelper(0x2f, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sMarketGb, sRQName, sScreenNo);
	return(result);
}
long C_KH_OPEN_API::SendOrderCredit(LPCSTR sRQName, LPCSTR sScreenNo, LPCSTR sAccNo, long nOrderType, LPCSTR sCode, long nQty, long nPrice, LPCSTR sHogaGb, LPCSTR sCreditGb, LPCSTR sLoanDate, LPCSTR sOrgOrderNo)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_I4 VTS_BSTR VTS_I4 VTS_I4 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR;
	long result = 0;
	InvokeHelper(0x30, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sRQName, sScreenNo, sAccNo, nOrderType, sCode, nQty, nPrice, sHogaGb, sCreditGb, sLoanDate, sOrgOrderNo);
	return(result);
}
CStringA C_KH_OPEN_API::KOA_Functions(LPCSTR sFunctionName, LPCSTR sParam)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR;
	CStringA result;
	InvokeHelper(0x31, DISPATCH_METHOD, VT_BSTR, (void*)&result, parms, sFunctionName, sParam);
	return(result);
}
long C_KH_OPEN_API::SetInfoData(LPCSTR sInfoData)
{
	static BYTE parms[] = VTS_BSTR;
	long result = 0;
	InvokeHelper(0x32, DISPATCH_METHOD, VT_I4, (void*)&result, parms, sInfoData);
	return(result);
}
long C_KH_OPEN_API::SetRealReg(LPCSTR strScreenNo, LPCSTR strCodeList, LPCSTR strFidList, LPCSTR strOptType)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR;
	long result = 0;
	InvokeHelper(0x33, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strScreenNo, strCodeList, strFidList, strOptType);
	return(result);
}
long C_KH_OPEN_API::GetConditionLoad()
{
	long result = 0;
	InvokeHelper(0x34, DISPATCH_METHOD, VT_I4, (void*)&result, NULL);
	return(result);
}
CStringA C_KH_OPEN_API::GetConditionNameList()
{
	CStringA result;
	InvokeHelper(0x35, DISPATCH_METHOD, VT_BSTR, (void*)&result, NULL);
	return(result);
}
BOOL C_KH_OPEN_API::SendCondition(LPCSTR strScrNo, LPCSTR strConditionName, int nIndex, int nSearch)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I2 VTS_I2;
	BOOL result = FALSE;
	InvokeHelper(0x36, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms, strScrNo, strConditionName, nIndex, nSearch);
	return(result);
}
void C_KH_OPEN_API::SendConditionStop(LPCSTR strScrNo, LPCSTR strConditionName, int nIndex)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR VTS_I2;
	InvokeHelper(0x37, DISPATCH_METHOD, VT_EMPTY, NULL, parms, strScrNo, strConditionName, nIndex);
}
COleVariant C_KH_OPEN_API::GetCommDataEx(LPCSTR strTrCode, LPCSTR strRecordName)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR;
	COleVariant result;
	InvokeHelper(0x38, DISPATCH_METHOD, VT_VARIANT, (void*)&result, parms, strTrCode, strRecordName);
	return(result);
}
void C_KH_OPEN_API::SetRealRemove(LPCSTR strScrNo, LPCSTR strDelCode)
{
	static BYTE parms[] = VTS_BSTR VTS_BSTR;
	InvokeHelper(0x39, DISPATCH_METHOD, VT_EMPTY, NULL, parms, strScrNo, strDelCode);
}
long C_KH_OPEN_API::GetMarketType(LPCSTR strCode)
{
	static BYTE parms[] = VTS_BSTR;
	long result = 0;
	InvokeHelper(0x3a, DISPATCH_METHOD, VT_I4, (void*)&result, parms, strCode);
	return(result);
}
