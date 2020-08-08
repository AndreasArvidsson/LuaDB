#pragma once
#include <string>
#include <vector>

class CliArgs {
public:	
	CliArgs(int argc, char* argv[]);

	std::vector<std::string> files;
	std::string host, database;
	int port;
	bool shell, quiet;
};

