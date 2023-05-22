#include "MeshUtility.h"

void GenerateCubeBuffer(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices)
{
	std::vector<GLuint> cubeIndices =
	{
		//Top
		2, 6, 7,
		2, 3, 7,

		//Bottom
		0, 4, 5,
		0, 1, 5,

		//Left
		0, 2, 6,
		0, 4, 6,

		//Right
		1, 3, 7,
		1, 5, 7,

		//Front
		0, 2, 3,
		0, 1, 3,

		//Back
		4, 6, 7,
		4, 5, 7
	};
	std::vector<glm::vec3> cubeVertices =
	{
		glm::vec3(-0.5f, -0.5f,  0.5f), // 0
		glm::vec3( 0.5f, -0.5f,  0.5f), // 1
		glm::vec3(-0.5f,  0.5f,  0.5f), // 2
		glm::vec3( 0.5f,  0.5f,  0.5f), // 3
		glm::vec3(-0.5f, -0.5f, -0.5f), // 4
		glm::vec3( 0.5f, -0.5f, -0.5f), // 5
		glm::vec3(-0.5f,  0.5f, -0.5f), // 6
		glm::vec3( 0.5f,  0.5f, -0.5f)  // 7
	};

	if (indices != nullptr)
	{
		*indices = cubeIndices;
	}

	vertexBuffer.BufferData(sizeof(glm::vec3) * cubeVertices.size(), cubeVertices.data(), false);
	elementBuffer.BufferData(sizeof(GLuint) * cubeIndices.size(), cubeIndices.data(), false);

	vertexArray.Attribute(vertexBuffer, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
}