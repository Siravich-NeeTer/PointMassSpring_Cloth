#pragma once

#include <iostream>
#include <string>

#include "Window.h"
#include "Renderer/Shader.h"
#include "Renderer/SkyBox.h"
#include "Camera.h"

#include "Cloth.h"
#include "PointMass.h"
#include "Sphere.h"
#include "Floor.h"
#include "Constant.h"

#include "FPSCounter.h"

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Screen Size
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

class ClothSimulationApp
{
	public:
		ClothSimulationApp();
		~ClothSimulationApp();

		void Init();
		void Run();
		void Render();
		
	private:
		Window window;
		Camera camera;
		FPSCounter fpsCounter;
		
		Shader normalShader;
		Shader skyboxShader;
		Shader geometryPassShader;
		Shader deferredShader;
		Shader shaderLightBox;

		// Rendering Objects
		SkyBox skybox;
		Cloth cloth;
		Sphere sphere;
		Floor floor;

		PointMass* hitPointMass;

		// Light Position
		glm::vec3 lightPos[5] =
		{
			glm::vec3(length / 2.0f, 3.0f, length / 2.0f),
			glm::vec3(0.0f, 0.0f, length / 2.0f),
			glm::vec3(length, 0.0f, length / 2.0f),
			glm::vec3(length / 2.0f, 0.0f, length),
			glm::vec3(length / 2.0f, 0.0f, 0.0f)
		};
		glm::vec3 lightColor[5] =
		{
			glm::vec3(1.0f),
			glm::vec3(1.0f),
			glm::vec3(1.0f),
			glm::vec3(1.0f),
			glm::vec3(1.0f)
		};

		// Rendering Technique
		// - Deferred Shading
		unsigned int gBuffer;
		unsigned int gPosition, gNormal, gAlbedoSpec;
		unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		unsigned int rboDepth;
		// -- Post Render
		unsigned int quadVAO = 0;
		unsigned int quadVBO = 0;
		unsigned int cubeVAO = 0;
		unsigned int cubeVBO = 0;

		// UI Information
		bool startSimulate = false;
		bool isCameraMove = false;
		bool drawType = false;
		bool isRenderSphere = true;
		const char* renderType[4] = { "Deferred", "Diffuse", "Position", "Normal"};
		const char* selectedRenderType = renderType[0];
		int selectedRenderTypeIndex = 0;
		bool isHideLight = true;
		// Time
		float prevTime = 0.0f;
		float dt = 0.0f;
		// Ray Casting
		struct Ray
		{
			glm::vec3 org;
			glm::vec3 dir;
			float t;
		};
		Ray ray;

		void InitImGui();

		void InitGBuffer();
		void UpdateGBuffer();

		void renderQuad()
		{
			if (quadVAO == 0)
			{
				float quadVertices[] = {
					// positions        // texture Coords
					-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
					-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
					 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
					 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
				};
				// setup plane VAO
				glGenVertexArrays(1, &quadVAO);
				glGenBuffers(1, &quadVBO);
				glBindVertexArray(quadVAO);
				glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			}
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);
		}
		void renderCube()
		{
			// initialize (if necessary)
			if (cubeVAO == 0)
			{
				float vertices[] = {
					// back face
					-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
					 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
					 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
					 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
					-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
					-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
					// front face
					-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
					 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
					 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
					 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
					-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
					-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
					// left face
					-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
					-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
					-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
					-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
					-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
					-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
					// right face
					 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
					 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
					 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
					 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
					 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
					 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
					 // bottom face
					 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
					  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
					  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
					  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
					 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
					 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
					 // top face
					 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
					  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
					  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
					  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
					 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
					 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
				};
				glGenVertexArrays(1, &cubeVAO);
				glGenBuffers(1, &cubeVBO);
				// fill buffer
				glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
				// link vertex attributes
				glBindVertexArray(cubeVAO);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindVertexArray(0);
			}
			// render Cube
			glBindVertexArray(cubeVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}
		void UpdateUIMenu(Cloth& cloth, Sphere& sphere, Floor& floor)
		{
			ImGui::Begin("Cloth Simulation");

			ImGui::Text("Input Instruction:");
			ImGui::Text("W/A/S/D : Moving Camera");
			ImGui::Text("Left ALT : Toggle Cursor Mode");
			ImGui::Text("1 : Cloth Wireframe Mode");
			ImGui::Text("2 : Toggle Render Mode");
			if (isCameraMove)
				ImGui::Text("Move Mouse : Rotate Camera");
			if (startSimulate)
			{
				ImGui::Text("Left Click : Pickup/Drag Cloth");
				ImGui::Text("Right Click : Cutting Cloth");
			}
			ImGui::NewLine();

			if (ImGui::CollapsingHeader("Cloth"))
			{
				ImGui::Text("Cloth Stiffness");
				ImGui::SliderFloat("kx", &kx, 0.1f, 10.0f);
				ImGui::SliderFloat("ky", &ky, 0.1f, 10.0f);

				ImGui::Text("Gravity");
				ImGui::SliderFloat("g", &g, 1.0f, 20.0f);

				ImGui::Text("Cloth Resolution");
				ImGui::SliderInt("Resolution", &cloth.GetClothResolution(), 1, 50);

				ImGui::Checkbox("Wireframe", &drawType);

				ImGui::Text("Wind Force");

				ImGui::Checkbox("x", &cloth.GetWindForceFlag().x);
				ImGui::SameLine();
				ImGui::SliderFloat("Coeff(x)", &cloth.GetWindForceCoeff().x, 0.0f, 1.0f);

				ImGui::Checkbox("y", &cloth.GetWindForceFlag().y);
				ImGui::SameLine();
				ImGui::SliderFloat("Coeff(y)", &cloth.GetWindForceCoeff().y, 0.0f, 1.0f);

				ImGui::Checkbox("z", &cloth.GetWindForceFlag().z);
				ImGui::SameLine();
				ImGui::SliderFloat("Coeff(z)", &cloth.GetWindForceCoeff().z, 0.0f, 1.0f);

				ImGui::Text("Pin Point");
				ImGui::Checkbox("1", &cloth.GetPinPoint(0));
				ImGui::SameLine(); ImGui::Checkbox("2", &cloth.GetPinPoint(1));
				ImGui::SameLine(); ImGui::Checkbox("3", &cloth.GetPinPoint(2));
				ImGui::SameLine(); ImGui::Checkbox("4", &cloth.GetPinPoint(3));

				ImGui::Text("Cloth Color");
				ImGui::Checkbox("Diffuse Color", &cloth.useDiffuseColor);
				ImGui::ColorEdit3("Color(Cloth)", &cloth.GetColor()[0]);

			}
			if (ImGui::CollapsingHeader("Sphere"))
			{
				ImGui::Checkbox("Active(Sphere)", &sphere.IsActive());
				ImGui::DragFloat3("Position(Sphere)", &sphere.GetPosition()[0], 0.01f);
				ImGui::ColorEdit3("Color(Sphere)", &sphere.GetColor()[0]);
			}
			if (ImGui::CollapsingHeader("Floor"))
			{
				ImGui::Checkbox("Active(Floor)", &floor.IsActive());
				ImGui::DragFloat3("Position(Floor)", &floor.GetPosition()[0], 0.01f);
				ImGui::ColorEdit3("Color(Floor)", &floor.GetColor()[0]);
			}

			ImGui::Text("Render Type");
			ImGui::SameLine();
			if (ImGui::BeginCombo("", selectedRenderType))
			{
				for (int n = 0; n < IM_ARRAYSIZE(renderType); n++)
				{
					bool is_selected = (selectedRenderType == renderType[n]); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(renderType[n], is_selected))
					{
						selectedRenderType = renderType[n];
						selectedRenderTypeIndex = n;
					}
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
				ImGui::EndCombo();
			}
			ImGui::Checkbox("Hide Light", &isHideLight);

			ImGui::Text(("FPS : " + fpsCounter.GetFPS()).c_str());

			if (ImGui::Button("Start"))
			{
				startSimulate = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Reset"))
			{
				ResetConstant();
				cloth.Reset();
				startSimulate = false;
			}

			ImGui::End();
		}

		void UpdateMousePicking(const glm::mat4& view, const glm::mat4 &proj);
};