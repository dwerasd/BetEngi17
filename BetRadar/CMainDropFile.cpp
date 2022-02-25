#include "framework.h"
#include "CMain.h"



LPBYTE pStickBuffer = (LPBYTE)힙할당((sizeof(STICKS_HEADER) + ((sizeof(STICK_DATAF) * 500)) * 3000));
LPBYTE pStickBufferPtr = pStickBuffer;
size_t 스틱등록된종목수 = 0;
class C_FILE_HANDLER
	: public dk::C_THREAD
{
private:
	dk::C_TIMER_QP 퍼포먼스타이머;
	DWORD ThreadFunc(LPVOID _pParam);

public:
	moodycamel::BlockingConcurrentQueue<LPSTR> queueFilePaths;
};

DWORD C_FILE_HANDLER::ThreadFunc(LPVOID)
{
	DWORD dwResult = 0;
	C_ENGINE* pEngine = (C_ENGINE::GetInstance());
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
				::strcpy_s(szPath, pPath);

				char szExt[(1 << 5)] = { 0 };
				::strcpy_s(szExt, ::strrchr(szPath, '.') + 1);	// 확장자만 따오고
				if (!::strcmp(szExt, "txt"))
				{	// 텍스트 파일이면
					
					//DBGPRINT("총 체결 건수: %d", dwResult);
				}
				else if (!::strcmp(szExt, "tic") || !::strcmp(szExt, "tick"))
				{
					퍼포먼스타이머.시작();
					dk::C_FILE file(szPath);
					//KIWOOM_REALDATA_TRANSACTION data;
					LPBYTE pBuffer = file.GetFileData();
					size_t 갯수 = file.GetReadSize() / sizeof(TICK_DATAEX);
					디뷰("체결틱갯수: %d", 갯수);
					for (size_t i = 0; i < 갯수; i++)
					{	// 읽을때 endian 변환된걸 복구해서 메모리에 올린다.
						LPTICK_DATAEX pTick = (LPTICK_DATAEX)pBuffer;
						dk::ntohl(pTick->nTime);

						pEngine->PushTickData(pTick);
						pBuffer += sizeof(TICK_DATAEX);
					}
					디뷰("완료: %0.6f", 퍼포먼스타이머.경과된시간());
				}
				else if (!strcmp(szExt, "bet"))
				{	// 여기서는 기록만 한다.
					dk::C_FILE file(szPath);
					//KIWOOM_REALDATA_TRANSACTION data;
					퍼포먼스타이머.시작();
					LPBYTE pBuffer = file.GetFileData();
					베트파일헤더포 헤더포 = (베트파일헤더포)file.GetFileData();

					LPSTICKS_HEADER pHeader = (LPSTICKS_HEADER)pStickBufferPtr;
					pHeader->nStickType = 헤더포->nStickType;
					pHeader->nStickLength = 헤더포->nStickLength;
					pHeader->nMaxSticks = 500;
					pHeader->nCountSticks = DMIN(헤더포->nCountSticks, 500);
					strcpy_s(pHeader->크레온코드, _countof(pHeader->크레온코드), 헤더포->크레온코드);
					pStickBufferPtr += sizeof(STICKS_HEADER);

					// 읽은 헤더포 증가
					pBuffer += 0x1000;

					for (size_t i = 0; i < pHeader->nCountSticks; i++)
					{	// 스틱을 채우고
						LPSTICK_DATAF pStickData = (LPSTICK_DATAF)pStickBufferPtr;
						LPBET_STICK_EX 기존스틱포 = (LPBET_STICK_EX)pBuffer;
						pStickData->nDate = dk::ntohl(기존스틱포->nDate);
						pStickData->nTime = dk::ntohl(기존스틱포->nTime);

						//디뷰("(%d/%d)", pStickData->nDate, pStickData->nTime);
						pStickBufferPtr += sizeof(STICK_DATAF);
						pBuffer += sizeof(BET_STICK_EX);
					}
					// 다 채웠으면 포인터를 해당 스틱의 처음으로 이동한 다음
					pStickBufferPtr = (LPBYTE)pHeader;
					// 한번에 건너띄기 한다.
					pStickBufferPtr += (sizeof(STICKS_HEADER) + (sizeof(STICK_DATAF) * 500));

					file.Destroy();
					스틱등록된종목수++;
					//디뷰("완료: %0.6f", 퍼포먼스타이머.경과된시간());
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
	힙해제(pStickBuffer);
	디뷰("C_FILE_HANDLER::ThreadFunc(end)");
	return(dwResult);
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
	return(0);
}
