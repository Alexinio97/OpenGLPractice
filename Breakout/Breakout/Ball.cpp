#include "Ball.h"

Ball::Ball(glm::vec2 pos, float radius, glm::vec2 velocity, Texture texture) 
    : GameObject(pos, glm::vec2(radius *2.0f, radius * 2.0f), texture, glm::vec3(1.0f), velocity), Radius(radius), IsStuck(true)
{
}

glm::vec2 Ball::Move(float dt, unsigned int window_width)
{
    if (!IsStuck)
    {
        Position += Velocity * dt;

        // check if outside window bounds; if so, reverse velocity and restore at correct position
        if (Position.x < 0.0f)
        {
            Velocity = -Velocity;
            Position.x = 0.0f;
        }
        else if (Position.x + Size.x >= window_width)
        {
            Velocity.x = -Velocity.x;
            Position.x = window_width - Size.x;
        }
        if (Position.y <= 0.0f)
        {
            Velocity.y = -Velocity.y;
            Position.y = 0.0f;
        }
    }

    return Position;
}

void Ball::Reset(glm::vec2 position, glm::vec2 velocity)
{
    Position = position;
    Velocity = velocity;
    IsStuck = true;
}
