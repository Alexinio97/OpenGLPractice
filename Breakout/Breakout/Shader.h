#pragma once

#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <string>

class Shader
{
public:
	Shader() = default;
	void Compile(const char* vertexShaderFile, const char* fragmentShaderFile);
	void Compile(const char* vertexSource, const char* fragmentSource,
		const char* geometrySource);

	Shader& Use();
	void Delete();
	void SetMatrix4(const char* modelName, glm::mat4& model);
	void SetVector3f(const char* colorName, glm::vec3& color);
	void SetInteger(const char* name, int value);

private:
	std::string GetFileContents(const char* fileName);

	void CompileErrors(unsigned int shader, const char* type);

public:
	GLuint ShaderProgramId; // this is shader program id
};