#include "EBO.h"

EBO::EBO(GLuint indices[], GLsizeiptr size)
{
	glGenBuffers(1, &ElementBufferObjectsId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObjectsId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}

void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObjectsId);
}

void EBO::Unbind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete()
{
	glDeleteBuffers(1, &ElementBufferObjectsId);
}
