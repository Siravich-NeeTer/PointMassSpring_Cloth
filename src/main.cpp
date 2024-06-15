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

bool startSimulate = false;
bool isCameraMove = false;
bool renderType = false;

bool isRenderSphere = true;

FPSCounter fpsCounter;

void UpdateUIMenu(Cloth& cloth, Sphere& sphere, Floor& floor);\
// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
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

int main()
{
	Window window(SCREEN_WIDTH, SCREEN_HEIGHT, "Cloth_Simulation");
	window.Init();

	SkyBox skybox;

	Cloth cloth(m, samplerAmount, length);
	Sphere sphere({ length / 2.0f, -1.0f, length / 2.0f });
	Floor floor;
	floor.GetPosition() = { length / 2.0f, -5.0f, length / 2.0f };
	floor.scale = { 10.0f, 0.3f, 10.0f };

	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");

	if (window.IsNull())
	{
		std::cerr << "Error(WINDOW_NULL) : Cannot open window \n";
		return -1;
	}

	Camera cam(glm::vec3(0,0,8));
	Shader normalShader("Shader/objectVertex.shader", "Shader/objectFragment.shader");
	Shader skyboxShader("Shader/SkyBoxVertex.shader", "Shader/SkyBoxFragment.shader");
	skybox.SetTexture({
		"Texture/right.jpg",
		"Texture/left.jpg",
		"Texture/top.jpg",
		"Texture/bottom.jpg",
		"Texture/front.jpg",
		"Texture/back.jpg"
		});
	Shader geometryPassShader("Shader/gBufferVertex.shader", "Shader/gBufferFragment.shader");
	Shader deferredShader("Shader/deferredVertex.shader", "Shader/deferredFragment.shader");

	// -------------- Initialize G-Buffer Frambuffer --------------
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedoSpec;
	// Position Color Buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// Normal Color Buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// Color + Specular Color Buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	// Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// Create and Attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "[ERROR] Framebuffer not complete!\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// --------------- Light Configuration ----------------
	glm::vec3 lightPos[3] = 
	{
		glm::vec3(length / 2.0f, 3.0f, length / 2.0f),
		glm::vec3(0.0f, 0.0f, length / 2.0f),
		glm::vec3(length, 0.0f, length / 2.0f),
	};
	glm::vec3 lightColor[3] =
	{
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(1.0f),
	};

	// --------------- Shader Configuration ---------------
	deferredShader.Activate();
	deferredShader.SetInt("gPosition", 0);
	deferredShader.SetInt("gNormal", 1);
	deferredShader.SetInt("gAlbedoSpec", 2);

	// --------------- Game Loop ---------------
	float prevTime = 0.0f;
	float dt = 0.0f;
	while (!window.ShouldClose())
	{
		float currentTime = glfwGetTime();
		//dt = currentTime - prevTime;
		dt = TimeStep;
		window.PollEvents();

		fpsCounter.Update(currentTime - prevTime);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (Input::isKeyPressed(GLFW_KEY_Z))
			sphere.DecreaseZ(dt);
		if (Input::isKeyPressed(GLFW_KEY_X))
			sphere.IncreaseZ(dt);
		if (Input::isKeyPressed(GLFW_KEY_C))
			sphere.DecreaseY(dt);
		if (Input::isKeyPressed(GLFW_KEY_V))
			sphere.IncreaseY(dt);

		if(isCameraMove)
			cam.ProcessMousesMovement();
		cam.Input(currentTime - prevTime);

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

		glm::mat4 view = cam.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), window.GetWidth() / window.GetHeight(), 0.1f, 100.0f);

		// ------------------ Update Process ------------------
		if(startSimulate) 
			cloth.UpdateForce(dt, sphere, floor);
		
		// ------------------ Render gBuffer ------------------
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		geometryPassShader.Activate();
		geometryPassShader.SetMat4("u_Projection", projection);
		geometryPassShader.SetMat4("u_View", view);

		//sphere.Draw(geometryPassShader);
		floor.Draw(geometryPassShader);
		cloth.DrawTexture(cam, geometryPassShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// ------------------ Render Lighting ------------------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		deferredShader.Activate();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
		// send light relevant uniforms
		for (unsigned int i = 0; i < 3; i++)
		{
			deferredShader.SetVec3("lights[" + std::to_string(i) + "].Position", lightPos[i]);
			deferredShader.SetVec3("lights[" + std::to_string(i) + "].Color", lightColor[i]);
			// update attenuation parameters and calculate radius
			const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
			const float linear = 0.7f;
			const float quadratic = 1.8f;
			deferredShader.SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
			deferredShader.SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
			// then calculate radius of light volume/sphere
			const float maxBrightness = std::fmaxf(std::fmaxf(lightColor[i].r, lightColor[i].g), lightColor[i].b);
			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
			deferredShader.SetFloat("lights[" + std::to_string(i) + "].Radius", radius);
		}
		deferredShader.SetVec3("viewPos", cam.GetPosition());
		// finally render quad
		renderQuad();

		// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
		// ----------------------------------------------------------------------------------
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
		// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
		// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
		glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		// Draw Normal Object
		/*
		normalShader.Activate();

		normalShader.SetMat4("u_View", view);
		normalShader.SetMat4("u_Projection", projection);
		normalShader.SetBool("u_DoLight", false);


		if(renderType)
			cloth.DrawWireframe(normalShader);
		else
			cloth.DrawTexture(cam, normalShader);
		*/

		// - Draw SkyBox
		skyboxShader.Activate();
		skyboxShader.SetMat4("u_View", glm::mat4(glm::mat3(view)));
		skyboxShader.SetMat4("u_Projection", projection);

		skybox.Draw(skyboxShader);

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