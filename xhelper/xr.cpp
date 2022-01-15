#include "include/xhelper.h"
#include <filesystem>
#include <random>

#pragma warning(disable : 4996)

XHELPER_API const char* getDocPath() {
	std::string alt = ".\\video";
	char* hd = getenv("HOMEDRIVE"),
		* hp = getenv("HOMEPATH");
	if (
		hd != nullptr &&
		hp != nullptr) {
		alt = std::string(hd) + "\\" + std::string(hp)
			+ "\\" + alt;
	}
retry:
	if (!std::filesystem::exists(alt.c_str())) {
		std::filesystem::create_directory(alt.c_str());
	}
	else if (!std::filesystem::is_directory(alt)) {
		alt += std::string(rndst(3));
		goto retry;
	}
	return alt.c_str();
}


XHELPER_API const char* rndst(int r)
{
	std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

	std::random_device rd;
	std::mt19937 generator(rd());

	std::shuffle(str.begin(), str.end(), generator);

	return str.substr(0, r).c_str();    // assumes 32 < number of characters in str         
}
