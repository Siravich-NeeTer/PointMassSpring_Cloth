#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <omp.h>

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

#include <bvh/v2/bvh.h>
#include <bvh/v2/vec.h>
#include <bvh/v2/ray.h>
#include <bvh/v2/node.h>
#include <bvh/v2/default_builder.h>
#include <bvh/v2/thread_pool.h>
#include <bvh/v2/executor.h>
#include <bvh/v2/stack.h>

#define SQRT_2 1.414213f

using Vec3 = bvh::v2::Vec<float, 3>;
using BBox = bvh::v2::BBox<float, 3>;
using Node = bvh::v2::Node<float, 3>;
using Bvh = bvh::v2::Bvh<Node>;
using Ray = bvh::v2::Ray<float, 3>;

class Cloth : public Object
{
	private:
		std::vector<glm::vec3> vertices = { glm::vec3(0,0,0), glm::vec3(1,1,1) };
		PointMass** m_PointMass;
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

		bvh::v2::ThreadPool thread_pool;
		bvh::v2::ParallelExecutor executor;
		Bvh bvh;
		bvh::v2::DefaultBuilder<Node>::Config Config;
		std::vector<BBox> BoxesList;
		std::vector<Vec3> CenterList;

		int GetIndex(const int& row, const int& column);
		int HashPosition(const glm::vec3& position);
		void BuildSpatialMap();

	public:
		bool useDiffuseColor = false;

		Cloth(const float& mass = 10.0f, const int& samplerAmount = 10, const float& clothSize = 10.0f);
		~Cloth();

		void Reset();
		void ResetPosition();
		void ResetComponent();

		void DrawWireframe(const Shader& shader);
		void DrawTexture(const Camera& camera, const Shader& shader);
		void UpdateForce(const float& dt);
		void UpdateCollision(const float& dt, const Sphere& sphere, const Floor& floor);

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

		void GetStructuralXAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const;
		void GetStructuralYAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const;
		void GetShearNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const;
		void GetFlexionXAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const;
		void GetFlexionYAxisNeighborPointMass(const int& row, const int& column, PointMass* pointMassList[], size_t& newSize) const;

		void UpdateBVH();
		void UpdateRaycast(const glm::vec3& rayOrg, const glm::vec3& rayDir, float &t, PointMass* &hitPointMass);
		bool RayAABB(Ray& ray, const BBox& aabb);
};