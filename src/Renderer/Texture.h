#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "stb/stb_image.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

class Texture
{
	private:
		GLuint ID;

		GLuint m_Width;
		GLuint m_Height;
	public:
		Texture();
		Texture(const char* src);
		void SetTexture(const char* path);

		void Activate(GLenum TextureSlot);
		void Bind();
		void UnBind();
		void Delete();
};
