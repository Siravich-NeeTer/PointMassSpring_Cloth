#include <iostream>
#include <string>

#include "Window.h"
#include "Renderer/Shader.h"
#include "Camera.h"

#include "Cloth.h"
#include "PointMass.h"
#include "Sphere.h"
#include "Floor.h"
#include "Constant.h"

#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Screen Size
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

bool startSimulate = false;
bool isCameraMove = false;
bool renderType = false;

bool isRenderSphere = true;

void UpdateUIMenu(Cloth& cloth, Sphere& sphere, Floor& floor);

int main()
{
	Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "Cloth_Simulation");
	window.Init();

	Cloth cloth(m, samplerAmount, length);
	Sphere sphere({ length / 2.0f, -1.0f, length / 2.0f });
	Floor floor;
	floor.GetPosition() = { length / 2.0f, -5.0f, length / 2.0f };
	floor.scale = { 10.0f, 0.3f, 10.0f };

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 330");

	if (window.IsNull())
	{
		std::cerr << "Error(WINDOW_NULL) : Cannot open window \n";
		return -1;
	}

	Camera cam(glm::vec3(0,0,8));
	Shader shader("Shader/objectVertex.shader", "Shader/objectFragment.shader");

	// --------------- Game Loop ---------------
	float prevTime = 0.0f;
	float dt = 0.0f;
	while (!window.ShouldClose())
	{
		float currentTime = glfwGetTime();
		dt = currentTime - prevTime;
		window.PollEvents();

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Input::isKeyPressed(GLFW_KEY_Z))
			sphere.DecreaseZ(TimeStep);
		if (Input::isKeyPressed(GLFW_KEY_X))
			sphere.IncreaseZ(TimeStep);
		if (Input::isKeyPressed(GLFW_KEY_C))
			sphere.DecreaseY(TimeStep);
		if (Input::isKeyPressed(GLFW_KEY_V))
			sphere.IncreaseY(TimeStep);

		if(isCameraMove)
			cam.ProcessMousesMovement();
		cam.Input(dt);

		if (Input::isKeyBeginPressed(GLFW_KEY_SPACE))
			startSimulate = true;
		if (Input::isKeyBeginPressed(GLFW_KEY_1))
			renderType = !renderType;
		if (Input::isKeyBeginPressed(GLFW_KEY_LEFT_ALT))
		{
			cam.ResetMousePosition();
			isCameraMove = !isCameraMove;
			if(isCameraMove)
				glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
			

		UpdateUIMenu(cloth, sphere, floor);

		// Run Application
		shader.Activate();

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
		view = cam.GetViewMatrix();

		shader.SetMat4("u_Projection", projection);
		shader.SetMat4("u_View", view);
		shader.SetBool("u_DoLight", false);

		if(startSimulate) cloth.UpdateForce(dt, sphere, floor);

		floor.Draw(shader);
		sphere.Draw(shader);
		if(renderType)
			cloth.DrawWireframe(shader);
		else
			cloth.DrawTexture(cam, shader);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Post - Render
		Input::endFrame();
		window.SwapBuffers();

		prevTime = currentTime;
	}

	// --------------- End Program ---------------
    // Clean Imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// Clean Application & Window
	window.Destroy();
}

void UpdateUIMenu(Cloth& cloth, Sphere& sphere, Floor& floor)
{
	ImGui::Begin("Cloth Simulation");

	if (ImGui::CollapsingHeader("Cloth"))
	{
		ImGui::Text("Cloth Stiffness");
		ImGui::SliderFloat("kx", &kx, 0.1f, 10.0f);
		ImGui::SliderFloat("ky", &ky, 0.1f, 10.0f);
	
		ImGui::Text("Gravity");
		ImGui::SliderFloat("g", &g, 1.0f, 20.0f);

		ImGui::Text("Cloth Resolution");
		ImGui::SliderInt("Resolution", &cloth.GetClothResolution(), 1, 50);
	
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