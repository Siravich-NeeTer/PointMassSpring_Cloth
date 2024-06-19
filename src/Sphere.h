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
        Texture m_SphereTexture;

    public:
        Sphere(const glm::vec3& position = glm::vec3(0, 0, 0), const float& radius = 1.0f)
            : m_Radius(radius)
        {
            m_SphereTexture.SetTexture("Texture/BasketBall.png");

            m_Position = position;
            m_Color = glm::vec3(0.6f);

            std::vector<V3N3T2> vertices;
            std::vector<GLuint> indices;

            float sectorStep = 2 * PI / SPHERE_SUBDIVISIONS;
            float stackStep = PI / SPHERE_SUBDIVISIONS;
            float sectorAngle, stackAngle;

            for (int i = 0; i <= SPHERE_SUBDIVISIONS; ++i)
            {
                stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
                float xy = radius * cosf(stackAngle);             // r * cos(u)
                float z = radius * sinf(stackAngle);              // r * sin(u)

                // add (sectorCount+1) vertices per stack
                // first and last vertices have same position and normal, but different tex coords
                for (int j = 0; j <= SPHERE_SUBDIVISIONS; ++j)
                {
                    sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                    // vertex position (x, y, z)
                    float x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
                    float y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
                    
                    // normalized vertex normal (nx, ny, nz)
                    float nx = x / radius;
                    float ny = y / radius;
                    float nz = z / radius;

                    // vertex tex coord (s, t) range between [0, 1]
                    float s = (float)j / SPHERE_SUBDIVISIONS;
                    float t = (float)i / SPHERE_SUBDIVISIONS;

                    vertices.push_back({ {x, y, z}, {nx, ny, nz}, {s, t} });
                }
            }

            int k1, k2;
            for (int i = 0; i < SPHERE_SUBDIVISIONS; ++i)
            {
                k1 = i * (SPHERE_SUBDIVISIONS + 1);     // beginning of current stack
                k2 = k1 + SPHERE_SUBDIVISIONS + 1;      // beginning of next stack

                for (int j = 0; j < SPHERE_SUBDIVISIONS; ++j, ++k1, ++k2)
                {
                    // 2 triangles per sector excluding first and last stacks
                    // k1 => k2 => k1+1
                    if (i != 0)
                    {
                        indices.push_back(k1);
                        indices.push_back(k2);
                        indices.push_back(k1 + 1);
                    }

                    // k1+1 => k2 => k2+1
                    if (i != (SPHERE_SUBDIVISIONS - 1))
                    {
                        indices.push_back(k1 + 1);
                        indices.push_back(k2);
                        indices.push_back(k2 + 1);
                    }
                }
            }

            m_SphereMesh.vbo.BufferData(sizeof(V3N3T2) * vertices.size(), vertices.data(), false);
            m_SphereMesh.ebo.BufferData(sizeof(GLuint) * indices.size(), indices.data(), false);

            m_SphereMesh.vao.Attribute(m_SphereMesh.vbo, 0, 3, GL_FLOAT, sizeof(V3N3T2), 0);
            m_SphereMesh.vao.Attribute(m_SphereMesh.vbo, 1, 3, GL_FLOAT, sizeof(V3N3T2), sizeof(glm::vec3));
            m_SphereMesh.vao.Attribute(m_SphereMesh.vbo, 2, 2, GL_FLOAT, sizeof(V3N3T2), 2 * sizeof(glm::vec3));

            m_SphereMesh.indices = indices;
        }

        void Draw(const Shader& shader)
        {
            if (!m_IsActive) { return; }

            m_SphereTexture.Activate(GL_TEXTURE0);

            shader.SetInt("u_Texture_Diffuse", 0);
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, m_Position);
            model = glm::scale(model, glm::vec3(m_Radius, m_Radius, m_Radius));
            shader.SetMat4("u_Model", model);
            /*
            shader.SetBool("u_IsTexture", false);
            shader.SetVec3("u_Color", m_Color);
            */

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