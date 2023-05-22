#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class Shader
{
	private:
		GLuint m_ID;
		void checkCompileError(GLuint shaderID, const std::string &type);
	public:
		Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
		~Shader();
		void Activate() const;

		// Act like glUniform
		void SetBool(const std::string& name, bool value) const;
		void SetInt(const std::string& name, int value) const;
		void SetFloat(const std::string& name, float value) const;
		void SetVec2(const std::string& name, const glm::vec2& value) const;
		void SetVec3(const std::string& name, const glm::vec3& value) const;
		void SetVec4(const std::string& name, const glm::vec4& value) const;
		void SetMat4(const std::string& name, const glm::mat4& value) const;

		GLuint GetID() const;
};