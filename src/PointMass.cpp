#include "PointMass.h"

PointMass::PointMass(const glm::vec3& position)
	: position(position)
{

}

void PointMass::Reset()
{
	velocity = glm::vec3(0.0f, 0.0f, 0.0f);
	acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
	force = glm::vec3(0.0f, 0.0f, 0.0f);

	isActive = true;
}

void PointMass::UpdatePosition(const float& dt)
{
	prevPosition = position;

	// Euler-Method
	/*
	velocity += dt * acceleration;
	position += dt * velocity;
	*/

	float invMass = 1.0f / mass;
	glm::vec3 k1, k2, k3, k4;

	// Compute k1
	glm::vec3 acceleration = force * invMass;
	k1 = acceleration * dt;

	// Compute k2
	glm::vec3 tempForce = force + k1 * 0.5f;
	acceleration = tempForce * invMass;
	k2 = acceleration * dt;

	// Compute k3
	tempForce = force + k2 * 0.5f;
	acceleration = tempForce * invMass;
	k3 = acceleration * dt;

	// Compute k4
	tempForce = force + k3;
	acceleration = tempForce * invMass;
	k4 = acceleration * dt;

	// Combine to get the new velocity
	glm::vec3 deltaVelocity = (k1 + (k2 * 2.0f) + (k3 * 2.0f) + k4) * (1.0f/6.0f);
	velocity = velocity + deltaVelocity;

	// Update position based on the new velocity
	glm::vec3 deltaPosition = velocity * dt;
	position += deltaPosition;

}
void PointMass::AddNormal(const glm::vec3& normal)
{
	this->normal = 0.5f * (this->normal + normal);
}