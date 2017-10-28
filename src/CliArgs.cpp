#include "CliArgs.h"
#include <cstring> //strstr

void CliArgs::parse(int argc, char* argv[], CliArgs *pArgsOut) {
	bool findDb = true;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--host") == 0) {
			pArgsOut->host = argv[i + 1];
			++i;
		}
		else if (strcmp(argv[i], "--port") == 0) {
			pArgsOut->port = atoi(argv[i + 1]);
			++i;
		}
		else if (strcmp(argv[i], "--shell") == 0) {
			pArgsOut->shell = true;
		}
		else if (findDb && !std::strstr(argv[i], ".lua")) {
			pArgsOut->database = argv[i];
		}
		else if (std::strstr(argv[i], ".lua")){
			pArgsOut->files.push_back(argv[i]);
		}
	}
}

CliArgs::CliArgs() {
	host = "localhost";
	port = 27017;
	database = "";
	shell = false;
}
