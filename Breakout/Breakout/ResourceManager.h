#pragma once
#include <map>
#include "Shader.h"
#include "Texture.h"

// this class will be static
class ResourceManager
{
public:
    // resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture> Textures;

    // loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
    static Shader LoadShader(const char* vShaderFile, const char* fShaderFile,
    const char* gShaderFile, std::string name);

    // retrieves a stored sader
    static Shader& GetShader(std::string name);

    // loads (and generates) a texture from file
    static Texture LoadTexture(const char* file, bool alpha, std::string name);

    // retrieves a stored texture
    static Texture& GetTexture(std::string name);

    // properly de-allocates all loaded resources
    static void Clear();

private:
    ResourceManager() = default;

    static Shader loadShaderFromFile(const char* vShaderFile, const char* fShaderFile,
        const char* gShaderFile = nullptr);
    static Texture loadTextureFromFile(const char* file, bool alpha);
};

