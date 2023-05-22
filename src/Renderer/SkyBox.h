#pragma once

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shader.h"
#include "MeshUtility.h"

#include "../Camera.h"

#include "stb/stb_image.h"

#include <string>
#include <array>

class SkyBox
{
	private:
		VAO vao;
		VBO vbo;
		EBO ebo;

		GLuint cubemapTexture;

	public:
		SkyBox()
		{
			GenerateCubeBuffer(vao, vbo, ebo);
		}
		SkyBox(const std::array<std::string, 6> &cubeMapTexturePath)
		{
			GenerateCubeBuffer(vao, vbo, ebo);
			SetTexture(cubeMapTexturePath);
		}

		void Draw(const Shader& shader)
		{
			// Since the cubemap will always have a depth of 1.0, we need that equal sign so it doesn't get discarded
			glDepthFunc(GL_LEQUAL);

			shader.Activate();

			shader.SetInt("u_SkyBox", 0);

			vao.Bind();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			vao.UnBind();

			// Switch back to the normal depth function
			glDepthFunc(GL_LESS);
		}

		void SetTexture(const std::array<std::string, 6>& cubeMapTexturePath)
		{
			glGenTextures(1, &cubemapTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			// These are very important to prevent seams
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			// This might help with seams on some systems
			//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

			// Cycles through all the textures and attaches them to the cubemap object
			for (unsigned int i = 0; i < 6; i++)
			{
				int width, height, nrChannels;
				unsigned char* data = stbi_load(cubeMapTexturePath[i].c_str(), &width, &height, &nrChannels, 0);
				if (data)
				{
					stbi_set_flip_vertically_on_load(false);
					glTexImage2D
					(
						GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
						0,
						GL_RGB,
						width,
						height,
						0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						data
					);
					stbi_image_free(data);
				}
				else
				{
					std::cout << "Failed to load texture: " << cubeMapTexturePath[i] << "\n";
					stbi_image_free(data);
				}
			}
		}
};