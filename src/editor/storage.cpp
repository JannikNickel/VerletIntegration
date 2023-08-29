#include "storage.h"
#include <algorithm>
#include <format>
#include <filesystem>
#include <fstream>

static const char forbiddenFsChars[] = { '\\', '/', ':', '*', '?', '"', '<', '>', '|', '\0', '.' };

SceneStorage::SceneStorage(const std::string& sceneDir) : sceneDir(sceneDir.ends_with('/') ? sceneDir : (sceneDir + '/'))
{
	try
	{
		std::filesystem::create_directory(this->sceneDir);
	}
	catch(const std::exception&) { }
}

std::optional<std::string> SceneStorage::IsValidFileName(const FileName& fileName) const
{
	for(size_t i = 0; i < fileName.name.size(); i++)
	{
		if(std::find(std::begin(forbiddenFsChars), std::end(forbiddenFsChars), fileName.name[i]) != std::end(forbiddenFsChars))
		{
			if(fileName.name[i] == '.')
			{
				return std::make_optional("File name can't contain a file extension!");
			}
			return std::make_optional(std::format("File name contains invalid character '{0}'!", fileName.name[i]));
		}
	}
	if(fileName.name.size() == 0)
	{
		return std::make_optional("File name can't be empty!");
	}
	return std::nullopt;
}

bool SceneStorage::FileExists(const FileName& fileName) const
{
	return std::filesystem::exists(GetRelFilePath(fileName));
}

bool SceneStorage::SaveFile(const FileName& fileName, const JsonObj& data)
{
	const std::string path = GetRelFilePath(fileName);
	std::ofstream out = std::ofstream(path);
	if(!out.is_open())
	{
		return false;
	}
	out << data.dump(4);
	out.close();
	return !out.fail();
}

std::string SceneStorage::GetRelFilePath(const FileName& fileName) const
{
	return sceneDir + fileName.name + fileExtension;
}
