#include <glm/glm/glm.hpp>
#include "Game.h"
#include "Texture.h"
#include "SpriteRenderer.h"

Game::Game(unsigned int width, unsigned int height, SpriteRenderer* Renderer)
{
	m_Renderer = Renderer;
	m_Width = width;
	m_Height = height;
}

Game::~Game()
{
	delete m_Renderer;
}

void Game::Init(Shader& shader)
{	
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width),
		static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
	shader.SetMatrix4("projection", projection);
	shader.SetInteger("image", 0);
}

void Game::Update(float deltaTime)
{
}

void Game::Render(Texture texture) 
{
	m_Renderer->DrawSprite(texture,
		glm::vec2(200.0f, 200.0f),
		glm::vec2(300.0f, 400.0f),
		45.0f,
		glm::vec3(0.0f, 1.0f, 0.0f));
}
