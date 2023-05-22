#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <iostream>

#include "Input.h"

class Window
{
	private:
		GLFWwindow* m_Window;
		int m_Width;
		int m_Height;
		const char* m_Title;

	public:
		Window(const int &width, const int &height, const char* title);
		
		void MakeContextCurrent();
		void PollEvents();
		void SwapBuffers();

		void Close();
		bool IsNull();
		bool ShouldClose();

		// Setter
		void SetWidth(const int& width);
		void SetHeight(const int& height);
		void SetTitle(const char* title);

		// Getter
		GLFWwindow* GetWindow();

		// Core-Loop
		void Init();
		void Destroy();
};