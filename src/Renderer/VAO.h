#pragma once

#include <glad/glad.h>
#include "VBO.h"

class VAO
{
	private:
		GLuint m_ID;
	public:
		VAO();
		void Bind() const;
		void UnBind() const;
		void Delete() const;
		void Attribute(const VBO& vbo, GLuint index, GLint size, GLenum varType, GLsizei stride, GLsizei offset) const;
		GLuint GetID() const;
};