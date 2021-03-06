#include <filesystem>

#include "pch.h"
#include "LevelEntity.h"
#include "ObjectParser.h"
const char extMAP[] = ".map";
const char extNAV[] = ".nav";

struct Level_t
{
	std::string levelName;
	std::string levelDescription;
	std::uint8_t levelMisson;
	std::string levelMissonText;

	struct {
		float navScalePerNeuron;
	}level_terrain2D;

};

vector<Level_t> m_levels;

using namespace RoninEngine;

void RoninEngine::Levels::Level_Init() {

	if (!m_levels.empty())
		throw std::bad_exception();

	ObjectParser parser;
	Level_t levl;
	ObjectNode* node;
	filesystem::directory_iterator dirIter(dataAt(FolderKind::LEVELS));

	for (auto file : dirIter) {
		if (!file.is_directory() && file.path().extension() == extMAP) {
			//load map
			parser.Deserialize(file.path().generic_string().c_str());
			node = parser.FindNode("map/level_name");
			if (node)
				levl.levelName = node->toString();
			else
				levl.levelName = file.path().filename().generic_string();

			node = parser.FindNode("map/level_description");
			if (node)
				levl.levelDescription = node->toString();

			node = parser.FindNode("map/level_mission");
			if (node)
				levl.levelMisson = static_cast<decltype(levl.levelMisson)>(node->toNumber());
			else
				levl.levelMisson = static_cast<decltype(levl.levelMisson)>(m_levels.size()+1);

			node = parser.FindNode("map/level_mission_text");
			if (node)
				levl.levelMissonText = node->toString();

			node = parser.FindNode("map/level_terrain2D/navScalePerNeuron");
			if (node)
				levl.level_terrain2D.navScalePerNeuron = static_cast<float>(node->toReal());

			m_levels.emplace_back(levl);
			levl = {};
		}
	}
}

int RoninEngine::Levels::Load_Level(string levelName, Terrain2D* terrain2d) {

	int id = -1;
	for (auto& v : m_levels) {
	}
	return id;
}

const char* RoninEngine::Levels::Get_LevelName(int levelId) {
	return {};
}


void RoninEngine::Levels::Unload_Level(int levelId) {

}

void RoninEngine::Levels::Level_Free() {
	m_levels.clear();
	m_levels.shrink_to_fit();
}
