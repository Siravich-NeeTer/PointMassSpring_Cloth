#pragma once

#include <iostream>
#include <vector>
#include <map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Renderer/Mesh.h"
#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/MeshUtility.h"

#include "Camera.h"

#include "Object.h"

#include "Constant.h"
#include "PointMass.h"
#include "Sphere.h"
#include "Floor.h"

#define SQRT_2 1.414213f

class Cloth : public Object
{
	private:
		std::vector<glm::vec3> vertices = { glm::vec3(0,0,0), glm::vec3(1,1,1) };
		std::vector<PointMass*> m_PointMass;
		mutable glm::bvec3 m_WindForceFlag = glm::bvec3(false);
		mutable glm::vec3 m_WindForceCoeff = glm::vec3(0.5f);
		mutable bool m_PinPoint[4] = { false, false, false, false };
		mutable int m_ClothResolution = 5;

		// PointMassMesh aka. CubeMesh (Drawing Each PointMass)
		struct PointMassMesh
		{
			VAO vao;
			VBO vbo;
			EBO ebo;

			std::vector<GLuint> indices;

		} m_PointMassMesh;
		// ClothMesh (Drawing Texture Cloth)
		struct ClothMesh
		{
			VAO vao;
			VBO vbo;
			EBO ebo;
		} m_ClothMesh;
		Texture m_ClothTexture;
		// Line Mesh (Drawing WireFrame)
		struct LineMesh
		{
			VAO vao;
			VBO vbo;
		} m_LineMesh;


		int m_SamplerAmount;

		// For Entire Object (All PointMass)
		float m_ClothSize;
		float m_Mass;

		// For each 1 PointMass
		float m_EachMass;
		float m_EachMassInvert;
		float m_EachLength;
		float m_EachClothResolution;

		float m_Time = 0.0f;

		std::map<int, std::vector<PointMass*>> m_SpatialMap;

		int GetIndex(const int& row, const int& column);
		int HashPosition(const glm::vec3& position);
		void BuildSpatialMap();

	public:
		glm::vec3 maxVelo;

		Cloth(const float& mass = 10.0f, const int& samplerAmount = 10, const float& clothSize = 10.0f);
		~Cloth();

		void Reset();
		void ResetPosition();
		void ResetComponent();

		void DrawWireframe(const Shader& shader);
		void DrawTexture(const Camera& camera, const Shader& shader);
		void UpdateForce(const float& dt, const Sphere& sphere, const Floor& floor);

		int GetSamplerAmount() const;
		float GetClothSize() const;
		float GetMass() const;
		glm::bvec3& GetWindForceFlag() const;
		glm::vec3& GetWindForceCoeff() const;
		bool& GetPinPoint(const int& index) const;
		int& GetClothResolution() const;
		PointMass* GetPointMass(const int& index) const;
		PointMass* GetPointMass(const int& row, const int& column) const;
		std::vector<PointMass*> GetStructuralXAxisNeighborPointMass(const int& row, const int& column) const;
		std::vector<PointMass*> GetStructuralYAxisNeighborPointMass(const int& row, const int& column) const;
		std::vector<PointMass*> GetShearNeighborPointMass(const int& row, const int& column) const;
		std::vector<PointMass*> GetFlexionXAxisNeighborPointMass(const int& row, const int& column) const;
		std::vector<PointMass*> GetFlexionYAxisNeighborPointMass(const int& row, const int& column) const;
};