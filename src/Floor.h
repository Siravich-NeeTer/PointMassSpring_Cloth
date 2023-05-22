#pragma once

#include "Renderer/Mesh.h"
#include "Renderer/MeshUtility.h"
#include "Renderer/Shader.h"

#include "Object.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Floor : public Object
{
	private:
		struct FloorMesh
		{
			VAO vao;
			VBO vbo;
			EBO ebo;

			std::vector<GLuint> indices;

		} m_FloorMesh;

	public:
		glm::vec3 scale;

		Floor();

		void Draw(const Shader& shader);
};