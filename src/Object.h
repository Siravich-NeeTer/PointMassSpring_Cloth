#pragma once

class Object
{
	protected:
		mutable bool m_IsActive = true;
		mutable glm::vec3 m_Position = glm::vec3(0.0f);
		mutable glm::vec3 m_Color = glm::vec3(1.0f);
	public:
		bool& IsActive() const
		{
			return m_IsActive;
		}
		glm::vec3& GetPosition() const 
		{
			return m_Position;
		}
		glm::vec3& GetColor() const
		{
			return m_Color;
		}
};