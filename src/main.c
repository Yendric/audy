#include <windows.h>
#include <objbase.h>
#include <commctrl.h>
#include <shellapi.h>
#include "audio_output.h"
#include "resource.h"

#define APP_NAME L"Audy"
#define REG_KEY L"Software\\Audy"
#define HOTKEY_ID 1
#define TRAY_ID 1
#define WM_TRAY WM_APP

typedef struct
{
    DWORD modifiers;
    DWORD vk;
} Shortcut;

static const Shortcut defaultShortcut = {MOD_SHIFT | MOD_ALT, VK_UP};

static UINT taskbarCreatedMsg;

static void ReadDword(LPCWSTR name, DWORD *value)
{
    DWORD size = sizeof(*value);
    RegGetValue(HKEY_CURRENT_USER, REG_KEY, name, RRF_RT_REG_DWORD, NULL, value, &size);
}

static Shortcut LoadShortcut(void)
{
    Shortcut s = defaultShortcut;
    ReadDword(L"Modifiers", &s.modifiers);
    ReadDword(L"Key", &s.vk);
    return s;
}

static void SaveShortcut(Shortcut s)
{
    RegSetKeyValue(HKEY_CURRENT_USER, REG_KEY, L"Modifiers", REG_DWORD, &s.modifiers, sizeof(DWORD));
    RegSetKeyValue(HKEY_CURRENT_USER, REG_KEY, L"Key", REG_DWORD, &s.vk, sizeof(DWORD));
}

static void RegisterShortcut(HWND hWnd, Shortcut s)
{
    UnregisterHotKey(hWnd, HOTKEY_ID);
    if (!RegisterHotKey(hWnd, HOTKEY_ID, s.modifiers | MOD_NOREPEAT, s.vk))
        MessageBox(hWnd, L"Failed to register hotkey. It might be in use by another application.", APP_NAME, MB_OK | MB_ICONERROR);
}

static void AddTrayIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = {
        .cbSize = sizeof(nid),
        .hWnd = hWnd,
        .uID = TRAY_ID,
        .uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP,
        .uCallbackMessage = WM_TRAY,
        .hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_AUDY_ICON)),
        .szTip = APP_NAME,
    };
    Shell_NotifyIcon(NIM_ADD, &nid);
}

static void RemoveTrayIcon(HWND hWnd)
{
    NOTIFYICONDATA nid = {
        .cbSize = sizeof(nid),
        .hWnd = hWnd,
        .uID = TRAY_ID,
    };
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

static void ShowTrayMenu(HWND hWnd)
{
    HMENU menu = CreatePopupMenu();
    AppendMenu(menu, MF_STRING, ID_ABOUT, L"About Audy");
    AppendMenu(menu, MF_STRING, ID_SETTINGS, L"Settings...");
    AppendMenu(menu, MF_STRING, ID_EXIT, L"Exit");
    SetMenuDefaultItem(menu, ID_ABOUT, FALSE);

    POINT cursor;
    GetCursorPos(&cursor);

    // Without this the menu doesn't close when clicking elsewhere
    SetForegroundWindow(hWnd);
    TrackPopupMenu(menu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
    DestroyMenu(menu);
}

static void ShowAbout(void)
{
    MSGBOXPARAMS mbp = {
        .cbSize = sizeof(mbp),
        .hInstance = GetModuleHandle(NULL),
        .lpszText = L"Audy is a Win32 application that enables users to modify their default audio output device using a keyboard shortcut."
                    L"\n\nAudy, version " VERSION_STRING
                    L"\nCopyright © 2023 Yendric Van Roey",
        .lpszCaption = L"About Audy",
        .dwStyle = MB_USERICON | MB_OK,
        .dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        .lpszIcon = MAKEINTRESOURCE(IDI_AUDY_ICON),
    };
    MessageBoxIndirect(&mbp);
}

static BOOL IsExtendedKey(DWORD vk)
{
    switch (vk)
    {
    case VK_UP: case VK_DOWN: case VK_LEFT: case VK_RIGHT:
    case VK_HOME: case VK_END: case VK_PRIOR: case VK_NEXT:
    case VK_INSERT: case VK_DELETE: case VK_DIVIDE:
        return TRUE;
    default:
        return FALSE;
    }
}

// The hotkey control uses HOTKEYF_* flags, RegisterHotKey uses MOD_* flags
static BYTE ToHotkeyFlags(Shortcut s)
{
    return ((s.modifiers & MOD_ALT) ? HOTKEYF_ALT : 0) |
           ((s.modifiers & MOD_CONTROL) ? HOTKEYF_CONTROL : 0) |
           ((s.modifiers & MOD_SHIFT) ? HOTKEYF_SHIFT : 0) |
           (IsExtendedKey(s.vk) ? HOTKEYF_EXT : 0);
}

static DWORD FromHotkeyFlags(BYTE flags)
{
    return ((flags & HOTKEYF_ALT) ? MOD_ALT : 0) |
           ((flags & HOTKEYF_CONTROL) ? MOD_CONTROL : 0) |
           ((flags & HOTKEYF_SHIFT) ? MOD_SHIFT : 0);
}

static INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Shortcut *s = (Shortcut *)GetWindowLongPtr(hDlg, DWLP_USER);

    switch (uMsg)
    {
    case WM_INITDIALOG:
        s = (Shortcut *)lParam;
        SetWindowLongPtr(hDlg, DWLP_USER, lParam);
        SendDlgItemMessage(hDlg, IDC_HOTKEY, HKM_SETHOTKEY, MAKEWORD(s->vk, ToHotkeyFlags(*s)), 0);

        // Focus OK so the hotkey control doesn't capture the keypress used to open the menu
        SetFocus(GetDlgItem(hDlg, IDOK));
        return FALSE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK)
        {
            WORD hotkey = (WORD)SendDlgItemMessage(hDlg, IDC_HOTKEY, HKM_GETHOTKEY, 0, 0);
            s->vk = LOBYTE(hotkey);
            s->modifiers = FromHotkeyFlags(HIBYTE(hotkey));
        }
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

static void OpenSettings(HWND hWnd)
{
    INITCOMMONCONTROLSEX icex = {
        .dwSize = sizeof(icex),
        .dwICC = ICC_HOTKEY_CLASS,
    };
    InitCommonControlsEx(&icex);

    Shortcut s = LoadShortcut();
    if (DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS), hWnd, SettingsDlgProc, (LPARAM)&s) == IDOK)
    {
        SaveShortcut(s);
        RegisterShortcut(hWnd, s);
    }
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        AddTrayIcon(hWnd);
        return 0;

    case WM_DESTROY:
        RemoveTrayIcon(hWnd);
        PostQuitMessage(0);
        return 0;

    case WM_TRAY:
        if (lParam == WM_RBUTTONUP)
            ShowTrayMenu(hWnd);
        else if (lParam == WM_LBUTTONDBLCLK)
            ShowAbout();
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_ABOUT:
            ShowAbout();
            break;
        case ID_SETTINGS:
            OpenSettings(hWnd);
            break;
        case ID_EXIT:
            DestroyWindow(hWnd);
            break;
        }
        return 0;

    case WM_HOTKEY:
        if (FAILED(CycleAudioOutput()))
            MessageBox(hWnd, L"Failed to change audio device.", APP_NAME, MB_OK | MB_ICONERROR);
        return 0;
    }

    // Explorer restarted, so the tray icon is gone
    if (uMsg == taskbarCreatedMsg)
    {
        AddTrayIcon(hWnd);
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    (void)hPrevInstance, (void)lpCmdLine, (void)nShowCmd;

    if (FAILED(CoInitialize(NULL)))
        return 1;

    taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

    WNDCLASS wc = {
        .lpfnWndProc = WindowProc,
        .hInstance = hInstance,
        .lpszClassName = APP_NAME,
    };
    RegisterClass(&wc);

    // Never shown, but needed to receive hotkey and tray icon messages
    HWND hWnd = CreateWindow(APP_NAME, APP_NAME, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    if (!hWnd)
        return 1;

    RegisterShortcut(hWnd, LoadShortcut());

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    CoUninitialize();
    return 0;
}
