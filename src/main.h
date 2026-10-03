#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <commctrl.h>
#include <shellapi.h>
#include <strsafe.h>
#include <stdbool.h>
#include <shellscalingapi.h>
#include "audio_output.h"
#include "resource.h"

#define APP_VERSION L"1.1"
#define ID_ABOUT WM_USER + 1
#define ID_SETTINGS WM_USER + 2
#define ID_EXIT WM_USER + 3

#define DEFAULT_HOTKEY_MODIFIER MOD_SHIFT | MOD_ALT | MOD_NOREPEAT
#define DEFAULT_HOTKEY_KEY VK_UP

typedef struct {
    UINT fsModifiers;
    UINT vk;
} ShortcutConfig;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK SettingsDlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
void AddTrayIcon(HWND hwnd, UINT uID, UINT uCallbackMsg);
void RemoveTrayIcon(HWND hwnd, UINT uID);
void ShowTrayPopup(HWND hWnd);
void OpenAboutBox();
void OpenSettings(HWND hWnd);

void LoadSettings(ShortcutConfig *config);
void SaveSettings(const ShortcutConfig *config);