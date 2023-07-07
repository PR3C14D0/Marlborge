#include "HTTPClient.h"

HTTPClient::HTTPClient() {

}

void HTTPClient::GET(std::string url, ADDR_TYPE type) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cout << "[HTTP_ERROR] Error initializing our socket" << std::endl;
		delete this;
		return;
	}

	if (type != IPv4 && type == DNS) {
		// TODO: DNS Support
	}

	std::vector<std::string> ipPort = split(url, ':');
	std::string ip = ipPort[0];
	int port = stoi(ipPort[1]);

	SOCKADDR_IN saIn = { };
	saIn.sin_family = AF_INET;
	saIn.sin_port = htons(port);
	inet_pton(AF_INET, ip.data(), &saIn.sin_addr.s_addr);

	struct timeval tv;
	tv.tv_sec = 2;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	connect(sock, (SOCKADDR*)&saIn, sizeof(saIn));

	std::string httpGet = "GET / HTTP/1.1\r\nHost: " + url + "\r\n\r\n";
	send(sock, httpGet.data(), httpGet.size(), 0);

	char respBuff[1024];
	recv(sock, respBuff, sizeof(respBuff), NULL);

	closesocket(sock);
}