#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"

#include "Object.h"

#include "Constant.h"

const int SPHERE_SUBDIVISIONS = 32;

class Sphere : public Object
{
    private:
        struct SphereMesh
        {
            VAO vao;
            VBO vbo;
            EBO ebo;

            std::vector<GLuint> indices;

        } m_SphereMesh;
        float m_Radius;

    public:
        Sphere(const glm::vec3& position = glm::vec3(0, 0, 0), const float& radius = 1.0f)
            : m_Radius(radius)
        {
            m_Position = position;
            m_Color = glm::vec3(0.6f);

            std::vector<glm::vec3> vertices;
            std::vector<GLuint> indices;

            float section = PI / SPHERE_SUBDIVISIONS;
            for (int i = 0; i <= SPHERE_SUBDIVISIONS; ++i)
            {
                float theta = i * section;
                float sinTheta = std::sin(theta);
                float cosTheta = std::cos(theta);

                for (int j = 0; j <= SPHERE_SUBDIVISIONS; ++j)
                {
                    float phi = j * 2 * section;
                    float sinPhi = std::sin(phi);
                    float cosPhi = std::cos(phi);

                    glm::vec3 vertex(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
                    vertices.push_back(vertex);
                }
            }

            for (int i = 0; i < SPHERE_SUBDIVISIONS; ++i)
            {
                for (int j = 0; j < SPHERE_SUBDIVISIONS; ++j)
                {
                    int index0 = i * (SPHERE_SUBDIVISIONS + 1) + j;
                    int index1 = index0 + 1;
                    int index2 = (i + 1) * (SPHERE_SUBDIVISIONS + 1) + j;
                    int index3 = index2 + 1;

                    indices.push_back(index0);
                    indices.push_back(index2);
                    indices.push_back(index1);

                    indices.push_back(index1);
                    indices.push_back(index2);
                    indices.push_back(index3);
                }
            }

            m_SphereMesh.vbo.BufferData(sizeof(glm::vec3) * vertices.size(), vertices.data(), false);
            m_SphereMesh.ebo.BufferData(sizeof(GLuint) * indices.size(), indices.data(), false);

            m_SphereMesh.vao.Attribute(m_SphereMesh.vbo, 0, 3, GL_FLOAT, sizeof(glm::vec3), 0);

            m_SphereMesh.indices = indices;
        }

        void Draw(const Shader& shader)
        {
            if (!m_IsActive) { return; }

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, m_Position);
            model = glm::scale(model, glm::vec3(m_Radius, m_Radius, m_Radius));

            shader.SetBool("u_IsTexture", false);
            shader.SetMat4("u_Model", model);
            shader.SetVec3("u_Color", m_Color);

            m_SphereMesh.vao.Bind();
            glDrawElements(GL_TRIANGLES, m_SphereMesh.indices.size(), GL_UNSIGNED_INT, m_SphereMesh.indices.data());
            m_SphereMesh.ebo.UnBind();
            m_SphereMesh.vao.UnBind();
        }

        void DecreaseZ(const float& dt) { m_Position.z -= 1.0f * dt; }
        void IncreaseZ(const float& dt) { m_Position.z += 1.0f * dt; }
        void DecreaseY(const float& dt) { m_Position.y -= 1.0f * dt; }
        void IncreaseY(const float& dt) { m_Position.y += 1.0f * dt; }

        float GetRadius() const
        {
            return m_Radius;
        }
};