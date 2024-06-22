#pragma once
#include <glad/glad.h>
#include <stb/stb_image.h>

#include "Shader.h"

/*
* S axis: The S axis is analogous to the X axis in the texture coordinate space.
It represents the horizontal direction on the texture.
T axis: The T axis is analogous to the Y axis in the texture coordinate space.
It represents the vertical direction on the texture.
*/

class Texture
{
public: 
	unsigned int Width, Height; // width and height of loaded image in pixels
	// texture Format
	unsigned int Internal_Format; // format of texture object
	GLenum Image_Format; // format of loaded image
	// texture configuration
	GLint Wrap_S; // wrapping mode on S axis
	GLint Wrap_T; // wrapping mode on T axis
	GLint Filter_Min; // filtering mode if texture pixels < screen pixels
	GLint Filter_Max; // filtering mode if texture pixels > screen pixels

public:
	GLuint ID;
	GLenum type;
	Texture() = default;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

	void Generate(unsigned int width, unsigned int height, unsigned char* data);
	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};

