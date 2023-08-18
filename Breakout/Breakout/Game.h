#pragma once

// forward declaration
class SpriteRenderer;
class Texture;
class Shader;

class Game
{
public:
	Game(unsigned int width, unsigned int height, SpriteRenderer* Renderer);
	~Game();

	// initialize game state (textures, shaders and levels)
	void Init(Shader& shader);
	void Update(float deltaTime);	

	void Render(Texture texture);

private:
	SpriteRenderer* m_Renderer;

	int m_Width;
	int m_Height;
};

