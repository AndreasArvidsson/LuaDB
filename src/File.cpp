#include "File.h"

#ifdef _WIN32
#include <filesystem>
namespace fs = std::experimental::filesystem : v1;
#else
#include <sys/stat.h>
#endif

File::File() {
}

File::File(const File &file) {
    _path = file._path;
}

File::File(const std::string &path) {
    _path = path;
}

File::File(const char *path) {
    _path = path;
}

File::File(const std::wstring &path) {
    _path = toString(path);
}

File::File(const wchar_t *path) {
    _path = toString(path);
}

const std::string File::getPath() const {
    return _path;
}

const std::string File::getParentPath() const {
    return _path.substr(0, _path.find_last_of("/\\"));
}

const std::string File::getName() const {
    return _path.substr(_path.find_last_of("/\\") + 1);
}

const std::string File::getShortName() const {
    size_t index = _path.find_last_of("/\\") + 1;
    return _path.substr(index, _path.find_last_of(".") - index);
}

const std::string File::getExtension() const {
    return _path.substr(_path.find_last_of("."));
}

const bool File::getData(std::string *result) const {
    FILE *pFile = fopen(_path.c_str(), "r");
    if (!pFile) {
        return false;
    }
    *result = "";
    char c;
    while ((c = fgetc(pFile)) != EOF) {
        *result += c;
    }
    fclose(pFile);
    return true;
}

const std::string File::toString(const std::wstring &wstring) const {
    return std::string(wstring.begin(), wstring.end());
}

const std::string File::toString(const char *str) const {
    return std::string(str);
}

const bool File::exists() const {
#ifdef _WIN32
    return fs::exists(_path);
#else
    struct stat buffer;
    return (stat(_path.c_str(), &buffer) == 0);
#endif
}