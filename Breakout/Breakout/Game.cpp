#include <glm/glm/glm.hpp>
#include <vector>
#include "Game.h"
#include "Texture.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

SpriteRenderer* Renderer;

Game::Game(unsigned int width, unsigned int height)
{	
	m_Width = width;
	m_Height = height;
}

Game::~Game()
{
}

void Game::Init()
{	
	ResourceManager::LoadShader("sprite_transform.vert", "sprite.frag", nullptr, "sprite");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width),
		static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
	// set render-specific controls
	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	ResourceManager::LoadTexture("background.jpg", false, "background");
	ResourceManager::LoadTexture("block.png", false, "block");
	ResourceManager::LoadTexture("block_solid.png", false, "block_solid");

	GameLevel levelOne;
	levelOne.Load("Standard.lvl", m_Width, m_Height);
	Levels.push_back(levelOne);
	Level = 0;
}

void Game::Update(float deltaTime)
{
}

void Game::Render() 
{
	if (GameState == GAME_ACTIVE)
	{		
		Renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0.0f, 0.0f), glm::vec2(m_Width, m_Height), 0.0f);
		Levels[Level].Draw(*Renderer);
	}
}
