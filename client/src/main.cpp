#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <libloaderapi.h>

void GetPID(char* procName, DWORD* dwpPID);

int main() {
	char procName[] = "mspaint.exe"; // Change this to the process you want to attach.
	const char* dllName = "MarlborgeDLL.dll";
	char dllPath[MAX_PATH];
	DWORD dwPID = 0;

	GetPID(procName, &dwPID);
	GetFullPathName(dllName, sizeof(dllPath), dllPath, nullptr);

	if (dwPID == 0) {
		std::cout << "[ERROR] Process not found" << std::endl;
		return 1;
	}

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

	LPVOID lpvAddr = VirtualAllocEx(hProc, nullptr, sizeof(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if (lpvAddr == nullptr) {
		std::cout << "[ERROR] Error allocating to process memory" << std::endl;
		return 1;
	}

	if (!WriteProcessMemory(hProc, lpvAddr, dllPath, sizeof(dllPath), nullptr)) {
		std::cout << "[ERROR] Error writing to process memory" << std::endl;
		return 1;
	}

	HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibrary, lpvAddr, NULL, nullptr);
	
	std::cout << "[SUCCESS] Injected to " << procName << std::endl;

	if (hThread)
		CloseHandle(hThread);

	if (hProc)
		CloseHandle(hProc);

	return 0;
}

void GetPID(char* procName, DWORD* dwpPID) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);
	
	if (Process32First(hSnap, &pe)) {
		do {
			if (strcmp(pe.szExeFile, procName) == 0) {
				*dwpPID = pe.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &pe));
	}
	return;
}