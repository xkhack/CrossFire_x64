#include "stdafx.h"

namespace Utils {
	BOOLEAN MaskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask) {
		for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b) {
			if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b) {
				return FALSE;
			}
		}

		return TRUE;
	}
	PBYTE FindPattern(PVOID base, DWORD size, LPCSTR pattern, LPCSTR mask) {
		size -= static_cast<DWORD>(strlen(mask));

		for (auto i = 0UL; i < size; ++i) {
			auto addr = reinterpret_cast<PBYTE>(base) + i;
			if (MaskCompare(addr, pattern, mask)) {
				return addr;
			}
		}

		return NULL;
	}
	PBYTE FindPattern(LPCSTR pattern, LPCSTR mask, HMODULE mod) {
		MODULEINFO info = { 0 };
		if (mod == NULL) {
			mod = GetModuleHandle(0);
		}

		GetModuleInformation(GetCurrentProcess(), mod, &info, sizeof(info));

		return FindPattern(info.lpBaseOfDll, info.SizeOfImage, pattern, mask);
	}

	BOOL InstallSafeThread(
		__in LPVOID function
	) {

		HMODULE hModule = GetModuleHandleA(0);

		HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetCurrentProcessId());
		if (hSnapshot == INVALID_HANDLE_VALUE)
			return FALSE;
		THREADENTRY32 te32 = { 0,0,0,0,0,0,0 };
		te32.dwSize = sizeof(THREADENTRY32);
		if (!Thread32First(hSnapshot, &te32))
		{
			CloseHandle(hSnapshot);
			return FALSE;
		}
		do
		{
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, te32.th32ThreadID);
			if (hThread)
			{
				DWORD_PTR dwStartAddress = 0;
				if (NT_SUCCESS(NtQueryInformationThread(hThread, static_cast<THREADINFOCLASS>(9), &dwStartAddress, sizeof(DWORD_PTR), NULL)))
				{
					PIMAGE_DOS_HEADER DosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(hModule);
					PIMAGE_NT_HEADERS NtHeader = reinterpret_cast<PIMAGE_NT_HEADERS>((reinterpret_cast<DWORD_PTR>(hModule) + DosHeader->e_lfanew));
					if (!NtHeader || NtHeader->Signature != IMAGE_NT_SIGNATURE)
					{
						SetLastError(1793);
						return FALSE;
					}
					if (dwStartAddress > reinterpret_cast<DWORD_PTR>(hModule) &&
						dwStartAddress < (reinterpret_cast<DWORD_PTR>(hModule + 0x1000) + NtHeader->OptionalHeader.SizeOfImage))
					{
						//MessageBoxA(0, 0, 0, 0);
						_MEMORY_BASIC_INFORMATION mbi;
						CONTEXT ctx;
						DWORD Old = 0;
						ZeroMemory(&mbi, sizeof(mbi));
						if (!VirtualQuery(reinterpret_cast<LPCVOID>(dwStartAddress), &mbi, sizeof(mbi)))
						{
							CloseHandle(hSnapshot);
							CloseHandle(hThread);
							return FALSE;
						}
						if (!VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &Old))
						{
							CloseHandle(hSnapshot);
							CloseHandle(hThread);
							return FALSE;

						}
						HANDLE Thread = CreateRemoteThread(GetCurrentProcess(), 0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(dwStartAddress), 0, CREATE_SUSPENDED, 0);
						if (Thread == 0 || Thread == INVALID_HANDLE_VALUE)
						{
							TerminateThread(Thread, 0);
							CloseHandle(Thread);
							CloseHandle(hSnapshot);
							CloseHandle(hThread);
							return FALSE;
						}
						ctx.ContextFlags = CONTEXT_ALL;
						if (!GetThreadContext(Thread, &ctx))
						{
							TerminateThread(Thread, 0);
							CloseHandle(Thread);
							CloseHandle(hSnapshot);
							CloseHandle(hThread);
							return FALSE;
						}
#ifdef _WIN64
						ctx.Rcx = reinterpret_cast<DWORD64>(function);
#else
						ctx.Eax = reinterpret_cast<DWORD>(function);
#endif

						if (!SetThreadContext(Thread, &ctx))
						{
							TerminateThread(Thread, 0);
							CloseHandle(Thread);
							CloseHandle(hSnapshot);
							CloseHandle(hThread);
							return FALSE;
						}
						ResumeThread(Thread);
						CloseHandle(Thread);
						CloseHandle(hSnapshot);
						CloseHandle(hThread);
						return TRUE;
					}

				}
				CloseHandle(hThread);
			}
		} while (Thread32Next(hSnapshot, &te32));
		CloseHandle(hSnapshot);
		return FALSE;

	}
}
