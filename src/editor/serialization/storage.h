#pragma once
#include "serializable.h"
#include "utils/stringutils.h"
#include <string>
#include <optional>
#include <vector>

class SceneStorage;

class FileName
{
	friend SceneStorage;

public:
	FileName(const std::string& name) : name(StringUtils::Trim(name)) { }
	FileName(const char* name) : name(StringUtils::Trim(name)) { }
	const std::string& Str() const { return name; }
	const char* CStr() const { return name.c_str(); }

private:
	std::string name;
};

inline bool operator==(const FileName& lhs, const FileName& rhs)
{
	return lhs.Str() == rhs.Str();
}

inline bool operator!=(const FileName& lhs, const FileName& rhs)
{
	return !operator==(lhs, rhs);
}

class SceneStorage
{
public:
	SceneStorage(const std::string& sceneDir);

	std::optional<std::string> IsValidFileName(const FileName& fileName) const;
	bool FileExists(const FileName& fileName) const;
	bool SaveFile(const FileName& fileName, const JsonObj& data);
	std::optional<JsonObj> LoadFile(const FileName& fileName);
	const std::vector<FileName>& RecentFiles() const;
	void ForEach(std::function<void(FileName file)> elementCallback) const;

private:
	static inline const std::string fileExtension = ".json";
	static const unsigned int recentLimit = 5;

	const std::string sceneDir;
	std::vector<FileName> recentFiles = {};

	std::string GetRelFilePath(const FileName& fileName) const;
	void UpdateRecentFiles();
};
