#include "HTTPClient.h"

HTTPClient::HTTPClient(bool bSSL) {
	this->m_bSSL = bSSL;
	
	if (this->m_bSSL) {
		SSL_load_error_strings();
		SSL_library_init();
		OpenSSL_add_all_algorithms();
	}
}

void HTTPClient::GET(std::string url, ADDR_TYPE type) {
	SSL_CTX* ssl_ctx = nullptr;
	SSL* ssl = nullptr;

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

	if (this->m_bSSL) {
		const SSL_METHOD* ssl_method = TLSv1_2_client_method();
		ssl_ctx = SSL_CTX_new(ssl_method);
		ssl = SSL_new(ssl_ctx);
		SSL_set_fd(ssl, sock);
		int nRes = SSL_connect(ssl);
		if (nRes != 1) {
			char err[2048];
			ERR_error_string(ERR_get_error(), err);
			std::cout << err << std::endl;
			std::cout << "[ERROR] An error occurred during the SSL/TLS handshake." << std::endl;
			SSL_free(ssl);
			SSL_CTX_free(ssl_ctx);
			delete this;
			return;
		}
	}

	srand(time(nullptr));
	std::string user_agent = user_agents[rand() % ((sizeof(user_agents) / sizeof(std::string)) - 1)];
	std::string referer = referers[rand() % ((sizeof(referers) / sizeof(std::string)) - 1)];
	std::string httpGet = 
		"GET / HTTP/1.1\r\n: "					
		"Host: " + url + "\r\n"
		"User-Agent: " + user_agent + "\r\n"
		"Referer: " + referer + "\r\n\r\n";

	char respBuff[1024];
	if (this->m_bSSL) {
			SSL_write(ssl, httpGet.data(), httpGet.size());
			SSL_read(ssl, respBuff, sizeof(respBuff));
	}
	else {
		send(sock, httpGet.data(), httpGet.size(), 0);
		recv(sock, respBuff, sizeof(respBuff), NULL);
	}

	closesocket(sock);
	if (this->m_bSSL) {
		SSL_free(ssl);
		SSL_CTX_free(ssl_ctx);
	}
}