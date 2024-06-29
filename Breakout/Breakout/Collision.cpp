#include "Collision.h"
#include "Ball.h"
#include <glm/glm/glm.hpp>

bool Collision::CheckCollision(GameObject& current, GameObject& other)
{
    bool collisionX = current.Position.x + current.Size.x >= other.Position.x &&
        other.Position.x + other.Size.x >= current.Position.x;
    bool collisionY = current.Position.y + current.Size.y >= other.Position.y &&
        other.Position.y + other.Size.y >= current.Position.y;
    return collisionX && collisionY;
}

CollisionTup Collision::CheckCollision(Ball& current, GameObject& other)
{
    glm::vec2 ball_center(current.Position + current.Radius);
    glm::vec2 aabb_half_extents(other.Size.x / 2, other.Size.y / 2.0f);

    glm::vec2 aabb_center(
        other.Position.x + aabb_half_extents.x,
        other.Position.y + aabb_half_extents.y
    );

    glm::vec2 difference = ball_center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;

    difference = closest - ball_center;
    if (glm::length(difference) <= current.Radius)
        return std::make_tuple(true, VectorDirection(difference), difference);
    else
        return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

Direction Collision::VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
         glm::vec2(0.0f, 1.0f),	// up
         glm::vec2(1.0f, 0.0f),	// right
         glm::vec2(0.0f, -1.0f),	// down
         glm::vec2(-1.0f, 0.0f)	// left
    };
    float max = 0.0f;
    unsigned int best_match = -1;

    for (unsigned int i = 0; i < 4; i++)
    {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max)
        {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}
