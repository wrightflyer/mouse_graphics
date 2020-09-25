// ILI_sim.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "mouse_graphics.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

COLORREF arr_screen[320 * 240];

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MOUSE_GRAPHICS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOUSE_GRAPHICS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MOUSE_GRAPHICS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MOUSE_GRAPHICS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 400, 360, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   int R, G, B;
   for (int n = 0; n < (320 * 240); n++) {
#if 0
       R = rand() & 0xFF;
       G = rand() & 0xFF;
       B = rand() & 0xFF;
#endif
       R = 240;
       G = 240;
       B = 240;
       arr_screen[n] = (B << 16) | (G << 8) | R;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

int mouse_idx;
typedef struct {
    int x;
    int y;
} mouse_pair;

mouse_pair coords[5000];

void clear_mouse_points() {
    for (int i = 0; i < 5000; i++) {
        coords[i].x = 0;
        coords[i].y = 0;
    }
    mouse_idx = 0;
}

int draw_idx;

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC src;
            HDC hdc = BeginPaint(hWnd, &ps);
            HBITMAP map;
            map = CreateBitmap(320, 240, 1, 8 * 4, (void *)arr_screen);
            src = CreateCompatibleDC(hdc);
            SelectObject(src, map);
            BitBlt(hdc, // Destination
                25,  // x and
                25,  // y - upper-left corner of place, where we'd like to copy
                320, // width of the region
                240, // height
                src, // source
                0,   // x and
                0,   // y of upper left corner  of part of the source, from where we'd like to copy
                SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;
            DeleteDC(src);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_MOUSEMOVE:
        if ((wParam & MK_LBUTTON) != 0) {
            coords[mouse_idx].x = LOWORD(lParam);
            coords[mouse_idx].y = HIWORD(lParam);
            // draw as mouse is moved..
            HDC hdc;
            hdc = GetDC(hWnd);
            SelectObject(hdc, GetStockObject(BLACK_BRUSH));
            Ellipse(hdc, coords[mouse_idx].x - 2, coords[mouse_idx].y - 2, coords[mouse_idx].x + 2, coords[mouse_idx].y + 2);
            ReleaseDC(hWnd, hdc);
            mouse_idx++;
            if (mouse_idx >= 4999) {
                clear_mouse_points();
            }
        }
        break;
    case WM_RBUTTONUP:
        // if right clicked start timed redraw...
        InvalidateRect(hWnd, NULL, true);
        SetTimer(hWnd, 1, 8, NULL);
        break;
    case WM_TIMER:
        if (draw_idx < mouse_idx) {
            HDC hdc;
            hdc = GetDC(hWnd);
            SelectObject(hdc, GetStockObject(BLACK_BRUSH));
            Ellipse(hdc, coords[draw_idx].x - 2, coords[draw_idx].y - 2, coords[draw_idx].x + 2, coords[draw_idx].y + 2);
            draw_idx++;
            if (draw_idx >= mouse_idx) {
                KillTimer(hWnd, 1);
                draw_idx = 0;
            }
            ReleaseDC(hWnd, hdc);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
