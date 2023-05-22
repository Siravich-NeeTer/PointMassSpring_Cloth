#pragma once

#include <glad/glad.h>

class EBO
{
	private:
		GLuint m_ID;
	public:
		EBO();
		void Bind() const;
		void UnBind() const;
		void Delete() const;
		void BufferData(GLsizei dataSize, const void* data, GLboolean dynamic) const;
		GLuint GetID() const;
};