#include <Windows.h>
#include "Installer.h"

void SendKey(WORD vk, bool keydown) {
	INPUT generated = {};
	generated.type = INPUT_KEYBOARD;
	generated.ki.wVk = vk;
	generated.ki.wScan = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
	generated.ki.dwFlags = keydown ? 0 : KEYEVENTF_KEYUP;
	SendInput(1, &generated, sizeof(INPUT));
}

LRESULT CALLBACK keyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		PKBDLLHOOKSTRUCT data = (PKBDLLHOOKSTRUCT)lParam;
		bool keydown = !(data->flags & LLKHF_UP);
		if (data->vkCode == VK_RWIN) {
			SendKey(VK_LSHIFT, keydown);
			SendKey(VK_LCONTROL, keydown);
			SendKey(VK_LMENU, keydown);
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char* argv[])
{
	if (argc == 2) {
		if (strcmp(argv[1], "/Install") == 0 || strcmp(argv[1], "/install") == 0) {
			return scheduleTask(true, argv[0]);
		}
		if (strcmp(argv[1], "/Uninstall") == 0 || strcmp(argv[1], "/uninstall") == 0) {
			return scheduleTask(false);
		}
	}

	HHOOK hhkKeyboard = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHook, NULL, 0);

	HWND hwnd = CreateWindow(L"STATIC", NULL, 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, NULL, NULL);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hhkKeyboard);
	
	return (int)msg.wParam;
}