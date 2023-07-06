/*
	Simple HTTP Client that doesn't check for the server response. Only makes GETs.
*/

#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Util.h"
#include <string>
#include <errno.h>

enum ADDR_TYPE {
	IPv4 = 0,
	DNS = 1
};

class HTTPClient {
public:
	HTTPClient();
	void GET(std::string url, ADDR_TYPE type);
};