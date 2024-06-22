#include "Shader.h"
#include <iostream>
#include <fstream>

void Shader::Compile(const char* vertexShaderFile, const char* fragmentShaderFile)
{
    std::string vertexCode = GetFileContents(vertexShaderFile);
    std::string fragmentCode = GetFileContents(fragmentShaderFile);

    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    // compiling shaders into machine code
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    CompileErrors(vertexShader, "VERTEX");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    CompileErrors(vertexShader, "FRAGMENT");

    ShaderProgramId = glCreateProgram();

    glAttachShader(ShaderProgramId, vertexShader);
    glAttachShader(ShaderProgramId, fragmentShader);

    glLinkProgram(ShaderProgramId);
    CompileErrors(ShaderProgramId, "PROGRAM");

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource)
{
    unsigned int sVertex, sFragment, gShader;

    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    CompileErrors(sVertex, "VERTEX");

    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    CompileErrors(sFragment, "FRAGMENT");

    if (geometrySource != nullptr)
    {
        gShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(gShader, 1, &geometrySource, NULL);
        glCompileShader(gShader);
        CompileErrors(gShader, "FRAGMENT");
    }

    ShaderProgramId = glCreateProgram();
    glAttachShader(ShaderProgramId, sVertex);
    glAttachShader(ShaderProgramId, sFragment);
    if (geometrySource != nullptr)
        glActiveShaderProgram(ShaderProgramId, gShader);

    glLinkProgram(ShaderProgramId);
    CompileErrors(ShaderProgramId, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource != nullptr)
        glDeleteShader(gShader);
}

Shader& Shader::Use()
{
    glUseProgram(this->ShaderProgramId);
    return *this;
}

void Shader::Delete()
{
    glDeleteProgram(ShaderProgramId);
}

void Shader::SetMatrix4(const char* modelName, glm::mat4& model)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderProgramId, modelName), 1, GL_FALSE, &model[0][0]);
}

void Shader::SetVector3f(const char* colorName, glm::vec3& color)
{
    glUniform3f(glGetUniformLocation(ShaderProgramId, colorName), color.x, color.y, color.z);
}

void Shader::SetInteger(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(ShaderProgramId, name), value);
}

// private methods
std::string Shader::GetFileContents(const char* fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::binary);

    if (file.is_open())
    {
        std::cout << "File " << fileName << " is open" << std::endl;

        std::string content((std::istreambuf_iterator<char>(file)),
            (std::istreambuf_iterator<char>()));
        return content;
    }

    std::cout << "File " << fileName << " could not be open" << std::endl;
    throw(errno);
}

void Shader::CompileErrors(unsigned int shader, const char* type)
{
    GLint hasCompiled;
    char infoLog[1024];


    glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
    if (type != "PROGRAM")
    {       
        
        if (hasCompiled == GL_FALSE)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Shader Compilation error for: " << type << "\n";
            std::cout << "Error message: " << infoLog << "\n";
        }
    }
    else 
    {              
        if (hasCompiled == GL_FALSE)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "Shader Linking error for: " << type << "\n";
            std::cout << "Error message: " << infoLog << "\n";
        }
    }
}
