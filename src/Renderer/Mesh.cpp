#include "Mesh.h"

std::vector<GLuint> squareIndices =
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


std::vector<glm::vec3> squareVertices =
{
    glm::vec3(-0.5f, -0.5f,  0.5f), //0
    glm::vec3( 0.5f, -0.5f,  0.5f), //1
    glm::vec3(-0.5f,  0.5f,  0.5f), //2
    glm::vec3( 0.5f,  0.5f,  0.5f), //3
    glm::vec3(-0.5f, -0.5f, -0.5f), //4
    glm::vec3( 0.5f, -0.5f, -0.5f), //5
    glm::vec3(-0.5f,  0.5f, -0.5f), //6
    glm::vec3( 0.5f,  0.5f, -0.5f)  //7
};

Mesh::Mesh()
	: vertices(squareVertices), indices(squareIndices)
{
	SetupMesh();
}

Mesh::Mesh(const std::vector<glm::vec3>& vertices)
	: vertices(vertices)
{
	m_VAO.Bind();
	m_VBO.Bind();
	m_VBO.BufferData(sizeof(glm::vec3) * vertices.size(), &vertices[0], false);

	m_VAO.Attribute(m_VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

	m_VBO.UnBind();
}

Mesh::Mesh(const std::vector <glm::vec3> & vertices, const std::vector<GLuint>& indices)
	: vertices(vertices), indices(indices)
{
	SetupMesh();
}

Mesh::~Mesh()
{
	m_VAO.Delete();
	m_VBO.Delete();
}

void Mesh::Draw(GLenum drawMode)
{
	m_VAO.Bind();
	if (indices.size() == 0)
	{
		glDrawArrays(drawMode, 0, vertices.size());
	}
	else
	{
		glDrawElements(drawMode, indices.size(), GL_UNSIGNED_INT, &indices[0]);
	}
}

void Mesh::SetVertices(const std::vector<glm::vec3>& vertices, bool isNormal)
{
	this->vertices = vertices;

	m_VAO.Bind();
	m_VBO.Bind();
	if (isNormal)
	{
		m_VBO.BufferData(sizeof(glm::vec3) * vertices.size(), &vertices[0], false);

		m_VAO.Attribute(m_VBO, 0, 3, GL_FLOAT, 2 * sizeof(glm::vec3), 0);
		m_VAO.Attribute(m_VBO, 1, 3, GL_FLOAT, 2 * sizeof(glm::vec3), sizeof(glm::vec3));
	}
	else
	{
		m_VBO.BufferData(sizeof(glm::vec3) * vertices.size(), &vertices[0], false);

		m_VAO.Attribute(m_VBO, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);
	}
	m_VBO.UnBind();
}

void Mesh::SetMesh(const std::vector<glm::vec3>& vertices, const std::vector<GLuint>& indices, bool isNormal)
{
	this->vertices = vertices;
	this->indices = indices;

	if (isNormal)
	{
		SetupNormalMesh();
	}
	else
	{
		SetupMesh();
	}
}

// ---------------------- Private Function ----------------------
void Mesh::SetupMesh()
{
	m_VAO.Bind();
	m_VBO.BufferData(3 * sizeof(float) * vertices.size(), &vertices[0], false);
	m_EBO.BufferData(sizeof(GLuint) * indices.size(), &indices[0], false);

	m_VAO.Attribute(m_VBO, 0, 3, GL_FLOAT, 3 * sizeof(float), 0);

	m_VBO.UnBind();
	m_EBO.UnBind();
}
void Mesh::SetupNormalMesh()
{
	m_VAO.Bind();
	m_VBO.BufferData(3 * sizeof(float) * vertices.size(), &vertices[0], false);
	m_EBO.BufferData(sizeof(GLuint) * indices.size(), &indices[0], false);

	m_VAO.Attribute(m_VBO, 0, 3, GL_FLOAT, 6 * sizeof(float), 0);
	m_VAO.Attribute(m_VBO, 1, 3, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float));

	m_VBO.UnBind();
	m_EBO.UnBind();
}