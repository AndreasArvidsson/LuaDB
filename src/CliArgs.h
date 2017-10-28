#pragma once
#include "String.h"
#include <vector>

class CliArgs {
public:	
	String host, database;
	int port;
	std::vector<String> files;
	bool shell;

	static void parse(int argc, char* argv[], CliArgs *pArgsOut);

	CliArgs();

};

