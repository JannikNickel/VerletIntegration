#pragma once
#include <string>
#include <optional>
#include "serialization/serializable.h"
#include "utils/stringutils.h"

class SceneStorage;

class FileName
{
	friend SceneStorage;

public:
	FileName(const std::string& name) : name(StringUtils::Trim(name)) { }
	FileName(const char* name) : name(StringUtils::Trim(name)) { }
	const char* CStr() { return name.c_str(); }

private:
	std::string name;
};

class SceneStorage
{
public:
	SceneStorage(const std::string& sceneDir);

	std::optional<std::string> IsValidFileName(const FileName& fileName) const;
	bool FileExists(const FileName& fileName) const;
	bool SaveFile(const FileName& fileName, const JsonObj& data);

private:
	static inline const std::string fileExtension = ".json";
	const std::string sceneDir;

	std::string GetRelFilePath(const FileName& fileName) const;
};
