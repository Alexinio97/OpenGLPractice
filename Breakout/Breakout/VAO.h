#pragma once
#include <glad/glad.h>
#include "VBO.h"

class VAO
{
public:

	VAO();
	// stride = the distance in bytes between the beginning of a vertex and the end of another
	void LinkAttrib(VBO& VBO, GLuint layout,
		GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	void Bind();
	void Unbind();
	void Delete();

	GLuint VertexArrayObjectsId;
};

