#include "stdafx.h"
#include "prga.h"

HINSTANCE hInst;
TCHAR *szTitle = _T("crunchSnpr");
TCHAR *szWindowClass = _T("CRUNCHSNAPER");
TCHAR *szWindowClassL = _T("CRUNCHSNAPERL");

HWND hLayerWnd;

// x, y axis
int ofX, ofY;
// Prototypes

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	LayerWndProc(HWND, UINT, WPARAM, LPARAM);

int					GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

BOOL				isPng(LPCTSTR fileName);
VOID				drawRubberband(HDC hdc, LPRECT newRect, BOOL erase);
VOID				execUrl(const char* str);
VOID				setClipBoardText(const char* str);
BOOL				convertPNG(LPCTSTR destFile, LPCTSTR srcFile);
BOOL				savePNG(LPCTSTR fileName, HBITMAP newBMP);
BOOL				uploadFile(HWND hwnd, LPCTSTR fileName);
std::string			getId();
BOOL				saveId(const WCHAR* str);

// Entry points

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;

	TCHAR	szThisPath[MAX_PATH];
	DWORD   sLen;
	// get directory
	sLen = GetModuleFileName(NULL, szThisPath, MAX_PATH);
	for (unsigned int i = sLen; i >= 0; i--) {
		if (szThisPath[i] == _T('\\')) {
			szThisPath[i] = _T('\0');
			break;
		}
	}
	SetCurrentDirectory(szThisPath);
	// if there's no file specified (arg)
	if ( 2 == __argc )
	{
		if (isPng(__targv[1])){
			// exit upload
			uploadFile(NULL, __targv[1]);
		}
		else {
			TCHAR tmpDir[MAX_PATH], tmpFile[MAX_PATH];
			GetTempPath(MAX_PATH, tmpDir);
			GetTempFileName(tmpDir, _T("cru"), 0, tmpFile);

			if (convertPNG(tmpFile, __targv[1])) {
				uploadFile(NULL, tmpFile);
			}
			else {
				// check in the case it can't conver the file
				MessageBox(NULL, _T("Cannot conver image."), szTitle, MB_OK | MB_ICONERRROR);
			}
			DeleteFile(tmpFile);

			MyRegisterClass(hInstance);
			// initializiation
			if (!InitInstance(hInstance, nCmdShow)){
				return FALSE;
			}
			while (GetMessage(&msg, NULL, 0, 0)){
				TransLateMessage(&msg);
				DispatchMessage(&msg);
			}
			return (int)msg.wParam;
		}

		// check if it's PNG
		BOOL isPng(LPCTSTR fileName)
		{
			unsigned char pngHead[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
			unsigned char readHead[8];

			FILE *fp = NULL;

			if (0 != _tfopen_s(&fp, fileName, _T("rb")) ||
				8 != fread(readHead, 1, 8, fp)) {
					return FALSE;
			}
			fclose(fp);

			// compare
			for (unsigned int i = 0; i<8; i++)
				if (pngHead[i] != readHead[i]) return FALSE;

			return TRUE;

		}
		// registering window class...
		ATOM MyRegisterClass(HINSTANCE hInstance)
		{
			WNDCLASS wc;

			// MAIN WINDOW
			wc.style = 0;							// dont send
			wc.lpfnWndProc = WndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CRUNCHSNPR));
			wc.hCursor = LoadCursor(NULL, IDC_CROSS);	// Cursor
			wc.hbrBackground = 0;							// no background
			wc.lpszMenuName = 0;
			wc.lpszClassName = szWindowClass;

			RegisterClass(&wc);

			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = LayerWndProc;
			wc.cbClsExtra = 0;
			wc.cbWndExtra = 0;
			wc.hInstance = hInstance;
			wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GYAZOWIN));
			wc.hCursor = LoadCursor(NULL, IDC_CROSS);	// loading cursor
			wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName = 0;
			wc.lpszClassName = szWindowClassL;

			return RegisterClass(&wc);
		}
