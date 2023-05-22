#include "Floor.h"

Floor::Floor()
{
	m_Color = glm::vec3(0.6f);

	GenerateCubeBuffer(m_FloorMesh.vao, m_FloorMesh.vbo, m_FloorMesh.ebo, &m_FloorMesh.indices);
}

void Floor::Draw(const Shader& shader)
{
	if (!m_IsActive) { return; }

	shader.Activate();
	shader.SetVec3("u_Color", m_Color);
	shader.SetBool("u_IsTexture", false);
	shader.SetBool("u_DoLight", false);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Position);
	model = glm::scale(model, scale);
	shader.SetMat4("u_Model", model);

	m_FloorMesh.vao.Bind();
	glDrawElements(GL_TRIANGLES, m_FloorMesh.indices.size(), GL_UNSIGNED_INT, m_FloorMesh.indices.data());
	m_FloorMesh.ebo.UnBind();
	m_FloorMesh.vao.UnBind();
}