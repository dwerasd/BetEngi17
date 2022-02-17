// BetMain.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CMain.h"

#define MAX_LOADSTRING 100

// 전역 변수:
_TCHAR wszTitle[MAX_LOADSTRING] = { 0 };                  // 제목 표시줄 텍스트입니다.
_TCHAR wszWindowClass[MAX_LOADSTRING] = { 0 };            // 기본 창 클래스 이름입니다.

int APIENTRY wWinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
    UNREFERENCED_PARAMETER(_hPrevInstance);
    UNREFERENCED_PARAMETER(_nCmdShow);

    // TODO: 여기에 코드를 입력합니다.
	dk::C_MUTEX mutex;
	if (!wcscmp(_lpCmdLine, L"restart"))
	{	// 자신을 실행하고 종료하는 경우 기다리기 위해.
		do
		{
			if (!mutex.Create(L"DBetMain")) { break; }	// 뮤텍스 생성에 성공하면 빠져나간다.
			dk::멈춰(200);
		} while (true);
	}
	else
	{
		mutex.Create(L"DBetMain");
	}
	if (!mutex.CheckError())
	{
		// 전역 문자열을 초기화합니다.
		::LoadString(_hInstance, IDS_APP_TITLE, wszTitle, MAX_LOADSTRING);
		::LoadString(_hInstance, IDC_BETMAIN, wszWindowClass, MAX_LOADSTRING);

		// 애플리케이션 초기화를 수행합니다:
		C_MAIN* pMain = new C_MAIN(_hInstance);
		if (pMain->Init(wszWindowClass, wszTitle))
		{
			if (pMain->Create())
			{
				::tagMSG msg;
				do
				{
					if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
					if (pMain->Calculate()) { break; }
					pMain->Display();
				} while (WM_QUIT != msg.message);	// WM_DESTROY에서 PostQuitMessage를 호출하면 WM_QUIT가 발생함.
			}
			pMain->Destroy();
		}
	}
	return (0);
}
