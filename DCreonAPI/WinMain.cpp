// CreonAPI.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CMain.h"

#define MAX_LOADSTRING 100

// 전역 변수:
char szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
char szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

LRESULT CALLBACK WndProc(HWND _hWnd, UINT _nMessage, WPARAM _wParam, LPARAM _lParam)
{
    static C_MAIN* pMain = nullptr;
    if (pMain)
    {
        if (pMain->OnWindowMessage(_hWnd, _nMessage, _wParam, _lParam))
        {
            return(0);
        }
    }
    switch (_nMessage)
    {
    case WM_CREATE:
        if (!pMain)
        {
            pMain = (C_MAIN*)((::LPCREATESTRUCTA)_lParam)->lpCreateParams;

            //HBRUSH hBrush = ::CreateSolidBrush(RGB(30, 30, 30));
            //::SetClassLongPtr(_hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            //::DeleteObject(hBrush);
        }
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    }
    return DefWindowProcA(_hWnd, _nMessage, _wParam, _lParam);
}

int APIENTRY WinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPSTR _lpCmdLine, _In_ int _nCmdShow)
{
	UNREFERENCED_PARAMETER(_hPrevInstance);
	UNREFERENCED_PARAMETER(_nCmdShow);

	// TODO: 여기에 코드를 입력합니다.
	dk::C_MUTEX mutex;
	if (!strcmp(_lpCmdLine, "restart"))
	{	// 자신을 실행하고 종료하는 경우 기다리기 위해.
		do
		{
			if (!mutex.Create(L"DCreonAPI")) { break; }	// 뮤텍스 생성에 성공하면 빠져나간다.
			dk::멈춰(200);
		} while (true);
	}
	else
	{
		mutex.Create(L"DCreonAPI");
	}
	if (!mutex.CheckError())
	{
		// 전역 문자열을 초기화합니다.
		::LoadStringA(_hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		::LoadStringA(_hInstance, IDC_DCREONAPI, szWindowClass, MAX_LOADSTRING);

		// 애플리케이션 초기화를 수행합니다:
		C_MAIN* pMain = new C_MAIN(_hInstance);
		if (pMain->Init(szWindowClass, szTitle, WndProc))
		{
			if (pMain->Create())
			{
				::tagMSG msg;
				do
				{
					if (::PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
					{
						::TranslateMessage(&msg);
						::DispatchMessageA(&msg);
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