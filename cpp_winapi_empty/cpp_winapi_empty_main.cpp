#include <windows.h>
#include <tchar.h>
#include <string>
#include <vector>

#define EDIT1 101
#define BUTTON1 102
#define EDIT2 103
#define EDIT3 104
#define PANEL1 104

//our window class name
const TCHAR CLSNAME[] = TEXT("ourWinClass");

//global identifiers
HWND hwnd; //window variable
HWND edit1, edit2, edit3;
HWND panel1;
HWND button1;

//global vars
std::vector<float> results = {};

//forward declaration of message processor
LRESULT CALLBACK winproc(HWND hwnd, UINT wm, WPARAM wp, LPARAM lp);

float getResMax()
{
	float max = results[0];
	for (float a : results)
	{
		if (a > max) max = a;
	}
	return max;
}

float getResMin()
{
	float min = results[0];
	for (float a : results)
	{
		if (a < min) min = a;
	}
	return min;
}

int APIENTRY WinMain(
	HINSTANCE hInst, 
	HINSTANCE hInstPrev,
	PSTR cmdline,
	int cmdshow
)
{
	WNDCLASSEX wc = { }; //winow class
	MSG msg;             //window message  
	

	wc.cbSize = sizeof(wc);
	wc.style = 0;
	wc.lpfnWndProc = winproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = CLSNAME;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc)) 
	{
		MessageBox(NULL,
			TEXT("Could not register window class"),
			NULL, MB_ICONERROR);
		return 0;
	}

	//створюємо вікно та записуємо у змінну hwnd
	hwnd = CreateWindowEx(
		WS_EX_LEFT,			//розширений стиль вікна
		CLSNAME,			//назва класу вікна
		"Window Name",		//заголовок вікна
		WS_OVERLAPPEDWINDOW,//стиль вікна
		10,					//координата Х лівого верхнього 	
							//кута
		15,					//координата У лівого верхніого 
							//кута
		800,				//ширина
		600,				//висота
		NULL,				//"батьківське" вікно
		NULL,				//індекс меню HMENU
		hInst,				//посилання на інстанс додатку
		NULL);				//параметр повідомлення WM_CREATE
	if (!hwnd) {
		MessageBox(NULL, TEXT("Could not create window"),
			NULL, MB_ICONERROR);
		return 0;
	}

	edit1 = CreateWindowEx(
		WS_EX_LEFT, "EDIT", "2",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
		10, 15, 200, 30,
		hwnd, (HMENU)EDIT1, hInst, NULL);

	edit2 = CreateWindowEx(
		WS_EX_LEFT, "EDIT", "2",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
		10, 50, 200, 30,
		hwnd, (HMENU)EDIT2, hInst, NULL);

	edit3 = CreateWindowEx(
		WS_EX_LEFT, "EDIT", "?",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | WS_BORDER,
		10, 85, 200, 30,
		hwnd, (HMENU)EDIT3, hInst, NULL);

	panel1 = CreateWindowEx(
		WS_EX_LEFT, "STATIC", NULL,
		WS_VISIBLE | WS_CHILD | WS_BORDER,
		10, 120, 610, 420,
		hwnd, (HMENU)PANEL1, hInst, NULL);

	button1 = CreateWindowEx(
		WS_EX_LEFT, "BUTTON", "Show",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		220, 50, 100, 30,
		hwnd, (HMENU)BUTTON1, hInst, NULL);


	ShowWindow(hwnd, cmdshow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK winproc(HWND hwnd, UINT wm, WPARAM wp,	LPARAM lp)
{
	switch (wm)
	{
	case WM_COMMAND:
	{
		if (LOWORD(wp) == BUTTON1)
		{
			char ed1txt[2048] = "";
			char ed2txt[2048] = "";

			GetWindowText(edit1, ed1txt, 2048);
			GetWindowText(edit2, ed2txt, 2048);

			float v1 = atof(ed1txt);
			float v2 = atof(ed2txt);

			float res = v1 + v2;
			results.push_back(res);
			
			std::string res_str = "Result = " + std::to_string(res);

			SetWindowText(edit3,res_str.c_str());

			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		break;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(panel1, &ps);

		// All painting occurs here, between BeginPaint and EndPaint.
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		if (results.size() > 1)
		{
			float max = getResMax();
			float min = getResMin();

			RECT pan1rect;
			GetClientRect(panel1, &pan1rect);

			int pan1w = (pan1rect.right - pan1rect.left);
			int pan1h = (pan1rect.bottom - pan1rect.top);

			int n = results.size();
			POINT * rpoints = new POINT[n];
			for (int i = 0; i < results.size(); i++)
			{
				int cx = (pan1w / n)*i;
				int cy = trunc((pan1h/(max-min))*(results[i]-min));
				rpoints[i].x = cx;
				rpoints[i].y = pan1h - cy;
			}
			Polyline(hdc, rpoints, results.size());
			delete(rpoints);
		}		
		EndPaint(hwnd, &ps);
		break;
	}

	case WM_CLOSE: {
		DestroyWindow(hwnd);
		break;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}


	default: return DefWindowProc(hwnd, wm, wp, lp);
	}
}
