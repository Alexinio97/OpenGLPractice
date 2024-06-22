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

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Breakout", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSetKeyCallback(window, KeyPressedCallBack);

    glViewport(0, 0, 800, 800);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);
 
    Game Breakout(800, 800);
    
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

