#include "Floor.h"

Floor::Floor()
{
	m_FloorTexture.SetTexture("Texture/container.jpg");

	GenerateCubeBufferWithTexture(m_FloorMesh.vao, m_FloorMesh.vbo, m_FloorMesh.ebo, &m_FloorMesh.indices);
}

void Floor::Draw(const Shader& shader)
{
	if (!m_IsActive) { return; }

	m_FloorTexture.Activate(GL_TEXTURE0);

	shader.Activate();
	shader.SetVec3("u_Color", m_Color);
	shader.SetBool("u_IsTexture", true);
	shader.SetBool("u_DoLight", false);
	shader.SetInt("u_Texture", 0);

	glm::mat4 model = glm::translate(glm::mat4(1.0f), m_Position);
	model = glm::scale(model, scale);
	shader.SetMat4("u_Model", model);

	m_FloorMesh.vao.Bind();
	glDrawElements(GL_TRIANGLES, m_FloorMesh.indices.size(), GL_UNSIGNED_INT, m_FloorMesh.indices.data());
	m_FloorMesh.ebo.UnBind();
	m_FloorMesh.vao.UnBind();
}