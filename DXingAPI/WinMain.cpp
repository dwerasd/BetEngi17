// XingAPI.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "CMain.h"



#define MAX_LOADSTRING 100

// 전역 변수:
char szTitle[MAX_LOADSTRING] = { 0 };                  // 제목 표시줄 텍스트입니다.
char szWindowClass[MAX_LOADSTRING] = { 0 };            // 기본 창 클래스 이름입니다.


int APIENTRY WinMain(
	_In_ HINSTANCE _hInstance
	, _In_opt_ HINSTANCE _hPrevInstance
	, _In_ LPTSTR _lpCmdLine
	, _In_ int _nCmdShow
)
{
	UNREFERENCED_PARAMETER(_hPrevInstance);
	UNREFERENCED_PARAMETER(_nCmdShow);

	// TODO: 여기에 코드를 입력합니다.
	dk::C_MUTEX mutex;
	if (!strcmp(_lpCmdLine, "restart"))
	{	// 자신을 실행하고 종료하는 경우 기다리기 위해.
		do
		{
			if (!mutex.Create(L"DXingAPI")) { break; }	// 뮤텍스 생성에 성공하면 빠져나간다.
			dk::멈춰(200);
		} while (true);
	}
	else
	{
		mutex.Create(L"DXingAPI");
	}
	if (!mutex.CheckError())
	{
		// 전역 문자열을 초기화합니다.
		::LoadStringA(_hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		::LoadStringA(_hInstance, IDC_XINGAPI, szWindowClass, MAX_LOADSTRING);

		//wchar_t szCommLib[MAX_PATH] = { 0 };
		//dk::GetCurrentDirectoryW(szCommLib);
		//DBGPRINT(L"_tWinMain() CurrentDirectory - %s", szCommLib);

		C_MAIN* pMain = new C_MAIN(_hInstance);
		if (false != pMain->Init(szWindowClass, szTitle))
		{
			if (false != pMain->Create())
			{
				tagMSG msg;
				do
				{
					if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
						//if (GetMessage(&msg, NULL, 0, 0))
					{
						TranslateMessage(&msg);	// 문자 메시지로 변환을 하고
						DispatchMessage(&msg);	// 콜백 함수로 보낸다
					}
					if (pMain->Calculate()) { break; }
					pMain->Display();
				} while (msg.message != WM_QUIT);	// WM_DESTROY에서 PostQuitMessage를 호출하면 WM_QUIT가 발생함.
				DBGPRINT("_tWinMain() - DestroyWindow 호출 전");
				::DestroyWindow(msg.hwnd);
			}
			pMain->Destroy();
		}
		//DSAFE_DELETE(pMain);
	}
	return (0);
}
