#include "SpriteRenderer.h"
#include <iostream>

Shader rectangleShader{};

SpriteRenderer::SpriteRenderer(Shader& shader)
{
    m_shader = shader;
    initRenderData();
    rectangleShader.Compile("default.vert", "default.frag");
}

SpriteRenderer::~SpriteRenderer()
{
    glDeleteVertexArrays(1, &m_quadVAO);
    glDeleteVertexArrays(1, &m_debugVAO);
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

void SpriteRenderer::DrawDebugRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color)
{
    rectangleShader.Use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(1600),
        static_cast<float>(800), 0.0f, -1.0f, 1.0f);
    rectangleShader.SetMatrix4("model", model);
    rectangleShader.SetVector3f("spriteColor", color);
    rectangleShader.SetMatrix4("projection", projection);

    glBindVertexArray(m_debugVAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}

void SpriteRenderer::initRenderData()
{
    unsigned int VBO;
    unsigned int debugVBO;
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
    
    // Initialize debug rectangle data
    float debugVertices[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &m_debugVAO);
    glGenBuffers(1, &debugVBO);
    glBindBuffer(GL_ARRAY_BUFFER, debugVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(debugVertices), debugVertices, GL_STATIC_DRAW);

    glBindVertexArray(m_debugVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}
