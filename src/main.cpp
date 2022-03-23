#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "strsafe.h"
#include "AudioHandler.h"
#include "resource.h"

#define ID_ABOUT WM_USER + 1
#define ID_EXIT WM_USER + 2
#define HOTKEY MOD_SHIFT | MOD_ALT | MOD_NOREPEAT

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddTrayIcon(HWND hwnd, UINT uID, UINT uCallbackMsg);
void RemoveTrayIcon(HWND hwnd, UINT uID);
void ShowTrayPopup(HWND hWnd);
void OpenAboutBox();

/*
 * Application entry point
 */
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Window class registreren
    const wchar_t CLASS_NAME[] = L"Audy";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // (verborgen) Window maken
    HWND hWnd = CreateWindowEx(
        0,                   // Window stjl
        CLASS_NAME,          // Window class
        L"Audy",             // Window titel
        WS_OVERLAPPEDWINDOW, // Window stjl

        // Plaats en positie
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,
        NULL,
        hInstance,
        NULL);

    if (hWnd == NULL)
    {
        return 0;
    }

    // Registreer hotkey
    RegisterHotKey(hWnd, 1, MOD_SHIFT | MOD_ALT | MOD_NOREPEAT, VK_UP);

    // Message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

/*
 * Message handler
 */
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        AddTrayIcon(hWnd, 1, WM_APP);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_QUIT:
        RemoveTrayIcon(hWnd, 1);
    case WM_APP:
        switch (lParam)
        {
        case WM_RBUTTONUP:
            ShowTrayPopup(hWnd);
            return 0;
        case WM_LBUTTONDBLCLK:
            OpenAboutBox();
            return 0;
        }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_ABOUT:
            OpenAboutBox();
            return 0;
        case ID_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return 0;
        }
        return 0;
    case WM_HOTKEY:
        setNextAudioDeviceAsDefault();
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*
/ Voegt icon toe aan system tray
*/
void AddTrayIcon(HWND hWnd, UINT uID, UINT uCallbackMsg)
{
    NOTIFYICONDATA nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = uID;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = uCallbackMsg;
    nid.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_AUDY_ICON));

    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Audy");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

/*
/ Verwijdert icon uit system tray
*/
void RemoveTrayIcon(HWND hWnd, UINT uID)
{
    NOTIFYICONDATA nid;
    nid.hWnd = hWnd;
    nid.uID = uID;

    Shell_NotifyIcon(NIM_DELETE, &nid);
}

/*
/ Toont context menu voor shell icon
*/
void ShowTrayPopup(HWND hWnd)
{
    // Maak popup menu
    HMENU hPop = CreatePopupMenu();
    // Voeg items toe, met hun messages
    InsertMenu(hPop, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, L"Over...");
    InsertMenu(hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Sluit");

    // Default item is in vet
    SetMenuDefaultItem(hPop, ID_ABOUT, FALSE);
    SetFocus(hWnd);
    SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);

    // Cursorlocatie
    POINT curpos;
    GetCursorPos(&curpos);

    WORD cmd = TrackPopupMenu(hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, curpos.x, curpos.y, 0, hWnd, NULL);
    SendMessage(hWnd, WM_COMMAND, cmd, 0);
}

void OpenAboutBox()
{
    MessageBox(
        NULL,
        L"Audy, versie 1.0.\nCopyright (c) 2021 Yendric Van Roey",
        L"Audy",
        MB_ICONINFORMATION | MB_OK);
}