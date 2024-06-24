#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

#include <iostream>
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Texture.h"
#include "Game.h"
#include "SpriteRenderer.h"
#include "Breakout.h"
#include "ResourceManager.h"

void KeyPressedCallBack(GLFWwindow* window, int key, int scanCode, int action, int mode);
void BufferResizeCallback(GLFWwindow* window, int width, int height);
void checkGLError(const std::string& location);

Game Breakout(1600, 800);

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(1600, 900, "Breakout", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetKeyCallback(window, KeyPressedCallBack);
    glfwSetFramebufferSizeCallback(window, BufferResizeCallback);

    glViewport(0, 0, 1600, 900);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
     
    Breakout.Init();

    // deltaTime variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        Breakout.Update(deltaTime);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        Breakout.Render();  

        glfwSwapBuffers(window);
    }


    ResourceManager::Clear();    
    glfwTerminate();
    return 0;
}


void KeyPressedCallBack(GLFWwindow* window, int key, int scanCode, int action, int mode)
{
    std::cout << "Key pressed: " << key << " Action: " << action << " ScanCode: " << scanCode
        << std::endl;
}

void BufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void checkGLError(const std::string& location)
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << location << std::endl;
    }
}
