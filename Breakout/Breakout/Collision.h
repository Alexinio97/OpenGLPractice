#pragma once
#include "GameObject.h"
#include "Ball.h"

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

typedef std::tuple<bool, Direction, glm::vec2> CollisionTup;

class Collision
{
public:
	static bool CheckCollision(GameObject& current, GameObject& other);

	static CollisionTup CheckCollision(Ball& current, GameObject& other);

private:
	static Direction VectorDirection(glm::vec2 target);
};



