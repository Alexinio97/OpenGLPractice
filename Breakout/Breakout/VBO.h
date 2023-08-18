#pragma once
// Vertex buffer class

#include <glad/glad.h>


class VBO {
public:
	VBO(GLfloat* vertices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();

public:
	GLuint VertexBufferId;

};