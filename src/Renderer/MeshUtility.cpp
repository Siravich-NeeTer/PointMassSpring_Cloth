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

void GenerateCubeBufferWithTexture(const VAO& vertexArray, const VBO& vertexBuffer, const EBO& elementBuffer, std::vector<GLuint>* indices)
{
	std::vector<GLuint> cubeIndices =
	{
		// Front face
		0, 1, 2,  // Triangle 1
		1, 3, 2,  // Triangle 2

		// Back face
		4, 6, 5,  // Triangle 1
		5, 6, 7,  // Triangle 2

		// Left face
		8, 9, 10,  // Triangle 1
		9, 11, 10, // Triangle 2

		// Right face
		12, 14, 13,  // Triangle 1
		13, 14, 15,  // Triangle 2

		// Top face
		16, 18, 17,  // Triangle 1
		17, 18, 19,  // Triangle 2

		// Bottom face
		20, 21, 22,  // Triangle 1
		21, 23, 22   // Triangle 2
	};

	std::vector<float> cubeVertices =
	{
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // Bottom right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  // Top left
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // Top right

		 // Back face
		-0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  // Bottom left
		 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // Bottom right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top left
		 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // Top right

		  // Left face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  // Bottom front
		-0.5f,  0.5f, -0.5f,  1.0f, 0.0f,  // Top front
		-0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  // Bottom back
		-0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  // Top back

		  // Right face
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Bottom front
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Top front
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Bottom back
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Top back

		  // Top face
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  // Front left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  // Front right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  // Back left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  // Back right

		   // Bottom face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  // Front left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  // Front right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  // Back left
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  // Back right
	};

	if (indices != nullptr)
	{
		*indices = cubeIndices;
	}

	vertexBuffer.BufferData(sizeof(float) * cubeVertices.size(), cubeVertices.data(), false);
	elementBuffer.BufferData(sizeof(GLuint) * cubeIndices.size(), cubeIndices.data(), false);

	vertexArray.Attribute(vertexBuffer, 0, 3, GL_FLOAT, 5 * sizeof(float), 0);
	vertexArray.Attribute(vertexBuffer, 2, 2, GL_FLOAT, 5 * sizeof(float), 3 * sizeof(float));
}