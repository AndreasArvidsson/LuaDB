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
	const bool exists() const;
	const bool isDirectory() const;
	const bool isFile() const;
	const size_t getSize() const;
	const time_t getLastWriteTime() const;

	std::vector<std::string> getFileNames() const;
	std::vector<File> getFiles() const;
	const bool getData(std::string *result) const;

	void rename(const std::string &newName);
	void move(const std::string &newPath);
	File copy(const std::string &newPath) const;
	const bool remove() const;
	
private:
	std::string  _path;

	const std::string toString(const std::wstring &wstring) const;

};