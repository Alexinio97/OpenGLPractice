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
#include <irrKlang.h>

using namespace irrklang;

SpriteRenderer* Renderer;
ParticleGenerator* Particles;
GameObject* Player;
Ball* PlayerBall;
PostProcessor* Effects;
ISoundEngine* SoundEngine = createIrrKlangDevice();

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

	ResourceManager::LoadTexture("powerup_chaos.png", true, "tex_chaos");
	ResourceManager::LoadTexture("powerup_confuse.png", true, "tex_confuse");
	ResourceManager::LoadTexture("powerup_increase.png", true, "tex_size");
	ResourceManager::LoadTexture("powerup_passthrough.png", true, "tex_pass");
	ResourceManager::LoadTexture("powerup_speed.png", true, "tex_speed");
	ResourceManager::LoadTexture("powerup_sticky.png", true, "tex_sticky");
	
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

	SoundEngine->play2D("SFX/breakout.mp3", true);
}

void Game::Update(float deltaTime)
{
	PlayerBall->Move(deltaTime, m_Width);

	DoCollisions();
	Particles->Update(deltaTime, *PlayerBall, 2, glm::vec2(PlayerBall->Radius / 2.0f));
	UpdatePowerUps(deltaTime);
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

		for (PowerUp& powerUp : PowerUps)
		{
			if (!powerUp.Destroyed)
				powerUp.Draw(*Renderer);
		}
		/*glm::vec2 ballSize = glm::vec2(PlayerBall->Radius * 2);
		Renderer->DrawDebugRectangle(PlayerBall->Position, ballSize,
			glm::vec3(1.0f, 4.0f, 0.0f));*/
	}
}

void ActivatePowerUp(PowerUp& powerUp);

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
				{
					brick.Destroyed = true;
					SpawnPowerUps(brick);
				}
				else
				{
					shakeTime = 0.05f;
					Effects->Shake = true;
				}

				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				brick.Destroyed = brick.IsSolid ? false : true;

				if (PlayerBall->PassThrough) continue;

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

		PlayerBall->IsStuck = PlayerBall->Sticky;
	}

	for (PowerUp& powerUp : PowerUps)
	{
		if (!powerUp.Destroyed)
		{
			if (powerUp.Position.y >= m_Height)
				powerUp.Destroyed = true;

			if (Collision::CheckCollision(*Player, powerUp))
			{
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = true;
				powerUp.Activated = true;
			}
		}
	}
}

void ActivatePowerUp(PowerUp& powerUp)
{
	if (powerUp.Type == "speed")
	{
		PlayerBall->Velocity *= 5;
	}
	else if (powerUp.Type == "sticky")
	{
		PlayerBall->Sticky = true;
		Player->Color = glm::vec3(1.0f, 0.5f, 1.0f);
	}
	else if (powerUp.Type == "pass-through")
	{
		PlayerBall->PassThrough = true;
		PlayerBall->Color = glm::vec3(1.0f, 0.5f, 0.5f);
	}
	else if (powerUp.Type == "pad-size-increase")
	{
		Player->Size.x += 50;
	}
	else if (powerUp.Type == "confuse")
	{
		if (!Effects->Chaos)
			Effects->Confuse = true;
	}
	else if (powerUp.Type == "chaos")
	{
		if (!Effects->Confuse)
			Effects->Chaos = true;
	}
}

static bool ShouldSpawn(unsigned int chance)
{
	unsigned int random = rand() % chance;
	return random == 0;
}


void Game::SpawnPowerUps(GameObject& block)
{
	std::vector<std::tuple<std::string, glm::vec3, float, Texture>> powerup_attrib = {
		{ "speed", glm::vec3(0.5f, 0.5f, 1.0f), 0.0f, 
		ResourceManager::GetTexture("tex_speed")},
		{ "sticky", glm::vec3(1.0f, 0.5f, 1.0f), 20.0f,
		ResourceManager::GetTexture("tex_sticky")},
		{ "pass-through", glm::vec3(0.5f, 1.0f, 0.5f), 10.0f,
		ResourceManager::GetTexture("tex_pass")},
		{ "pad-size-increase", glm::vec3(1.0f, 0.6f, 0.4f), 0.0f,
		ResourceManager::GetTexture("tex_size")},
		{ "confuse", glm::vec3(1.0f, 0.3f, 0.3f), 15.0f,
		ResourceManager::GetTexture("tex_confuse")},
		{ "chaos", glm::vec3(0.9f, 0.25f, 0.25f), 15.0f,
		ResourceManager::GetTexture("tex_chaos")}
	};

	for (auto powerUp : powerup_attrib)
	{
		if (ShouldSpawn(75))
			PowerUps.push_back(
				PowerUp(std::get<0>(powerUp), std::get<1>(powerUp),
					std::get<2>(powerUp), block.Position, std::get<3>(powerUp))
			);
	}
}

bool isOtherPowerUpActive(std::vector<PowerUp>& powerUps, std::string type)
{
	for (auto& powerUp : powerUps)
	{
		if (powerUp.Activated)
			if (powerUp.Type == type)
				return true;
	}

	return false;
}

void Game::UpdatePowerUps(float dt)
{
	for (auto& powerUp : PowerUps)
	{
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated)
		{
			powerUp.Duration -= dt;

			if (powerUp.Duration <= 0.0f)
			{
				powerUp.Activated = false;

				if (powerUp.Type == "sticky")
				{
					if (!isOtherPowerUpActive(PowerUps, "sticky"))
					{	// only reset if no other PowerUp of type sticky is active
						PlayerBall->Sticky = false;
						Player->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "pass-through")
				{
					if (!isOtherPowerUpActive(PowerUps, "pass-through"))
					{	// only reset if no other PowerUp of type pass-through is active
						PlayerBall->PassThrough = false;
						PlayerBall->Color = glm::vec3(1.0f);
					}
				}
				else if (powerUp.Type == "confuse")
				{
					if (!isOtherPowerUpActive(PowerUps, "confuse"))
					{	// only reset if no other PowerUp of type confuse is active
						Effects->Confuse = false;
					}
				}
				else if (powerUp.Type == "chaos")
				{
					if (!isOtherPowerUpActive(PowerUps, "chaos"))
					{	// only reset if no other PowerUp of type chaos is active
						Effects->Chaos = false;
					}
				}
			}			
		}
	}

	PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
		[](const PowerUp& powerUp) {
			return powerUp.Destroyed && !powerUp.Activated;
		}), PowerUps.end());
}

void Game::ResetLevel()
{
	auto levelName = std::to_string(Level) + ".lvl";
	Levels[Level].Load(levelName.c_str(), m_Width, m_Height / 2.0f);
	PowerUps.clear();
}

void Game::ResetPlayer()
{
	Player->Position = glm::vec2(m_Width / 2.0f - PLAYER_SIZE.x / 2.0f,
		m_Height - PLAYER_SIZE.y);
	PlayerBall->Position = Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2);
	PlayerBall->IsStuck = true;

	Effects->Chaos = Effects->Confuse = false;
	PlayerBall->PassThrough = PlayerBall->Sticky = false;
	Player->Color = glm::vec3(1.0f);
	PlayerBall->Color = glm::vec3(1.0f);
	PlayerBall->Velocity = INITIAL_BALL_VELOCITY;
}
