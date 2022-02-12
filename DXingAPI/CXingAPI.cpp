#include "framework.h"
#include "CXingAPI.h"



C_XING_API::C_XING_API()
	: hWnd(0)
	, hXingAPI(0)

	, m_fpConnect(nullptr)
	, m_fpIsConnected(nullptr)
	, m_fpDisconnect(nullptr)
	, m_fpLogin(nullptr)
	, m_fpLogout(nullptr)

	, m_fpGetLastError(nullptr)
	, m_fpGetErrorMessage(nullptr)

	, m_fpRequest(nullptr)
	, m_fpAdviseRealData(nullptr)
	, m_fpUnadviseRealData(nullptr)
	, m_fpUnadviseWindow(nullptr)
	, m_fpReleaseRequestData(nullptr)
	, m_fpReleaseMessageData(nullptr)

	, m_fpGetAccountListCount(nullptr)
	, m_fpGetAccountList(nullptr)
	, m_fpGetAccountName(nullptr)
	, m_fpGetAcctDetailName(nullptr)
	, m_fpGetAcctNickname(nullptr)

	, m_fpGetCommMedia(nullptr)
	, m_fpGetETKMedia(nullptr)
	, m_fpGetClientIP(nullptr)
	, m_fpGetServerName(nullptr)
	, m_fpGetAPIPath(nullptr)

	, m_fpSetHeaderInfo(nullptr)
	, m_fpSetUseAPIVer(nullptr)
	, m_fpSetMode(nullptr)

	, m_fpGetProcBranchNo(nullptr)
	, m_fpGetUseOverFuture(nullptr)
	, m_fpGetUseFX(nullptr)

	, m_fpGetTRCountPerSec(nullptr)
	, m_fpGetTRCountBaseSec(nullptr)
	, m_fpGetTRCountRequest(nullptr)
	, m_fpGetTRCountLimit(nullptr)

	, m_fpSetNotifyFlag(nullptr)

	, m_fpRequestService(nullptr)
	, m_fpRemoveService(nullptr)

	, m_fpRequestLinkToHTS(nullptr)
	, m_fpAdviseLinkFromHTS(nullptr)
	, m_fpUnAdviseLinkFromHTS(nullptr)

	, m_fpDecompress(nullptr)
	, m_fpIsChartLib(nullptr)

	, m_fpSetProgramOrder(nullptr)
	, m_fpGetProgramOrder(nullptr)

{
	//ZeroMemory(this, sizeof(C_XING_API));
	wchar_t szCommLib[_MAX_PATH] = { 0 };
	dk::GetCurrentDirectoryW(szCommLib);
	wcscat_s(szCommLib, L"XingAPI/");
	// \\XingAPI 폴더로 세팅.
	SetCurrentDirectoryW(szCommLib);
	// 파일 경로를 완성하고
	wcscat_s(szCommLib, L"XingAPI.dll");
	// XingAPI.dll 을 읽어오고
	hXingAPI = ::LoadLibraryW(szCommLib);

	DBGPRINT(L"[%x] %x - %s", hWnd, hXingAPI, szCommLib);
	디뷰("sizeof(체결_데이터_이베스트): %d", sizeof(체결_데이터_이베스트));
	// 실행 파일 폴더로 다시 세팅.
	dk::GetCurrentDirectoryW(szCommLib);
	ZeroMemory(szCommLib, sizeof(szCommLib));
}

C_XING_API::~C_XING_API()
{
	Destroy();
}

bool C_XING_API::Init(HWND _hWnd)
{
	hWnd = _hWnd;
	BOOL bResult = false;
	do
	{
		if (!hXingAPI)
		{
			DBGPRINT("hXingAPI == 0");
			break;
		}
		m_fpConnect = (FP_CONNECT)::GetProcAddress(hXingAPI, "ETK_Connect");
		if (!m_fpConnect) { break; }
		m_fpIsConnected = (FP_ISCONNECTED)::GetProcAddress(hXingAPI, "ETK_IsConnected");
		if (!m_fpIsConnected) { break; }
		m_fpDisconnect = (FP_DISCONNECT)::GetProcAddress(hXingAPI, "ETK_Disconnect");
		if (!m_fpDisconnect) { break; }
		m_fpLogin = (FP_LOGIN)::GetProcAddress(hXingAPI, "ETK_Login");
		if (!m_fpLogin) { break; }
		m_fpLogout = (FP_LOGOUT)::GetProcAddress(hXingAPI, "ETK_Logout");
		if (!m_fpLogout) { break; }

		m_fpGetLastError = (FP_GETLASTERROR)::GetProcAddress(hXingAPI, "ETK_GetLastError");
		if (!m_fpGetLastError) { break; }
		m_fpGetErrorMessage = (FP_GETERRORMESSAGE)::GetProcAddress(hXingAPI, "ETK_GetErrorMessage");
		if (!m_fpGetErrorMessage) { break; }

		m_fpRequest = (FP_REQUEST)::GetProcAddress(hXingAPI, "ETK_Request");
		if (!m_fpRequest) { break; }
		m_fpAdviseRealData = (FP_ADVISEREALDATA)::GetProcAddress(hXingAPI, "ETK_AdviseRealData");
		if (!m_fpAdviseRealData) { break; }
		m_fpUnadviseRealData = (FP_UNADVISEREALDATA)::GetProcAddress(hXingAPI, "ETK_UnadviseRealData");
		if (!m_fpUnadviseRealData) { break; }
		m_fpUnadviseWindow = (FP_UNADVISEWINDOW)::GetProcAddress(hXingAPI, "ETK_UnadviseWindow");
		if (!m_fpUnadviseWindow) { break; }
		m_fpReleaseRequestData = (FP_RELEASEREQUESTDATA)::GetProcAddress(hXingAPI, "ETK_ReleaseRequestData");
		if (!m_fpReleaseRequestData) { break; }
		m_fpReleaseMessageData = (FP_RELEASEMESSAGEDATA)::GetProcAddress(hXingAPI, "ETK_ReleaseMessageData");
		if (!m_fpReleaseMessageData) { break; }
		
		m_fpGetAccountListCount = (FP_GETACCOUNTLISTCOUNT)::GetProcAddress(hXingAPI, "ETK_GetAccountListCount");
		if (!m_fpGetAccountListCount) { break; }
		m_fpGetAccountList = (FP_GETACCOUNTLIST)::GetProcAddress(hXingAPI, "ETK_GetAccountList");
		if (!m_fpGetAccountList) { break; }
		m_fpGetAccountName = (FP_GETACCOUNTNAME)::GetProcAddress(hXingAPI, "ETK_GetAccountName");
		if (!m_fpGetAccountName) { break; }
		m_fpGetAcctDetailName = (FP_GETACCTDETAILNAME)::GetProcAddress(hXingAPI, "ETK_GetAcctDetailName");
		if (!m_fpGetAcctDetailName) { break; }
		m_fpGetAcctNickname = (FP_GETACCTNICKNAME)::GetProcAddress(hXingAPI, "ETK_GetAcctNickname");
		if (!m_fpGetAcctNickname) { break; }

		m_fpGetCommMedia = (FP_GETCOMMMEDIA)::GetProcAddress(hXingAPI, "ETK_GetCommMedia");
		if (!m_fpGetCommMedia) { break; }
		m_fpGetETKMedia = (FP_GETETKMEDIA)::GetProcAddress(hXingAPI, "ETK_GetETKMedia");
		if (!m_fpGetETKMedia) { break; }
		m_fpGetClientIP = (FP_GETCLIENTIP)::GetProcAddress(hXingAPI, "ETK_GetClientIP");
		if (!m_fpGetClientIP) { break; }
		m_fpGetServerName = (FP_GETSERVERNAME)::GetProcAddress(hXingAPI, "ETK_GetServerName");
		if (!m_fpGetServerName) { break; }
		m_fpGetAPIPath = (FP_GETAPIPATH)::GetProcAddress(hXingAPI, "ETK_GetAPIPath");
		if (!m_fpGetAPIPath) { break; }

		m_fpSetHeaderInfo = (FP_SETHEADERINFO)::GetProcAddress(hXingAPI, "ETK_SetHeaderInfo");
		if (!m_fpSetHeaderInfo) { break; }
		m_fpSetUseAPIVer = (FP_SETUSEAPIVER)::GetProcAddress(hXingAPI, "ETK_SetUseAPIVer");
		if (!m_fpSetUseAPIVer) { break; }
		m_fpSetMode = (FP_SETMODE)::GetProcAddress(hXingAPI, "ETK_SetMode");
		if (!m_fpSetMode) { break; }

		m_fpGetProcBranchNo = (FP_GETPROCBRANCHNO)::GetProcAddress(hXingAPI, "ETK_GetProcBranchNo");
		if (!m_fpGetProcBranchNo) { break; }
		m_fpGetUseOverFuture = (FP_GETUSEOVERFUTURE)::GetProcAddress(hXingAPI, "ETK_GetUseOverFuture");
		if (!m_fpGetUseOverFuture) { break; }
		m_fpGetUseFX = (FP_GETUSEFX)::GetProcAddress(hXingAPI, "ETK_GetUseFX");
		if (!m_fpGetUseFX) { break; }

		m_fpGetTRCountPerSec = (FP_GETTRCOUNTPERSEC)::GetProcAddress(hXingAPI, "ETK_GetTRCountPerSec");
		if (!m_fpGetTRCountPerSec) { break; }
		m_fpGetTRCountBaseSec = (FP_GETTRCOUNTBASESEC)::GetProcAddress(hXingAPI, "ETK_GetTRCountBaseSec");
		if (!m_fpGetTRCountBaseSec) { break; }
		m_fpGetTRCountRequest = (FP_GETTRCOUNTREQUEST)::GetProcAddress(hXingAPI, "ETK_GetTRCountRequest");
		if (!m_fpGetTRCountRequest) { break; }
		m_fpGetTRCountLimit = (FP_GETTRCOUNTLIMIT)::GetProcAddress(hXingAPI, "ETK_GetTRCountLimit");
		if (!m_fpGetTRCountLimit) { break; }

		m_fpSetNotifyFlag = (FP_SETNOTIFYFLAG)::GetProcAddress(hXingAPI, "ETK_SetNotifyFlag");
		if (!m_fpSetNotifyFlag) { break; }
		
		m_fpRequestService = (FP_REQUESTSERVICE)::GetProcAddress(hXingAPI, "ETK_RequestService");
		if (!m_fpRequestService) { break; }
		m_fpRemoveService = (FP_REMOVESERVICE)::GetProcAddress(hXingAPI, "ETK_RemoveService");
		if (!m_fpRemoveService) { break; }

		m_fpRequestLinkToHTS = (FP_REQUESTLINKTOHTS)::GetProcAddress(hXingAPI, "ETK_RequestLinkToHTS");
		if (!m_fpRequestLinkToHTS) { break; }
		m_fpAdviseLinkFromHTS = (FP_ADVISELINKFROMHTS)::GetProcAddress(hXingAPI, "ETK_AdviseLinkFromHTS");
		if (!m_fpAdviseLinkFromHTS) { break; }
		m_fpUnAdviseLinkFromHTS = (FP_UNADVISELINKFROMHTS)::GetProcAddress(hXingAPI, "ETK_UnAdviseLinkFromHTS");
		if (!m_fpUnAdviseLinkFromHTS) { break; }

		m_fpDecompress = (FP_DECOMPRESS)::GetProcAddress(hXingAPI, "ETK_Decompress");
		if (!m_fpDecompress) { break; }
		m_fpIsChartLib = (FP_ISCHARTLIB)::GetProcAddress(hXingAPI, "ETK_IsChartLib");
		if (!m_fpIsChartLib) { break; }

		m_fpSetProgramOrder = (FP_SETPROGRAMORDER)::GetProcAddress(hXingAPI, "ETK_SetProgramOrder");
		if (!m_fpSetProgramOrder) { break; }
		m_fpGetProgramOrder = (FP_GETPROGRAMORDER)::GetProcAddress(hXingAPI, "ETK_GetProgramOrder");
		if (!m_fpGetProgramOrder) { break; }

		bResult = true;
	} while (false);
	return(bResult);
}

void C_XING_API::Destroy()
{
	if (0 != hXingAPI)
	{
		::FreeLibrary(hXingAPI);
	}
	//hXingAPI = 0;
	//ZeroMemory(this, sizeof(C_XING_API));
}

BOOL C_XING_API::Connect(LPCSTR _pszSvrIP, int _nPort, int _nStartMsgID, int _nTimeOut, int _nSendMaxPacketSize)
{
	if (m_fpConnect) { return m_fpConnect(hWnd, _pszSvrIP, _nPort, _nStartMsgID, _nTimeOut, _nSendMaxPacketSize);	}
	return(FALSE);
}

BOOL C_XING_API::IsConnected()
{
	if (m_fpIsConnected)	{ return m_fpIsConnected(); }
	return(FALSE);
}

BOOL C_XING_API::Disconnect()
{
	if (m_fpDisconnect) { return m_fpDisconnect(); }
	return(FALSE);
}

BOOL C_XING_API::Login(LPCSTR pszID, LPCSTR pszPwd, LPCSTR pszCertPwd, int nType, BOOL bShowCertErrDlg)
{
	if (m_fpLogin) { return m_fpLogin(hWnd, pszID, pszPwd, pszCertPwd, nType, bShowCertErrDlg); }
	return(FALSE);
}

BOOL C_XING_API::Logout()
{
	if (m_fpLogout) { return m_fpLogout(hWnd); }
	return(FALSE);
}

int	C_XING_API::GetLastError()
{
	if (m_fpGetLastError) { return m_fpGetLastError(); }
	return(0);
}

std::string	C_XING_API::GetErrorMessage(int nErrorCode)
{
	char szData[1024] = { 0 };
	if (m_fpGetErrorMessage) { m_fpGetErrorMessage(nErrorCode, szData, sizeof(szData)); }
	return szData;
}

int C_XING_API::Request(LPCSTR pszCode, LPVOID lpData, int nDataSize, BOOL bNext, LPCSTR pszNextKey, int nTimeOut)
{
	if (m_fpRequest) { return m_fpRequest(hWnd, pszCode, lpData, nDataSize, bNext, pszNextKey, nTimeOut); }
	return(-1);
}

BOOL C_XING_API::AdviseRealData(LPCSTR pszTrNo, LPCSTR pszData, int nDataUnitLen)
{
	if (m_fpAdviseRealData) { return m_fpAdviseRealData(hWnd, pszTrNo, pszData, nDataUnitLen); }
	return(FALSE);
}

BOOL C_XING_API::UnadviseRealData(LPCSTR pszTrNo, LPCSTR pszData, int nDataUnitLen)
{
	if (m_fpUnadviseRealData) { return m_fpUnadviseRealData(hWnd, pszTrNo, pszData, nDataUnitLen); }
	return(FALSE);
}

BOOL C_XING_API::UnadviseWindow()
{
	if (m_fpUnadviseWindow) { return m_fpUnadviseWindow(hWnd); }
	return(FALSE);
}

BOOL C_XING_API::ReleaseRequestData(int nRequestID)
{
	BOOL bResult = FALSE;
	if (m_fpReleaseRequestData)
	{
		m_fpReleaseRequestData(nRequestID);
		bResult = TRUE;
	}
	return(bResult);
}

BOOL C_XING_API::ReleaseMessageData(LPARAM lp)
{
	BOOL bResult = FALSE;
	if (m_fpReleaseMessageData)
	{
		m_fpReleaseMessageData(lp);
		bResult = TRUE;
	}
	return(bResult);
}

int	C_XING_API::GetAccountListCount()
{
	if (m_fpGetAccountListCount) { return m_fpGetAccountListCount(); }
	return 0;
}

BOOL C_XING_API::GetAccountList(int nIndex, LPSTR pszAcc, int nAccSize)
{
	if (m_fpGetAccountList) { return m_fpGetAccountList(nIndex, pszAcc, nAccSize); }
	return(FALSE);
}

BOOL C_XING_API::GetAccountName(LPCSTR pszAcc, LPSTR pszAccName, int nAccNameSize)
{
	if (m_fpGetAccountName) { return m_fpGetAccountName(pszAcc, pszAccName, nAccNameSize); }
	return(FALSE);
}

BOOL C_XING_API::GetAcctDetailName(LPCSTR pszAcc, LPSTR pszAcctDetailName, int nAcctDetailNameSize)
{
	if (m_fpGetAcctDetailName) { return m_fpGetAcctDetailName(pszAcc, pszAcctDetailName, nAcctDetailNameSize); }
	return(FALSE);
}

BOOL C_XING_API::GetAcctNickname(LPCSTR pszAcc, LPSTR pszAcctNickname, int nAcctNicknameSize)
{
	if (m_fpGetAcctNickname) { return m_fpGetAcctNickname(pszAcc, pszAcctNickname, nAcctNicknameSize); }
	return(FALSE);
}

std::string	C_XING_API::GetCommMedia()
{
	char szMedia[256] = { 0 };
	if (m_fpGetCommMedia) { m_fpGetCommMedia(szMedia); }
	return szMedia;
}

std::string	C_XING_API::GetETKMedia()
{
	char szMedia[256] = { 0 };
	if (m_fpGetETKMedia) { m_fpGetETKMedia(szMedia); }
	return szMedia;
}

std::string	C_XING_API::GetClientIP()
{
	char szIP[256] = { 0 };
	if (m_fpGetClientIP) { m_fpGetClientIP(szIP); }
	return szIP;
}

std::string	C_XING_API::GetServerName()
{
	char 종목명[256] = { 0 };
	if (m_fpGetServerName) { m_fpGetServerName(종목명); }
	return 종목명;
}

std::string	C_XING_API::GetAPIPath()
{
	char szPath[256] = { 0 };
	if (m_fpGetAPIPath)	{ m_fpGetAPIPath(szPath); }
	return szPath;
}

void C_XING_API::SetHeaderInfo(LPCSTR szType, LPCSTR szValue)
{
	if (m_fpSetHeaderInfo) { m_fpSetHeaderInfo(szType, szValue); }
}

void C_XING_API::SetUseAPIVer(LPCSTR szUserAPIVer)
{
	if (m_fpSetUseAPIVer) { m_fpSetUseAPIVer(szUserAPIVer); }
}

BOOL C_XING_API::SetMode(LPCSTR pszMode, LPCSTR pszValue)
{
	BOOL bResult = FALSE;
	if (m_fpSetMode)
	{
		m_fpSetMode(pszMode, pszValue);
		bResult = TRUE;
	}
	return(bResult);
}

std::string	C_XING_API::GetProcBranchNo()
{
	char szProcBranchNo[256] = { 0 };
	if (m_fpGetProcBranchNo) { m_fpGetProcBranchNo(szProcBranchNo); }
	return szProcBranchNo;
}

BOOL C_XING_API::GetUseOverFuture()
{
	if (m_fpGetUseOverFuture) { return m_fpGetUseOverFuture(); }
	return(FALSE);
}

BOOL C_XING_API::GetUseFX()
{
	if (m_fpGetUseFX) { return m_fpGetUseFX(); }
	return(FALSE);
}

int C_XING_API::GetTRCountPerSec(LPCSTR pszCode)
{
	if (m_fpGetTRCountPerSec) { return m_fpGetTRCountPerSec(pszCode); }
	return(-1);
}

int C_XING_API::GetTRCountBaseSec(LPCSTR pszCode)
{
	if (m_fpGetTRCountBaseSec) { return m_fpGetTRCountBaseSec(pszCode); }
	return(-1);
}

int C_XING_API::GetTRCountRequest(LPCSTR pszCode)
{
	if (m_fpGetTRCountRequest) { return m_fpGetTRCountRequest(pszCode); }
	return(-1);
}

int C_XING_API::GetTRCountLimit(LPCSTR pszCode)
{
	if (m_fpGetTRCountLimit) { return m_fpGetTRCountLimit(pszCode); }
	return(-1);
}

void C_XING_API::SetNotifyFlag(BOOL bNotifyFlag)
{
	if (m_fpSetNotifyFlag) { m_fpSetNotifyFlag(bNotifyFlag); }
}

int C_XING_API::RequestService(LPCSTR pszCode, LPCSTR pszData)
{
	if (m_fpRequestService)	{ return m_fpRequestService(hWnd, pszCode, pszData); }
	return(-1);
}

int C_XING_API::RemoveService(LPCSTR pszCode, LPCSTR pszData)
{
	if (m_fpRemoveService) { return m_fpRemoveService(hWnd, pszCode, pszData); }
	return(-1);
}

int C_XING_API::RequestLinkToHTS(LPCSTR pszLinkKey, LPCSTR pszData, LPCSTR pszFiller)
{
	if (m_fpRequestLinkToHTS) { return m_fpRequestLinkToHTS(hWnd, pszLinkKey, pszData, pszFiller); }
	return(-1);
}

void C_XING_API::AdviseLinkFromHTS()
{
	if (m_fpAdviseLinkFromHTS) { m_fpAdviseLinkFromHTS(hWnd); }
}

void C_XING_API::UnAdviseLinkFromHTS()
{
	if (m_fpUnAdviseLinkFromHTS) { m_fpUnAdviseLinkFromHTS(hWnd); }
}

int C_XING_API::Decompress(LPCSTR pszSrc, LPCSTR pszDest, int nSrcLen)
{
	if (m_fpDecompress)	{ return m_fpDecompress(pszSrc, pszDest, nSrcLen); }
	return(-1);
}

BOOL C_XING_API::IsChartLib()
{
	if (m_fpIsChartLib) { return m_fpIsChartLib(); }
	return(FALSE);
}

void C_XING_API::SetProgramOrder(BOOL bProgramOrder)
{
	if (NULL == m_fpSetProgramOrder) { m_fpSetProgramOrder(bProgramOrder); }
}

BOOL C_XING_API::GetProgramOrder()
{
	if (NULL == m_fpGetProgramOrder) { return m_fpGetProgramOrder(); }
	return(FALSE);
}
