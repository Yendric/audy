#define WIN32_LEAN_AND_MEAN
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

    INITCOMMONCONTROLSEX icex = {
        .dwSize = sizeof(INITCOMMONCONTROLSEX),
        .dwICC = ICC_HOTKEY_CLASS
    };
    InitCommonControlsEx(&icex);

    ShortcutConfig config;
    LoadSettings(&config);
    if (!RegisterHotKey(hWnd, 1, config.fsModifiers, config.vk))
    {
        MessageBox(NULL, L"Failed to register hotkey. It might be in use by another application.", L"Audy Error", MB_OK | MB_ICONERROR);
    }

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
        case ID_SETTINGS:
            OpenSettings(hWnd);
            return 0;
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
    InsertMenu(hPop, 1, MF_BYPOSITION | MF_STRING, ID_SETTINGS, L"Settings...");
    InsertMenu(hPop, 2, MF_BYPOSITION | MF_STRING, ID_EXIT, L"Exit");

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

void LoadSettings(ShortcutConfig *config)
{
    HKEY hKey;
    config->fsModifiers = DEFAULT_HOTKEY_MODIFIER;
    config->vk = DEFAULT_HOTKEY_KEY;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Audy", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD dwSize = sizeof(DWORD);
        RegQueryValueEx(hKey, L"Modifiers", NULL, NULL, (LPBYTE)&config->fsModifiers, &dwSize);
        RegQueryValueEx(hKey, L"Key", NULL, NULL, (LPBYTE)&config->vk, &dwSize);
        RegCloseKey(hKey);
    }
}

void SaveSettings(const ShortcutConfig *config)
{
    HKEY hKey;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Audy", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS)
    {
        RegSetValueEx(hKey, L"Modifiers", 0, REG_DWORD, (const BYTE *)&config->fsModifiers, sizeof(DWORD));
        RegSetValueEx(hKey, L"Key", 0, REG_DWORD, (const BYTE *)&config->vk, sizeof(DWORD));
        RegCloseKey(hKey);
    }
}

void OpenSettings(HWND hWnd)
{
    DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS), hWnd, SettingsDlgProc);
}

static bool IsExtendedKey(WORD vk)
{
    switch (vk)
    {
    case VK_UP: case VK_DOWN: case VK_LEFT: case VK_RIGHT:
    case VK_HOME: case VK_END: case VK_PRIOR: case VK_NEXT:
    case VK_INSERT: case VK_DELETE: case VK_DIVIDE:
        return true;
    default:
        return false;
    }
}

INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        ShortcutConfig config;
        LoadSettings(&config);

        // Convert RegisterHotKey modifiers to HOTKEY_CLASS modifiers
        WORD hkModifiers = 0;
        if (config.fsModifiers & MOD_ALT) hkModifiers |= HOTKEYF_ALT;
        if (config.fsModifiers & MOD_CONTROL) hkModifiers |= HOTKEYF_CONTROL;
        if (config.fsModifiers & MOD_SHIFT) hkModifiers |= HOTKEYF_SHIFT;
        
        // Add extended flag for display purposes (arrows, etc.)
        if (IsExtendedKey((WORD)config.vk)) hkModifiers |= HOTKEYF_EXT;

        SendDlgItemMessage(hDlg, IDC_HOTKEY, HKM_SETHOTKEY, MAKEWORD(config.vk, hkModifiers), 0);
        
        // Focus OK button so it doesn't immediately capture a keypress (like the one used to open the menu)
        SetFocus(GetDlgItem(hDlg, IDOK));
        return (INT_PTR)FALSE;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            LRESULT result = SendDlgItemMessage(hDlg, IDC_HOTKEY, HKM_GETHOTKEY, 0, 0);
            WORD vk = LOBYTE(LOWORD(result));
            WORD hkModifiers = HIBYTE(LOWORD(result));

            ShortcutConfig config;
            config.vk = vk;
            config.fsModifiers = MOD_NOREPEAT;
            if (hkModifiers & HOTKEYF_ALT) config.fsModifiers |= MOD_ALT;
            if (hkModifiers & HOTKEYF_CONTROL) config.fsModifiers |= MOD_CONTROL;
            if (hkModifiers & HOTKEYF_SHIFT) config.fsModifiers |= MOD_SHIFT;

            SaveSettings(&config);

            // Re-register hotkey
            HWND hWndMain = GetParent(hDlg);
            UnregisterHotKey(hWndMain, 1);
            if (!RegisterHotKey(hWndMain, 1, config.fsModifiers, config.vk))
            {
                MessageBox(hDlg, L"Failed to register hotkey. It might be in use by another application.", L"Error", MB_OK | MB_ICONERROR);
            }

            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}