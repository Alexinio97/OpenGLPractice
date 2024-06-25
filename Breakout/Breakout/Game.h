#pragma once
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
const float PLAYER_VELOCITY(500.0f);

class Game
{
public:
	Game(unsigned int width, unsigned int height);
	~Game();

	// initialize game state (textures, shaders and levels)
	void Init();
	void Update(float deltaTime);	
	void ProcessInput(float deltaTime);
	void Render();

public:
	std::vector<GameLevel> Levels;
	unsigned int Level;
	GameState GameState;
	bool Keys[1024];

private:
	int m_Width;
	int m_Height;
};