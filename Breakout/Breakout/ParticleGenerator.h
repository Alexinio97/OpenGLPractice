#pragma once
#include <glm/glm/glm.hpp>
#include "Texture.h"
#include "GameObject.h"
#include <vector>

struct Particle {
	glm::vec2 Position, Velocity;
	glm::vec4 Color;
	float LifeSpan;

	Particle()
		: Position(0.0f), Velocity(0.0f), Color(1.0f), LifeSpan(0.0f){}
};
class ParticleGenerator
{
public:
	ParticleGenerator(Shader shader, Texture texture, unsigned int amount);

	void Update(float dt, GameObject& object, unsigned int newParticles,
		glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	void Draw();

private:
	std::vector<Particle> particles;
	unsigned int amount;

	Shader shader;
	Texture texture;
	unsigned int VAO;

	void init();
	// returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
	unsigned int firstUnusedParticle();

	void respawnParticle(Particle& particle, GameObject& object,
	glm::vec2 offset = glm::vec2(0.0f, 0.0f));
};

