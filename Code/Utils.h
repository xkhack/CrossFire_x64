#pragma once
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))
namespace Utils {
	PBYTE FindPattern(LPCSTR pattern, LPCSTR mask, HMODULE mod);
	BOOLEAN MaskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask);
	PBYTE FindPattern(PVOID base, DWORD size, LPCSTR pattern, LPCSTR mask);
	BOOL InstallSafeThread(
		__in LPVOID function
	);
}