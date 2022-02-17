#include "framework.h"
#include "CMain.h"


#if defined(_USE_WIN32_GUI_)
void btnEnableHandler(int code, void *extra)
{
	디뷰("눌림");
}

void C_MAIN::CreateWin32Gui()
{
	if (!pLabel1)
	{
		pLabel1 = new dk::C_WIN32_LABEL(L"Nova Proxy Switcher v1.1", this);
		pLabel1->setPosition(0, 0);
		pLabel1->setSize(300, 30);

		pLabel1->setFont(L"Arial", 20, true);
	}
	if (!pButton1)
	{
		pButton1 = new dk::C_WIN32_BUTTON(L"버튼", this);
		pButton1->setPosition(300, 0);
		pButton1->setSize(100, 30);
		pButton1->pCommandHandler = (dk::CommandHandler_PROC)&btnEnableHandler;
	}
	hListboxStockCode = CreateWindowExW(0, L"listbox", NULL, WS_CHILD | WS_VISIBLE | LBS_STANDARD, 280, 260, 200, 80, hWnd, (HMENU)ID_LIST, hInst, NULL);


}
#endif