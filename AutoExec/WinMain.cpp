// AutoExec.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CMain.h"



int APIENTRY WinMain(_In_ HINSTANCE hInstance , _In_opt_ HINSTANCE hPrevInstance , _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);
	UNREFERENCED_PARAMETER(hInstance);

    // TODO: 여기에 코드를 입력합니다.
	char 설정파일[_MAX_PATH] = { 0 };
	::GetModuleFileNameA(::GetModuleHandleW(0), 설정파일, 배열크기(설정파일));
	for (size_t i = 0; i < ::strlen(설정파일); i++) { if ('\\' == 설정파일[i]) { 설정파일[i] = '/'; } }
	설정파일[::strlen(설정파일) - 3] = 0;
	strcat_s(설정파일, "ini");
	디뷰("설정파일: %s", 설정파일);
	if (1 == dk::파일체크(설정파일))
	{
		dk::PROCESS_INFOMATION_EX 프로세스;
		::GetPrivateProfileStringA("file", "path", "", 프로세스.szPath, 배열크기(프로세스.szPath), 설정파일);
		char 감시할파일명[(1 << 5)] = { 0 };
		strcpy_s(감시할파일명, ::strrchr(프로세스.szPath, '/') + 1);	// 파일명만 따오고
		디뷰("감시대상: %s / %s", 프로세스.szPath, 감시할파일명);
		tagMSG msg = { 0 };
		do
		{
			if (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);	// 문자 메시지로 변환을 하고
				DispatchMessageA(&msg);	// 콜백 함수로 보낸다
			}
			dk::DLOCAL_TIME currentTime;
			if (!(currentTime.wSecond % 5))
			{	// 여긴 5초에 한번씩 들어온다.
				DWORD 프로세스아이디 = dk::GetProcessOf(감시할파일명);
				if (!프로세스아이디)
				{
					dk::파일실행(프로세스.szPath, 프로세스.szParam, true, (::LPPROCESS_INFORMATION)&프로세스);
					프로세스아이디 = 프로세스.dwProcessId;
				}
			}
			dk::멈춰(10);
		} while (WM_QUIT != msg.message);	// WM_DESTROY에서 PostQuitMessage를 호출하면 WM_QUIT가 발생함.
	}
	else
	{
		디뷰(L"설정파일이 없음: %s", 설정파일);
	}
    return (0);
}
