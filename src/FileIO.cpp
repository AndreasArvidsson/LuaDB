#include "FileIO.h"
#include <stdio.h>

const String FileIO::readFile(const String &path) {
	String str;
	FILE * pFile;
	pFile = fopen(path.c_str(), "r");
	char c;
	while ((c= fgetc(pFile)) != EOF) {
		str += c;
	}
	fclose(pFile);
	return str;
}