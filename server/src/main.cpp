#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <thread>
#include <mutex>
#include "Colors.h"
#include "Util.h"

bool g_bQuit = false;

enum COMMANDS {
	DDOS = 0x01
};

void ListenThread();

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

void ListenThread() {

}