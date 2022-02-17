﻿#include "framework.h"
#include "CMain.h"


class C_FILE_HANDLER
	: public dk::C_THREAD
{
private:
	dk::C_TIMER_QP 퍼포먼스타이머;
	DWORD ThreadFunc(LPVOID _pParam);

public:
	moodycamel::BlockingConcurrentQueue<LPSTR> queueFilePaths;
};

DWORD C_FILE_HANDLER::ThreadFunc(LPVOID )
{
	DWORD dwResult = 0;
	C_GAME* pGame = (C_GAME::GetInstance());
	디뷰("C_FILE_HANDLER::ThreadFunc(start)");
	do
	{
		LPSTR pPath = nullptr;
		try
		{
			queueFilePaths.wait_dequeue(pPath);
			if (pPath)
			{
				char szPath[_MAX_PATH] = { 0 };
				strcpy_s(szPath, pPath);

				char szExt[(1 << 5)] = { 0 };
				strcpy_s(szExt, ::strrchr(szPath, '.') + 1);	// 확장자만 따오고
				if (!strcmp(szExt, "txt"))
				{	// 텍스트 파일이면
					dk::C_FILE file(szPath);
					char szLine[(1 << 10)] = { 0 };
					KIWOOM_REALDATA_TRANSACTION data;
					do
					{
						ZeroMemory(szLine, _countof(szLine));
						data.clear();
						if (0 >= file.ReadLine(szLine))
						{
							break;
						}
						LPSTR pLinePtr = &szLine[0];
						//DBGPRINT("%s", pLinePtr);
						pLinePtr += 39;
						memcpy_s(data.szTime, _countof(data.szTime), pLinePtr, 6);				// 시간 복사
						//DBGPRINT("time: %s", data.szTime);

						pLinePtr += (6 + 9);				// " / code: "
						memcpy_s(data.szStockCode, _countof(data.szStockCode), pLinePtr, 6);	// 종목코드 복사
						//DBGPRINT("code: %s", data.szStockCode);

						pLinePtr += (6 + 6);				// ", 시: "
						LPSTR pComma = strchr(pLinePtr, ',');
						size_t nLength = (pComma - pLinePtr);
						memcpy_s(data.szOpen, _countof(data.szOpen), pLinePtr, nLength);
						//DBGPRINT("szOpen: %s", data.szOpen);

						pLinePtr += (nLength + 6);			// ", 고: "
						pComma = strchr(pLinePtr, ',');
						nLength = (pComma - pLinePtr);
						memcpy_s(data.szHigh, _countof(data.szHigh), pLinePtr, nLength);
						//DBGPRINT("szHigh: %s", data.szHigh);

						pLinePtr += (nLength + 6);			// ", 저: "
						pComma = strchr(pLinePtr, ',');
						nLength = (pComma - pLinePtr);
						memcpy_s(data.szLow, _countof(data.szLow), pLinePtr, nLength);
						//DBGPRINT("szLow: %s", data.szLow);

						pLinePtr += (nLength + 6);			// ", 종: "
						pComma = strchr(pLinePtr, ',');
						nLength = (pComma - pLinePtr);
						memcpy_s(data.szClose, _countof(data.szClose), pLinePtr, nLength);
						//DBGPRINT("szClose: %s", data.szClose);

						pLinePtr += (nLength + 10);			// ", 거래량: "
						pComma = strchr(pLinePtr + sizeof(char), ' ');	// 공백이 있을 수 있어서 한칸 더 뒤에서 찾는다.
						nLength = (pComma - pLinePtr);
						memcpy_s(data.szVolume, _countof(data.szVolume), pLinePtr, nLength);
						//DBGPRINT("szVolume: %s", data.szVolume);

						pLinePtr += (nLength + 7);			// " 누적: "
						strcpy_s(data.szAccrueVolume, _countof(data.szAccrueVolume), pLinePtr);
						//DBGPRINT("szAccrueVolume: %s", data.szAccrueVolume);
						/*
						ULONG nVolume = atoi(data.szVolume);
						if (0 == nVolume)
						{
							DBGPRINT(szLine);
							nErrorCount++ ;
						}
						*/
						pGame->PushTickData(&data);
						dwResult++;
					} while (true);
					//DBGPRINT("총 체결 건수: %d", dwResult);
				}
				else if (!strcmp(szExt, "tic") || !strcmp(szExt, "tick"))
				{
					dk::C_FILE file(szPath);
					//KIWOOM_REALDATA_TRANSACTION data;
					LPBYTE pBuffer = file.GetFileData();
					size_t 갯수 = file.GetReadSize() / sizeof(TICK_DATA);
					디뷰("체결틱갯수: %d", 갯수);
					퍼포먼스타이머.시작();
					for (size_t i = 0; i < 갯수; i++)
					{
						LPTICK_DATA pTick = (LPTICK_DATA)pBuffer;
						dk::ntohl(pTick->nTime);
						pGame->PushTickData(pTick);
						pBuffer += sizeof(TICK_DATA);
					}
					디뷰("완료: %0.6f", 퍼포먼스타이머.경과된시간());
				}
				else if (!strcmp(szExt, "bet"))
				{
					dk::C_FILE file(szPath);
					//KIWOOM_REALDATA_TRANSACTION data;
					LPBYTE pBuffer = file.GetFileData();
					size_t 갯수 = file.GetReadSize() / sizeof(TICK_DATA);
					디뷰("체결틱갯수: %d", 갯수);
					퍼포먼스타이머.시작();
					for (size_t i = 0; i < 갯수; i++)
					{
						LPTICK_DATA pTick = (LPTICK_DATA)pBuffer;
						dk::ntohl(pTick->nTime);
						pGame->PushTickData(pTick);
						pBuffer += sizeof(TICK_DATA);
					}
					디뷰("완료: %0.6f", 퍼포먼스타이머.경과된시간());
				}
			}
		}
		catch (...)
		{
			디뷰메시지("Unexpected exception");
		}
		DSAFE_DELETE_ARRAY(pPath);
		dk::멈춰();
	} while (true);
	디뷰("C_FILE_HANDLER::ThreadFunc(end)");
	return(0);
}

static C_FILE_HANDLER* pFileHandler = nullptr;

size_t C_MAIN::DropFile(LPCSTR _pPath)
{
	if (!pFileHandler)
	{
		pFileHandler = new C_FILE_HANDLER();
		pFileHandler->ThreadStart();
	}
	LPSTR pPath = DSAFE_NEW_ARRAY(_MAX_PATH);
	strcpy_s(pPath, _MAX_PATH, _pPath);
	pFileHandler->queueFilePaths.enqueue(pPath);
#if 0
	size_t nResult = 0;

	dk::C_TIMER_QP timerPerformance;
	timerPerformance.Start();
	if (!strcmp(szExt, "txt"))
	{	// 텍스트 파일이면
		dk::C_FILE file(_pPath);
		char szLine[(1 << 10)] = { 0 };
		KIWOOM_REALDATA_TRANSACTION data;
		do
		{
			ZeroMemory(szLine, _countof(szLine));
			data.clear();
			if (0 >= file.ReadLine(szLine))
			{
				break;
			}
			LPSTR pLinePtr = &szLine[0];
			//DBGPRINT("%s", pLinePtr);
			pLinePtr += 39;
			memcpy_s(data.szTime, _countof(data.szTime), pLinePtr, 6);				// 시간 복사
			//DBGPRINT("time: %s", data.szTime);

			pLinePtr += (6 + 9);				// " / code: "
			memcpy_s(data.szStockCode, _countof(data.szStockCode), pLinePtr, 6);	// 종목코드 복사
			//DBGPRINT("code: %s", data.szStockCode);

			pLinePtr += (6 + 6);				// ", 시: "
			LPSTR pComma = strchr(pLinePtr, ',');
			size_t nLength = (pComma - pLinePtr);
			memcpy_s(data.szOpen, _countof(data.szOpen), pLinePtr, nLength);
			//DBGPRINT("szOpen: %s", data.szOpen);

			pLinePtr += (nLength + 6);			// ", 고: "
			pComma = strchr(pLinePtr, ',');
			nLength = (pComma - pLinePtr);
			memcpy_s(data.szHigh, _countof(data.szHigh), pLinePtr, nLength);
			//DBGPRINT("szHigh: %s", data.szHigh);

			pLinePtr += (nLength + 6);			// ", 저: "
			pComma = strchr(pLinePtr, ',');
			nLength = (pComma - pLinePtr);
			memcpy_s(data.szLow, _countof(data.szLow), pLinePtr, nLength);
			//DBGPRINT("szLow: %s", data.szLow);

			pLinePtr += (nLength + 6);			// ", 종: "
			pComma = strchr(pLinePtr, ',');
			nLength = (pComma - pLinePtr);
			memcpy_s(data.szClose, _countof(data.szClose), pLinePtr, nLength);
			//DBGPRINT("szClose: %s", data.szClose);

			pLinePtr += (nLength + 10);			// ", 거래량: "
			pComma = strchr(pLinePtr + sizeof(char), ' ');	// 공백이 있을 수 있어서 한칸 더 뒤에서 찾는다.
			nLength = (pComma - pLinePtr);
			memcpy_s(data.szVolume, _countof(data.szVolume), pLinePtr, nLength);
			//DBGPRINT("szVolume: %s", data.szVolume);

			pLinePtr += (nLength + 7);			// " 누적: "
			strcpy_s(data.szAccrueVolume, _countof(data.szAccrueVolume), pLinePtr);
			//DBGPRINT("szAccrueVolume: %s", data.szAccrueVolume);
			/*
			ULONG nVolume = atoi(data.szVolume);
			if (0 == nVolume)
			{
				DBGPRINT(szLine);
				nErrorCount++ ;
			}
			*/
			pGame->PushTickData(&data);
			nResult++;
		} while (true);
		DBGPRINT("총 체결 건수: %d", nResult);
	}
	
	else if (!strcmp(szExt, "tic"))
	{
		dk::C_FILE file(_pPath);
		//KIWOOM_REALDATA_TRANSACTION data;
		LPBYTE pBuffer = file.GetFileData();
		size_t 갯수 = file.GetReadSize() / sizeof(TICK_DATA);
		디뷰("체결틱갯수: %d", 갯수);
		for (size_t i = 0 ; i < 갯수; i++)
		{
			LPTICK_DATA pTick = (LPTICK_DATA)pBuffer;
			dk::ntohl(pTick->nTime);
			pGame->PushTickData(pTick);
			pBuffer += sizeof(TICK_DATA);
		}
	}
	
	디뷰("C_MAIN::DropFile() - Path: %s / TimeLeft: %0.6f", _pPath, timerPerformance.TimeLeft());
#endif
	return(0);
}