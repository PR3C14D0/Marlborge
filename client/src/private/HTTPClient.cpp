#include "HTTPClient.h"

HTTPClient::HTTPClient(bool bSSL) {
	this->m_bSSL = bSSL;
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

	srand(time(nullptr));
	std::string user_agent = user_agents[rand() % ((sizeof(user_agents) / sizeof(std::string)) - 1)];
	std::string referer = referers[rand() % ((sizeof(referers) / sizeof(std::string)) - 1)];
	std::string httpGet =
		"GET / HTTP/1.1\r\n: "
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Accept-Language: en-US,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\n"
		"Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Charset: iso-8859-1\r\nAccept-Encoding: gzip\r\n"
		"Accept: application/xml,application/xhtml+xml,text/html;q=0.9, text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Charset: iso-8859-1\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: br;q=1.0, gzip;q=0.8, *;q=0.1\r\nAccept-Language: utf-8, iso-8859-1;q=0.5, *;q=0.1\r\nAccept-Charset: utf-8, iso-8859-1;q=0.5\r\n"
		"Accept: image/jpeg, application/x-ms-application, image/gif, application/xaml+xml, image/pjpeg, application/x-ms-xbap, application/x-shockwave-flash, application/msword, */*\r\nAccept-Language: en-US,en;q=0.5\r\n"
		"Accept: text/html, application/xhtml+xml, image/jxr, */*\r\nAccept-Encoding: gzip\r\nAccept-Charset: utf-8, iso-8859-1;q=0.5\r\nAccept-Language: utf-8, iso-8859-1;q=0.5, *;q=0.1\r\n"
		"Accept: text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/webp, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1\r\nAccept-Encoding: gzip\r\nAccept-Language: en-US,en;q=0.5\r\nAccept-Charset: utf-8, iso-8859-1;q=0.5\r\n,"
		"Accept: text/html, application/xhtml+xml, application/xml;q=0.9, */*;q=0.8\r\nAccept-Language: en-US,en;q=0.5\r\n"
		"Accept-Charset: utf-8, iso-8859-1;q=0.5\r\nAccept-Language: utf-8, iso-8859-1;q=0.5, *;q=0.1\r\n"
		"Accept: text/html, application/xhtml+xml"
		"Accept-Language: en-US,en;q=0.5\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Encoding: br;q=1.0, gzip;q=0.8, *;q=0.1\r\n"
		"Accept: text/plain;q=0.8,image/png,*/*;q=0.5\r\nAccept-Charset: iso-8859-1\r\n"
		"Referer: " + referer + "\r\n\r\n"
		"Host: " + url + "\r\n"
		"User-Agent: " + user_agent + "\r\n";
		

	send(sock, httpGet.data(), httpGet.size(), 0);

	char respBuff[1024];
	if (recv(sock, respBuff, sizeof(respBuff), NULL) == WSAETIMEDOUT)
		printf("socket timed out\n");
	
	delete respBuff;
	closesocket(sock);
}