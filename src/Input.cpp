#include "Input.h"

namespace Input
{
	float mouseX;
	float mouseY;
	bool keyPressed[GLFW_KEY_LAST];
	bool keyBeginPressed[GLFW_KEY_LAST];

	bool isKeyPressed(const int& key)
	{
		return keyPressed[key];
	}

	bool isKeyBeginPressed(const int& key)
	{
		return keyBeginPressed[key];
	}

	void endFrame()
	{
		for (int idx = 0; idx < GLFW_KEY_LAST; idx++)
		{
			keyBeginPressed[idx] = false;
		}
	}

	void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// In case of : FN + F1 to FN + F12
		if (key < 0 || key >= GLFW_KEY_LAST) { return; }

		if (action == GLFW_PRESS)
		{
			keyBeginPressed[key] = true;
			keyPressed[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keyPressed[key] = false;
			keyBeginPressed[key] = false;
		}

		if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
		{
			glfwSetWindowShouldClose(window, GLFW_TRUE);
		}
	}

	void cursorCallBack(GLFWwindow* window, double xPos, double yPos)
	{
		mouseX = (float)xPos;
		mouseY = (float)yPos;
	}

	void mouseCallBack(GLFWwindow* window, int key, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			keyPressed[key] = true;
			keyBeginPressed[key] = true;
		}
		else if (action == GLFW_RELEASE)
		{
			keyPressed[key] = false;
			keyBeginPressed[key] = false;
		}
	}
}
