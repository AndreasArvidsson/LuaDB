#pragma once
#include <string>
#include <vector>

class File {
public:
    File();
    File(const File &file);
    File(const std::string &path);
    File(const char *path);
    File(const std::wstring &path);
    File(const wchar_t *path);

    const std::string getPath() const;
    const std::string getParentPath() const;
    const std::string getName() const;
    const std::string getShortName() const;
    const std::string getExtension() const;
    const bool getData(std::string *result) const;
    const bool exists() const;

private:
    std::string _path;

    const std::string toString(const std::wstring &wstring) const;
    const std::string toString(const char *str) const;

};