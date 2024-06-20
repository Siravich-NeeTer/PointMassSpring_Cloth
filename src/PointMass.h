#pragma once

#include <utility>
#include <glm/glm.hpp>

enum NeighborType
{
	STRUCTURAL_X,
	STRUCTURAL_Y,
	SHEAR,
	FLEXION_X,
	FLEXION_Y
};

class PointMass
{
	public:
		glm::vec3 prevPosition = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 position;
		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 force = glm::vec3(0.0f, 0.0f, 0.0f);

		bool isActive = true;

		std::pair<PointMass*, NeighborType> neighborList[12];
		size_t neighborSize = 0;

		PointMass(const glm::vec3 &position);

		void Reset();

		void UpdatePosition(const float& dt);
		void AddNormal(const glm::vec3& normal);
};