#pragma once
#include "GameObject.h"


const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
const float BALL_RADIUS = 12.5f;

class Ball :
    public GameObject
{
public:
    bool IsStuck;
    float Radius;
    bool Sticky, PassThrough;

public:
    Ball() = default;

    Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture texture);
    glm::vec2 Move(float dt, unsigned int window_width);
    void Reset(glm::vec2 position, glm::vec2 velocity);
};

