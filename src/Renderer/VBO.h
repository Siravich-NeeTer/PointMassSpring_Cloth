#pragma once

#include <glad/glad.h>

class VBO
{
	private:
		GLuint m_ID;
	public:
		VBO();
		void Bind() const;
		void UnBind() const;
		void Delete() const;
		void BufferData(size_t dataSize, const void* data, bool dynamic) const;
		GLuint GetID() const;
};