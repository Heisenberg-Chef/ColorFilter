#include <windows.h>
#define TIMER 11

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int cxClient, cyClient, cx, cy;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("MyWindows");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndclass;
	HACCEL hAccel;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("这个程序需要在 Windows NT 才能执行！"), szAppName, MB_ICONERROR);
		return 0;
	}

					//hwnd = CreateWindow(szAppName,
					//	TEXT("C+V to cpy"),
					//	WS_POPUPWINDOW|WS_VISIBLE|WS_CAPTION,
					//	0,
					//	0,
					//	0,
					//	0,
					//	NULL,
					//	NULL,
					//	hInstance,
					//	szCmdLine);
	hwnd = CreateWindowEx(WS_EX_TOPMOST,
		szAppName,
		TEXT("F1 -> cpy"),
		WS_POPUPWINDOW | WS_VISIBLE | WS_CAPTION,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		hInstance,
		szCmdLine);
	hAccel = LoadAccelerators(hInstance, szAppName);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT __stdcall WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndEdit;
	static HBRUSH hBrush;
	static HDC hdc, hdcScreen;
	POINT pt;
	static COLORREF cr, crLast;
	static TCHAR str[20],clip[20];
	static PAINTSTRUCT ps;
	static RECT rect;
	HGLOBAL hGlobal;
	PTSTR pGlobal;
	switch (message)
	{
	case WM_CREATE:
		cxClient = GetSystemMetrics(SM_CXSCREEN);
		cyClient = GetSystemMetrics(SM_CYSCREEN);
		MoveWindow(hwnd, cxClient * 3 / 4, cyClient / 8, cxClient / 8, cyClient / 6, TRUE);
		SetTimer(hwnd, TIMER, 50, NULL);
		//hdcScreen = CreateDC(TEXT("display"), NULL, NULL, NULL);
		break;
	case WM_SIZE:
		cx = LOWORD(lParam);
		cy = HIWORD(lParam);
		break;
	case WM_TIMER:
		hdcScreen = CreateDC(TEXT("display"), NULL, NULL, NULL);
		GetCursorPos(&pt);
		cr = GetPixel(hdcScreen, pt.x, pt.y);
		if (cr != crLast)
		{
			crLast = cr;
			InvalidateRect(hwnd, NULL, TRUE);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);
		
		hBrush = CreateSolidBrush(cr);
		FillRect(hdc, &rect, hBrush);
		wsprintf(str, TEXT("R:%02x G:%02x B:%02x"),
			GetRValue(cr), GetGValue(cr), GetBValue(cr));
		
		SetBkColor(hdc, cr);
		
		if (GetRValue(cr) + GetGValue(cr) + GetBValue(cr) > 127 + 127 + 127)
		{
			SetTextColor(hdc, RGB(0, 0, 0));
		}
		else
		{
			SetTextColor(hdc, RGB(255, 255, 255));
		}
		DrawText(hdc, str, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		return 0; 
	case WM_KEYDOWN:
		if(wParam == VK_F1)
		{
			hGlobal = GlobalAlloc(GHND, (lstrlen(str) + 1) * sizeof(TCHAR));
			pGlobal = GlobalLock(hGlobal);
			wsprintf(str, TEXT("R:%02x G:%02x B:%02x---># %02x %02x %02x"),
				GetRValue(cr), GetGValue(cr), GetBValue(cr),GetRValue(cr), GetGValue(cr), GetBValue(cr));;
			lstrcpy(pGlobal, clip);
			GlobalUnlock(hGlobal);
			OpenClipboard(hwnd);
			EmptyClipboard();
			SetClipboardData(CF_TEXT, hGlobal);
			CloseClipboard();
			GlobalFree(hGlobal);
		}
		break;
	case WM_DESTROY:
		KillTimer(hwnd, TIMER);
		DeleteDC(hdc);
		PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}
