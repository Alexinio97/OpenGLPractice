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

void KeyPressedCallBack(GLFWwindow* window, int key, int scanCode, int action, int mode);

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "OpenGl window", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL();
    glfwSetKeyCallback(window, KeyPressedCallBack);

    glViewport(0, 0, 700, 700);

    Shader shaderProgram{};
    shaderProgram.Compile("sprite_transform.vert", "sprite.frag");
 
       
    Texture GameTexture("img.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    GameTexture.texUnit(shaderProgram, "tex0", 0);
    SpriteRenderer* Renderer = new SpriteRenderer(shaderProgram);
    Game testGame(800, 800, Renderer);
    
    testGame.Init(shaderProgram);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // tell opengl which shader program we want to use
        shaderProgram.Activate();
        

        // Render
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        testGame.Render(GameTexture);        
        
        glfwSwapBuffers(window);
        
    }

    GameTexture.Delete();
    shaderProgram.Delete();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


void KeyPressedCallBack(GLFWwindow* window, int key, int scanCode, int action, int mode)
{
    std::cout << "Key pressed: " << key << " Action: " << action << " ScanCode: " << scanCode
        << std::endl;
}

