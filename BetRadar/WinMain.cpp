// BetRadar.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CMain.h"

#define MAX_LOADSTRING 100

// 전역 변수:
//HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

int APIENTRY wWinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPWSTR _lpCmdLine, _In_ int _nCmdShow)
{
	UNREFERENCED_PARAMETER(_hInstance);
	UNREFERENCED_PARAMETER(_hPrevInstance);
	UNREFERENCED_PARAMETER(_lpCmdLine);
	UNREFERENCED_PARAMETER(_nCmdShow);

	// TODO: 여기에 코드를 입력합니다.
	dk::C_MUTEX mutex;
	if (!wcscmp(_lpCmdLine, L"restart"))
	{	// 자신을 실행하고 종료하는 경우 기다리기 위해.
		do
		{
			if (!mutex.Create(L"BetRadar")) { break; }	// 뮤텍스 생성에 성공하면 빠져나간다.
			dk::멈춰(200);
		} while (true);
	}
	else
	{
		mutex.Create(L"BetRadar");
	}
	try
	{
		if (!mutex.CheckError())
		{
			// 전역 문자열을 초기화합니다.
			::LoadStringW(_hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
			::LoadStringW(_hInstance, IDC_BETRADAR, szWindowClass, MAX_LOADSTRING);
			// 애플리케이션 초기화를 수행합니다:
			C_MAIN* pMain = new C_MAIN(_hInstance);
			if (pMain->Init(szWindowClass, szTitle))
			{
				if (pMain->Create())
				{
					::tagMSG msg;
					do
					{
						if (::PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
						{
							::TranslateMessage(&msg);
							::DispatchMessageW(&msg);
						}
						if (pMain->Calculate()) { break; }
						pMain->Display();
					} while (WM_QUIT != msg.message);	// WM_DESTROY에서 PostQuitMessage를 호출하면 WM_QUIT가 발생함.
				}
				pMain->Destroy();
			}
		}
	}
	catch (...)
	{
		디뷰메시지("Unexpected exception");
	}
	return(0);
}
