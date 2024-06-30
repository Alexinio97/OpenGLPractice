#include <glm/glm/glm.hpp>
#include <vector>
#include "Game.h"
#include "Texture.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "Ball.h"
#include "Collision.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"

SpriteRenderer* Renderer;
ParticleGenerator* Particles;
GameObject* Player;
Ball* PlayerBall;
PostProcessor* Effects;
float shakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height)
	: GameState(GAME_ACTIVE), m_Width(width), m_Height(height), Level(0)
{

}

Game::~Game()
{
	delete Renderer;
	delete Player;
	delete Particles;
	delete PlayerBall;
	delete Effects;
}

void Game::Init()
{		
	ResourceManager::LoadShader("sprite.vs", "sprite.frag", nullptr, "sprite");
	ResourceManager::LoadShader("particle.vert", "particle.frag", nullptr, "particle");
	ResourceManager::LoadShader("post_process.vs", "post_process.frag", nullptr, "post_process");

	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width),
		static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
	auto shader = ResourceManager::GetShader("sprite");
	shader.Use();
	shader.SetInteger("image", 0);
	shader.SetMatrix4("projection", projection);	
	ResourceManager::GetShader("particle").Use().SetMatrix4("projection", projection);
	// set render-specific controls	
	Renderer = new SpriteRenderer(shader);

	ResourceManager::LoadTexture("background.jpg", false, "background");
	ResourceManager::LoadTexture("block.png", false, "block");
	ResourceManager::LoadTexture("block_solid.png", false, "block_solid");	
	ResourceManager::LoadTexture("paddle.png", true, "paddle");
	ResourceManager::LoadTexture("ballTexture.png", true, "ball");
	ResourceManager::LoadTexture("particle.png", true, "particle");
	
	GameLevel levelOne;
	levelOne.Load("0.lvl", m_Width, m_Height / 2);
	Levels.push_back(levelOne);
	Level = 0;

	Particles = new ParticleGenerator(
		ResourceManager::GetShader("particle"),
		ResourceManager::GetTexture("particle"),
		500
	);
	Effects = new PostProcessor(
		ResourceManager::GetShader("post_process"),
		m_Width, m_Height);

	glm::vec2 playerPos = glm::vec2(m_Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		m_Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2);
	PlayerBall = new Ball(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("ball"));
}

void Game::Update(float deltaTime)
{
	PlayerBall->Move(deltaTime, m_Width);

	DoCollisions();
	Particles->Update(deltaTime, *PlayerBall, 2, glm::vec2(PlayerBall->Radius / 2.0f));

	if (PlayerBall->Position.y >= m_Height) // did ball reach bottom edge?
	{
		ResetLevel();
		ResetPlayer();
	}

	if (shakeTime > 0.0f)
	{
		shakeTime -= deltaTime;
		if (shakeTime <= 0.0f)
			Effects->Shake = false;
	}
}

void Game::ProcessInput(float deltaTime)
{
	if (GameState == GAME_ACTIVE) {
		float velocity = PLAYER_VELOCITY * deltaTime;

		if (Keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0.0f)
			{
				Player->Position.x -= velocity;
				if (PlayerBall->IsStuck)
					PlayerBall->Position.x -= velocity;
			}

		}
		if (Keys[GLFW_KEY_D])
		{
			if (Player->Position.x < m_Width - PLAYER_SIZE.x)
			{
				Player->Position.x += velocity;
				if (PlayerBall->IsStuck)
					PlayerBall->Position.x += velocity;
			}
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
		Effects->BeginRender();
		auto texture = ResourceManager::GetTexture("background");
		Renderer->DrawSprite(texture,
			glm::vec2(0.0f, 0.0f), glm::vec2(m_Width, m_Height), 0.0f);
		Levels[Level].Draw(*Renderer);
		Player->Draw(*Renderer);
		if (!PlayerBall->IsStuck)
			Particles->Draw();
		PlayerBall->Draw(*Renderer);		
		Effects->EndRender();
		Effects->Render(glfwGetTime());
		/*glm::vec2 ballSize = glm::vec2(PlayerBall->Radius * 2);
		Renderer->DrawDebugRectangle(PlayerBall->Position, ballSize,
			glm::vec3(1.0f, 4.0f, 0.0f));*/
	}
}

void Game::DoCollisions()
{
	for (auto& brick : Levels[Level].Bricks)
	{
		if (!brick.Destroyed)
		{
			auto collision = Collision::CheckCollision(*PlayerBall, brick);
			if (std::get<0>(collision))
			{
				if (!brick.IsSolid)
					brick.Destroyed = true;
				else
				{
					shakeTime = 0.05f;
					Effects->Shake = true;
				}

				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);

				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					PlayerBall->Velocity.x = -PlayerBall->Velocity.x;
					// relocate
					float penetration = PlayerBall->Radius - std::abs(diff_vector.x);
					if (dir == LEFT)
						PlayerBall->Position.x += penetration;
					else
						PlayerBall->Position.x -= penetration;
				}
				else // vertical collision
				{
					PlayerBall->Velocity.y = -PlayerBall->Velocity.y;
					// relocate
					float penetration = PlayerBall->Radius - std::abs(diff_vector.y);
					if (dir == UP)
						PlayerBall->Position.y -= penetration;
					else
						PlayerBall->Position.y += penetration;
				}
				brick.Destroyed = brick.IsSolid ? false : true;
			}
		}
	}

	CollisionTup result = Collision::CheckCollision(*PlayerBall, *Player);
	if (!PlayerBall->IsStuck && std::get<0>(result))
	{
		// check where it hit the board, and change velocity based on where it hit the board
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (PlayerBall->Position.x + PlayerBall->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);
		// then move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = PlayerBall->Velocity;
		PlayerBall->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		PlayerBall->Velocity.y = -1.0f * abs(PlayerBall->Velocity.y);
		PlayerBall->Velocity = glm::normalize(PlayerBall->Velocity) * glm::length(oldVelocity);
	}
}

void Game::ResetLevel()
{
	auto levelName = std::to_string(Level) + ".lvl";
	Levels[Level].Load(levelName.c_str(), m_Width, m_Height / 2.0f);
}

void Game::ResetPlayer()
{
	Player->Position = glm::vec2(m_Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		m_Height - PLAYER_SIZE.y);
	PlayerBall->Position = Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2);
	PlayerBall->IsStuck = true;
}
