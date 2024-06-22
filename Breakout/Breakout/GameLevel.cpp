#include "GameLevel.h"
#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#include "GameObject.h"

void GameLevel::Load(const char* file, unsigned int levelWidth, unsigned int levelHeight)
{
	Bricks.clear();

	unsigned int tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fileStream(file);

	std::vector<std::vector<unsigned int>> tileData;

	if (fileStream) 
	{
		while (std::getline(fileStream, line))
		{
			std::istringstream stringStream(line);
			std::vector<unsigned int> row;
			// stream extraction operator
			while (stringStream >> tileCode)
				row.push_back(tileCode);
			tileData.push_back(row);
		}

		if (tileData.size() > 0)
			init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
	for (GameObject& tile : Bricks)
	{
		if (!tile.Destroyed)
			tile.Draw(renderer);
	}
}

bool GameLevel::IsCompleted()
{
	return false;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData,
	unsigned int levelWidth, unsigned int levelHeight)
{
	unsigned int height = tileData.size();
	auto width = tileData[0].size();
	auto unit_width = levelWidth / static_cast<float>(width);
	float unit_height = static_cast<float>(levelHeight) / height;

	for (size_t y = 0; y < height; ++y)
	{
		for (size_t x = 0; x < width; x++)
		{
			if (tileData[y][x] == 1) // solid
			{
				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);

				GameObject gameObj(pos, size,
					ResourceManager::GetTexture("block_solid"),
					glm::vec3(0.8f, 0.8f, 0.7f)
				);

				gameObj.IsSolid = true;
				Bricks.push_back(gameObj);
				continue;
			}

			if (tileData[y][x] > 1) // non solid brick
			{
				glm::vec3 color = glm::vec3(1.0f); // original: white
				switch (tileData[y][x])
				{
				case 2:
					color = glm::vec3(0.2f, 0.6f, 1.0f);
					break;
				case 3:
					color = glm::vec3(0.0f, 0.7f, 0.0f);
					break;
				case 4:
					color = glm::vec3(0.8f, 0.8f, 0.4f);
					break;
				case 5:
					color = glm::vec3(1.0f, 0.5f, 0.0f);
					break;
				default:
					break;
				}

				glm::vec2 pos(unit_width * x, unit_height * y);
				glm::vec2 size(unit_width, unit_height);
				Bricks.push_back(GameObject(
					pos, size, ResourceManager::GetTexture("block"), color));				
			}
		}
	}
}
