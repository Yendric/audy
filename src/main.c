#include "main.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    WNDCLASS wc = {
        .lpfnWndProc = WindowProc,
        .hInstance = hInstance,
        .lpszClassName = L"Audy",
    };
    RegisterClass(&wc);

    /**
     * Creates a window, which we won't be showing to the user.
     * It is however needed in order to register hotkeys and show a system tray icon.
     */
    HWND hWnd = CreateWindowEx(0, L"Audy",
                               L"Audy",
                               WS_OVERLAPPEDWINDOW,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               NULL,
                               NULL,
                               hInstance,
                               NULL);

    if (hWnd == NULL)
        return 1;

    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    RegisterHotKey(hWnd, 1, HOTKEY_MODIFIER, HOTKEY_KEY);

    // Message loop
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

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
        HRESULT hr = setNextAudioDeviceAsDefault();
        if (FAILED(hr))
            MessageBox(NULL, L"Failed to change audio device.", L"Error", MB_OK | MB_ICONERROR);
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AddTrayIcon(HWND hWnd, UINT uID, UINT uCallbackMsg)
{
    NOTIFYICONDATA nid = {
        .cbSize = sizeof(nid),
        .hWnd = hWnd,
        .uID = uID,
        .uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
        .uCallbackMessage = uCallbackMsg,
        .hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_AUDY_ICON)),
    };

    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Audy");

    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RemoveTrayIcon(HWND hWnd, UINT uID)
{
    NOTIFYICONDATA nid = {
        .hWnd = hWnd,
        .uID = uID,
    };
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

void ShowTrayPopup(HWND hWnd)
{
    HMENU hPop = CreatePopupMenu();

    InsertMenu(hPop, 0, MF_BYPOSITION | MF_STRING, ID_ABOUT, L"About Audy");
    InsertMenu(hPop, 1, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");

    // The default item is shown in bold
    SetMenuDefaultItem(hPop, ID_ABOUT, FALSE);
    SetFocus(hWnd);
    SendMessage(hWnd, WM_INITMENUPOPUP, (WPARAM)hPop, 0);

    // Get cursor position in order to show popup menu at said position
    POINT curpos;
    GetCursorPos(&curpos);

    WORD cmd = TrackPopupMenu(hPop, TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, curpos.x, curpos.y, 0, hWnd, NULL);
    SendMessage(hWnd, WM_COMMAND, cmd, 0);
}

void OpenAboutBox()
{
    MSGBOXPARAMS mbp = {
        .cbSize = sizeof(MSGBOXPARAMS),
        .hwndOwner = NULL,
        .hInstance = GetModuleHandle(NULL),
        .lpszText = L"Audy is a Win32 application that enables users to modify their default audio output device using a keyboard shortcut."
                    L"\n\nAudy, version " APP_VERSION
                    L"\nCopyright © 2023 Yendric Van Roey",
        .lpszCaption = L"About Audy",
        .dwStyle = MB_USERICON | MB_OK,
        .dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        .lpfnMsgBoxCallback = NULL,
        .dwContextHelpId = 0,
        .lpszIcon = MAKEINTRESOURCE(IDI_AUDY_ICON),
    };

    MessageBoxIndirect(&mbp);
}