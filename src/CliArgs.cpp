#include "CliArgs.h"
#include <cstring> //strstr
#include "CliOpts.h"

using std::string;

CliArgs::CliArgs(int argc, char* argv[]) {
    const CliOpts opts(argc, argv);

    database = "";
    quiet = opts.hasOpt("quiet");
    shell = opts.hasOpt("shell");
    host = opts.hasOpt("host") ? opts.getOpt("host") : "localhost";
    port = opts.hasOpt("port") ? atoi(opts.getOpt("port").c_str()) : 27017;

    for (const string value : opts.getValues()) {
        if (std::strstr(value.c_str(), ".lua")) {
            files.push_back(value);
        }
        else if (database.empty()) {
            database = value;
        }
    }
}