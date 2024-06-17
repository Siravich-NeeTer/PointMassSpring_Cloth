#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

#include <iostream>
#include <vector>

void GenerateCubeBuffer(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices = nullptr);

void GenerateCubeBufferWithTexture(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices = nullptr);

void GenerateCubeBufferWithTextureNormal(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices = nullptr);