#include "VAO.h"

// Constructor -> Generate VAO
VAO::VAO()
{
	glGenVertexArrays(1, &m_ID);
}

void VAO::Bind() const
{
	glBindVertexArray(m_ID);
}

void VAO::UnBind() const
{
	glBindVertexArray(0);
}

// Delete Vertex Array via ID
void VAO::Delete() const
{
	glDeleteVertexArrays(1, &m_ID);
}

// Bind VBO and Specific Data(AttribPointer) of that VBO
void VAO::Attribute(const VBO& vbo, GLuint index, GLint size, GLenum varType, GLsizei stride, GLsizei offset) const
{
	this->Bind();
	vbo.Bind();
	glVertexAttribPointer(index, size, varType, GL_FALSE, stride, (const void*)offset);
	glEnableVertexAttribArray(index);
	vbo.UnBind();
}

// Get VAO ID
GLuint VAO::GetID() const
{
	return m_ID;
}