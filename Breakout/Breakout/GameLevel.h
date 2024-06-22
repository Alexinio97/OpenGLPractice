#pragma once
#include <vector>
#include "SpriteRenderer.h"
#include "GameObject.h"


class GameLevel
{
public:
	GameLevel() = default;

	std::vector<GameObject> Bricks;

	void Load(const char* file, unsigned int levelWidth, unsigned int levelHeight);

	void Draw(SpriteRenderer& renderer);

	bool IsCompleted();
private:
	void init(std::vector<std::vector<unsigned int>> tileData,
		unsigned int levelWidth, unsigned int levelHeight);


};