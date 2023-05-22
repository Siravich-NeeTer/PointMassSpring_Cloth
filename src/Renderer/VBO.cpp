#include "VBO.h"

// Constructor -> Generate VBO
VBO::VBO()
{
	glGenBuffers(1, &m_ID);
}

void VBO::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}

void VBO::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Delete VBO via ID
void VBO::Delete() const
{
	glDeleteBuffers(1, &m_ID);
}

// Bind Buffer and Specific that Buffer
void VBO::BufferData(size_t dataSize, const void* data, bool dynamic) const
{
	this->Bind();
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, (dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW));
}

// Get VBO ID
GLuint VBO::GetID() const
{
	return m_ID;
}