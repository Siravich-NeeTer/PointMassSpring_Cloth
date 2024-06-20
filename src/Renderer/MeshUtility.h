#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include <iostream>
#include <vector>

struct V3N3T2
{
	glm::vec3 vertices;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

void GenerateCubeBuffer(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices = nullptr);

void GenerateCubeBufferWithTexture(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices = nullptr);

void GenerateCubeBufferWithTextureNormal(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices = nullptr);