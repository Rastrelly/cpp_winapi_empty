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
HWND edit1, edit2, edit3; //text blocks
HWND panel1; //drawing panel
HWND button1;//execution button

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

	//create a window and write to hwnd variable
	hwnd = CreateWindowEx(
		WS_EX_LEFT,			//window style extended
		CLSNAME,			//window class name
		"Window Name",		//window header
		WS_OVERLAPPEDWINDOW,//window style
		10,					//top left corner X coord 	
		15,					//top left corner Y coord
		800,				//width
		600,				//height
		NULL,				//parent window
		NULL,				//HMENU index
		hInst,				//instance ref
		NULL);				//WM_CREATE parameter
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
	case WM_COMMAND: //fires when command action executed
		//(like moyuse click over a button)
	{
		if (LOWORD(wp) == BUTTON1) //wp defines HMENU index 
									//of WM_COMMAND recipient
		{
			char ed1txt[2048] = "";
			char ed2txt[2048] = "";

			//extract text data of window
			GetWindowText(edit1, ed1txt, 2048);
			GetWindowText(edit2, ed2txt, 2048);

			float v1 = atof(ed1txt);
			float v2 = atof(ed2txt);

			float res = v1 + v2;
			results.push_back(res);
			
			std::string res_str = "Result = " + std::to_string(res);

			//redefines text of a window
			SetWindowText(edit3,res_str.c_str());

			//forces WM_PAINT message execution
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
		break;
	}

	case WM_PAINT: //fires when the window needs to be redrawn
		//(mostly on resizes, but we force it in WM_COMMAND)
	{
		PAINTSTRUCT ps;
		//creates draw context and starts painting mode for it
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

		//destroys paint context
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
