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
	velocity += dt * acceleration;
	position += dt * velocity;
}
void PointMass::AddNormal(const glm::vec3& normal)
{
	this->normal = 0.5f * (this->normal + normal);
}