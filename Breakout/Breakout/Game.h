#pragma once
#include <vector>
#include "GameLevel.h"

// forward declaration
class SpriteRenderer;
class Texture;
class Shader;

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};


class Game
{
public:
	Game(unsigned int width, unsigned int height);
	~Game();

	// initialize game state (textures, shaders and levels)
	void Init();
	void Update(float deltaTime);	

	void Render();

public:
	std::vector<GameLevel> Levels;
	unsigned int Level;
	GameState GameState;

private:
	int m_Width;
	int m_Height;
};