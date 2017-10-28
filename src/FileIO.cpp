#include "FileIO.h"
#include <stdio.h>

const bool FileIO::readFile(const String &path, String &result) {
	FILE *pFile = fopen(path.c_str(), "r");
	if (!pFile) {
		return false;
	}
	result = "";
	char c;
	while ((c= fgetc(pFile)) != EOF) {
		result += c;
	}
	fclose(pFile);
	return true;
}