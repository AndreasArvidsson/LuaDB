#include "File.h"

#ifdef _WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
#endif

namespace fs = std::experimental::filesystem::v1;

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

const bool File::exists() const {
    return fs::exists(_path);
}

const bool File::isDirectory() const {
    return fs::is_directory(_path);
}

const bool File::isFile() const {
    return !isDirectory();
}

const size_t File::getSize() const {
    return fs::file_size(_path);
}

const time_t File::getLastWriteTime() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(fs::last_write_time(_path).time_since_epoch()).count();
}

std::vector<std::string> File::getFileNames() const {
    std::vector<std::string> fileNames;
    for (auto &entry : fs::directory_iterator(_path)) {
        //fileNames.push_back(toString(entry.path()));
        fileNames.push_back(toString(entry.path().c_str()));
    }
    return fileNames;
}

std::vector<File> File::getFiles() const {
    std::vector<File> files;
    for (auto &entry : fs::directory_iterator(_path)) {
        files.push_back(File(entry.path()));
    }
    return files;
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

void File::rename(const std::string &newName) {
    move(getParentPath() + "/" + newName);
}

void File::move(const std::string &newPath) {
    fs::rename(_path, newPath);
    _path = newPath;
}

File File::copy(const std::string &newPath) const {
    fs::copy_file(_path, newPath);
    return File(newPath);
}

const bool File::remove() const {
    return fs::remove_all(_path);
}

const std::string File::toString(const std::wstring &wstring) const {
    return std::string(wstring.begin(), wstring.end());
}

const std::string File::toString(const char *str) const {
    return std::string(str);
}

