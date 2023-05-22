#pragma once

#include <string>

class FPSCounter
{
	private:
		float m_TimeCounter = 0.0f;

		int m_Frame = 0;
		int m_CurrentFPS = 0;


	public:
		void Update(const float& dt)
		{
			m_TimeCounter += dt;
			m_Frame++;

			if (m_TimeCounter >= 1.0f)
			{
				m_CurrentFPS = m_Frame;
				m_TimeCounter = 0.0f;
				m_Frame = 0;
			}
		}

		std::string GetFPS()
		{
			return std::to_string(m_CurrentFPS);
		}
};