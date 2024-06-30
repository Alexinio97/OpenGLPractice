#pragma once
#include "Shader.h"
#include "Texture.h"

class PostProcessor
{
public:
	PostProcessor(Shader shader, unsigned int width, unsigned int height);
	void BeginRender();
	void EndRender();
	void Render(float time);

public:
	Shader PostProcessingShader;
	Texture GivenTexture;
	unsigned int Width, Height;
	bool Confuse, Chaos, Shake;	

private:
	unsigned int MSFBO, FBO; //  MSFBO = Multisampled FBO. FBO is regular, used for blitting MS color-buffer to texture
	unsigned int RBO;
	unsigned int VAO;

	void initRenderData();
};

