#pragma once
#include <iostream>
#include <vector>

inline std::vector<std::string> split(std::string sData, char delimiter) {
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