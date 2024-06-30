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
	void Delete() const;
	void SetMatrix4(const char* modelName, glm::mat4& model, bool useShader = false);
	void SetVector2f(const char* name, glm::vec2& offset) const;
	void SetVector3f(const char* colorName, glm::vec3& color) const;
	void SetVector4f(const char* name, glm::vec4& value) const;
	void SetInteger(const char* name, int value, bool useShader = false);
	void SetFloat(const char* name, float value);

private:
	std::string GetFileContents(const char* fileName);

	void CompileErrors(unsigned int shader, const char* type);

public:
	GLuint ShaderProgramId; // this is shader program id
};