#include "SpriteRenderer.h"
#include <iostream>

void checkGLError();

SpriteRenderer::SpriteRenderer(Shader& shader)
{
    m_shader = shader;
    initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &m_quadVAO);
}

void SpriteRenderer::DrawSprite(Texture& texture, glm::vec2 position,
    glm::vec2 size, float rotate, glm::vec3 color)
{
    m_shader.Use();

    // initialize to identity matrix so it won't be null ( 0 )
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(position, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    m_shader.SetMatrix4("model", model);
    m_shader.SetVector3f("spriteColor", color);    
    glActiveTexture(GL_TEXTURE0);    
    texture.Bind();

    glBindVertexArray(m_quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
    unsigned int VBO;
	float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_quadVAO);
    glGenBuffers(1, &VBO);
    std::cout << "Generated VAO ID: " << m_quadVAO << std::endl;
    std::cout << "Generated VBO ID: " << VBO << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(m_quadVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
