#include "MemoryManager.h"
#include "LuaManager.h"
#include "MongoManager.h"
#include "CliArgs.h"
#include <iostream> //getline
#include <conio.h> //_kbhit

#define LUA_DB_VERSION "0.1.0.0"

int run(int argc, char* argv[]) {
	CliArgs args;
	CliArgs::parse(argc, argv, &args);
	const String uri = MongoManager::getUri(args.host, args.port);

	args.database = "test";

	printf("LuaDB shell version: %s\n", LUA_DB_VERSION);
	if (args.database.size()) {
		printf("Connecting to : %s/%s\n", uri.c_str(), args.database.c_str());
	}
	else {
		printf("Connecting to : %s\n", uri.c_str());
	}

	LuaManager *pLua = LuaManager::getInstance();

	bson_t reply;
	if (!pLua->getMongo()->connect(uri, &reply)) {
		printf("Error: couldn't connect to server, connection attempt failed\n");
		return -1;
	}

	bson_iter_t it;
	if (bson_iter_init_find(&it, &reply, "version")) {
		printf("MongoDB server version: %s\n", bson_iter_utf8(&it, nullptr));
	}
	bson_destroy(&reply);

	if (args.database.size()) {
		pLua->useDatabase(args.database);
	}

	//pLua->loadFile("scripts/performance.lua");

	pLua->loadFile("../test/newtest.lua");

	//pLua->loadFile("scripts/test.lua");
	//pLua->loadFile("scripts/test1.lua");

	//if (args.files.size()) {
	//	for (int i = 0; i < args.files.size(); ++i) {
	//		printf("\nLoading file: %s\n\n", args.files[i].c_str());
	//		pLua->loadFile(args.files[i]);
	//	}
	//	if (args.shell) {
	//		printf("\n");
	//	}
	//	pLua->setIsRunning(args.shell);
	//}

	//if (pLua->isRunning()) {
	//	printf("Type \"help()\" for help\n");
	//	while (pLua->isRunning()) {
	//		printf("> ");
	//		String commandStr;
	//		String inputStr;
	//		if (std::getline(std::cin, inputStr)) {
	//			commandStr += inputStr;
	//			while (_kbhit()) {
	//				printf("... ");
	//				std::getline(std::cin, inputStr);
	//				commandStr += inputStr + "\n";
	//			}
	//		}
	//		if (!commandStr.empty()) {
	//			pLua->loadString(commandStr.c_str());
	//		}
	//	}
	//}

	return 0;
}

int main(int argc, char* argv[]) {
	run(argc, argv);
	LuaManager::destroyInstance();
	#ifdef DEBUG
		if (MemoryManager::getInstance()->hasLeak()) {
			printf("\n");
			MemoryManager::getInstance()->displayInfo();
			getchar();
			return 1;
		}
	#endif
	return 0;
}