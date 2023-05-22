#include "EBO.h"

EBO::EBO()
{
	glGenBuffers(1, &m_ID);
}

void EBO::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);
}

void EBO::UnBind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void EBO::Delete() const
{
	glDeleteBuffers(1, &m_ID);
}

void EBO::BufferData(GLsizei dataSize, const void* data, GLboolean dynamic) const
{
	this->Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

GLuint EBO::GetID() const
{
	return m_ID;
}