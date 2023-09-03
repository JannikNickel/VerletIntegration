#include "storage.h"
#include <algorithm>
#include <format>
#include <filesystem>
#include <fstream>
#include <queue>

static const char forbiddenFsChars[] = { '\\', '/', ':', '*', '?', '"', '<', '>', '|', '\0', '.' };

SceneStorage::SceneStorage(const std::string& sceneDir) : sceneDir(sceneDir.ends_with('/') ? sceneDir : (sceneDir + '/'))
{
	try
	{
		std::filesystem::create_directory(this->sceneDir);
	}
	catch(const std::exception&) {}

	UpdateRecentFiles();
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

	UpdateRecentFiles();
	return !out.fail();
}

std::optional<JsonObj> SceneStorage::LoadFile(const FileName& fileName)
{
	const std::string path = GetRelFilePath(fileName);
	std::ifstream in = std::ifstream(path);
	if(!in.is_open())
	{
		return std::nullopt;
	}

	JsonObj json = JsonObj::parse(in);
	in.close();
	return !in.fail() ? std::make_optional(json) : std::nullopt;
}

const std::vector<FileName>& SceneStorage::RecentFiles() const
{
	return recentFiles;
}

FileName FileNameFromPath(const std::filesystem::path& path)
{
	std::filesystem::path p = path.filename();
	p.replace_extension();
	return p.string();
}

void SceneStorage::ForEach(std::function<void(FileName file)> elementCallback) const
{
	for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(sceneDir))
	{
		if(!entry.is_regular_file())
		{
			continue;
		}
		elementCallback(FileNameFromPath(entry.path()));
	}
}

std::string SceneStorage::GetRelFilePath(const FileName& fileName) const
{
	return sceneDir + fileName.name + fileExtension;
}

struct CmpFileLastWrite
{
	bool operator()(const std::filesystem::directory_entry& a, const std::filesystem::directory_entry& b)
	{
		return a.last_write_time() < b.last_write_time();
	}
};

void SceneStorage::UpdateRecentFiles()
{
	std::priority_queue<std::filesystem::directory_entry, std::vector<std::filesystem::directory_entry>, CmpFileLastWrite> best = {};
	for(const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(sceneDir))
	{
		if(!entry.is_regular_file())
		{
			continue;
		}
		best.push(entry);
		if(best.size() > recentLimit)
		{
			best.pop();
		}
	}

	recentFiles.clear();
	while(!best.empty())
	{
		recentFiles.push_back(FileNameFromPath(best.top().path()));
		best.pop();
	}
}
