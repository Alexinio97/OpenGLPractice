#include <glm/glm/glm.hpp>
#include <vector>
#include "Game.h"
#include "Texture.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Ball.h"

SpriteRenderer* Renderer;
GameObject* Player;
Ball* PlayerBall;

Game::Game(unsigned int width, unsigned int height)
	: GameState(GAME_ACTIVE), m_Width(width), m_Height(height), Level(0)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
}

void Game::Init()
{	
	ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width),
		static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
	auto shader = ResourceManager::GetShader("sprite");
	shader.Use();
	shader.SetInteger("image", 0);
	shader.SetMatrix4("projection", projection);	
	// set render-specific controls	
	Renderer = new SpriteRenderer(shader);

	ResourceManager::LoadTexture("background.jpg", false, "background");
	ResourceManager::LoadTexture("block.png", false, "block");
	ResourceManager::LoadTexture("block_solid.png", false, "block_solid");	
	ResourceManager::LoadTexture("paddle.png", true, "paddle");
	ResourceManager::LoadTexture("ballTexture.png", true, "ball");
	
	GameLevel levelOne;
	levelOne.Load("Standard.lvl", m_Width, m_Height / 2);
	Levels.push_back(levelOne);
	Level = 0;

	glm::vec2 playerPos = glm::vec2(m_Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		m_Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2);
	PlayerBall = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("ball"));
}

void Game::Update(float deltaTime)
{
	PlayerBall->Move(deltaTime, m_Width);
}

void Game::ProcessInput(float deltaTime)
{
	if (GameState == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * deltaTime;

		if (Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
				Player->Position.x -= velocity;

			if (PlayerBall->IsStuck && PlayerBall->Position.x >= PLAYER_SIZE.x / 2.0f - BALL_RADIUS)
				PlayerBall->Position.x -= velocity;

		}
		if (Keys[GLFW_KEY_D])
		{
			if (Player->Position.x < m_Width - PLAYER_SIZE.x)
				Player->Position.x += velocity;

			if (PlayerBall->IsStuck && PlayerBall->Position.x < m_Width - (PLAYER_SIZE.x / 2.0f + BALL_RADIUS))
				PlayerBall->Position.x += velocity;
		}

		if (Keys[GLFW_KEY_SPACE])
		{
			PlayerBall->IsStuck = false;
		}
	}
}

void Game::Render() 
{	
	if (GameState == GAME_ACTIVE)
	{		
		auto texture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(texture,
			glm::vec2(0.0f, 0.0f), glm::vec2(m_Width, m_Height), 0.0f);
		Levels[Level].Draw(*Renderer);
		Player->Draw(*Renderer);
		PlayerBall->Draw(*Renderer);
	}
}
