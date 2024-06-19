#include "ClothSimulationApp.h"

ClothSimulationApp::ClothSimulationApp()
	:	window(SCREEN_WIDTH, SCREEN_HEIGHT, "Cloth_Simulation"),
		camera(glm::vec3(0, 0, 8)),

		normalShader("Shader/objectVertex.shader", "Shader/objectFragment.shader"),
		skyboxShader("Shader/SkyBoxVertex.shader", "Shader/SkyBoxFragment.shader"),
		geometryPassShader("Shader/gBufferVertex.shader", "Shader/gBufferFragment.shader"),
		deferredShader("Shader/deferredVertex.shader", "Shader/deferredFragment.shader"),
		shaderLightBox("Shader/lightBoxVertex.shader", "Shader/lightBoxFragment.shader"),

		cloth(m, samplerAmount, length),
		sphere({ length / 2.0f, -1.0f, length / 2.0f })
{
	InitImGui();

	// Init Skybox Textures
	skybox.SetTexture({
	"Texture/right.jpg",
	"Texture/left.jpg",
	"Texture/top.jpg",
	"Texture/bottom.jpg",
	"Texture/front.jpg",
	"Texture/back.jpg"
		});

	// Init Floor Components
	floor.GetPosition() = { length / 2.0f, -5.0f, length / 2.0f };
	floor.scale = { 10.0f, 0.3f, 10.0f };

	InitGBuffer();
	UpdateGBuffer();
	window.resizeEvents.push_back([this]()
	{
		UpdateGBuffer();
		Render();
	});
}
ClothSimulationApp::~ClothSimulationApp()
{
	// Destroy G-Buffer
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedoSpec);
	glDeleteFramebuffers(1, &gBuffer);

	// Clean Imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// Clean Application & Window
	window.Destroy();
}

void ClothSimulationApp::Init()
{

}
void ClothSimulationApp::Run()
{
	// --------------- Shader Configuration ---------------
	deferredShader.Activate();
	deferredShader.SetInt("gPosition", 0);
	deferredShader.SetInt("gNormal", 1);
	deferredShader.SetInt("gAlbedoSpec", 2);

	while (!window.ShouldClose())
	{
		Render();
	}
}
void ClothSimulationApp::Render()
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

	if (isCameraMove)
		camera.ProcessMousesMovement();
	camera.Input(currentTime - prevTime);

	if (Input::isKeyBeginPressed(GLFW_KEY_SPACE))
		startSimulate = true;
	if (Input::isKeyBeginPressed(GLFW_KEY_1))
		renderType = !renderType;
	if (Input::isKeyBeginPressed(GLFW_KEY_LEFT_ALT))
	{
		camera.ResetMousePosition();
		isCameraMove = !isCameraMove;
		if (isCameraMove)
			glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	UpdateUIMenu(cloth, sphere, floor);

	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), window.GetWidth() / window.GetHeight(), 0.1f, 100.0f);


	if (Input::isKeyBeginPressed(GLFW_MOUSE_BUTTON_1) && !isCameraMove)
	{
		UpdateMousePicking(view, projection);
	}

	// ------------------ Update Process ------------------
	if (startSimulate)
		cloth.UpdateForce(dt, sphere, floor);

	// ------------------ Render gBuffer ------------------
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	geometryPassShader.Activate();
	geometryPassShader.SetMat4("u_Projection", projection);
	geometryPassShader.SetMat4("u_View", view);

	sphere.Draw(geometryPassShader);
	floor.Draw(geometryPassShader);
	cloth.DrawTexture(camera, geometryPassShader);
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
	for (unsigned int i = 0; i < 5; i++)
	{
		deferredShader.SetVec3("lights[" + std::to_string(i) + "].Position", lightPos[i]);
		deferredShader.SetVec3("lights[" + std::to_string(i) + "].Color", lightColor[i]);
		// update attenuation parameters and calculate radius
		const float constant = 1.0f; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const float linear = 0.09f;
		const float quadratic = 0.032f;
		deferredShader.SetFloat("lights[" + std::to_string(i) + "].Linear", linear);
		deferredShader.SetFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(lightColor[i].r, lightColor[i].g), lightColor[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		deferredShader.SetFloat("lights[" + std::to_string(i) + "].Radius", radius);
	}
	deferredShader.SetVec3("viewPos", camera.GetPosition());
	// finally render quad
	renderQuad();

	// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
	// ----------------------------------------------------------------------------------
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
	// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
	// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
	glBlitFramebuffer(0, 0, window.GetWidth(), window.GetHeight(), 0, 0, window.GetWidth(), window.GetHeight(), GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 3. render lights on top of scene
	// --------------------------------
	shaderLightBox.Activate();
	shaderLightBox.SetMat4("projection", projection);
	shaderLightBox.SetMat4("view", view);
	for (unsigned int i = 0; i < 5; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos[i]);
		model = glm::scale(model, glm::vec3(0.125f));
		shaderLightBox.SetMat4("model", model);
		shaderLightBox.SetVec3("lightColor", lightColor[i]);
		renderCube();
	}


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

void ClothSimulationApp::InitImGui()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.GetWindow(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}
void ClothSimulationApp::InitGBuffer()
{
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glGenTextures(1, &gPosition);
	glGenTextures(1, &gNormal);
	glGenTextures(1, &gAlbedoSpec);
	glGenRenderbuffers(1, &rboDepth);
}
void ClothSimulationApp::UpdateGBuffer()
{
	// Position Color Buffer
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.GetWidth(), window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// Normal Color Buffer
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, window.GetWidth(), window.GetHeight(), 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// Color + Specular Color Buffer
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window.GetWidth(), window.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	
	// Draw Attachment from following Texture
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	// Create and Attach depth buffer (renderbuffer)
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.GetWidth(), window.GetHeight());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	
	// Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw std::runtime_error("[ERROR] UpdateGBuffer : Framebuffer not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ClothSimulationApp::UpdateMousePicking(const glm::mat4& view, const glm::mat4& proj)
{
	float x = (2.0f * Input::mouseX) / window.GetWidth() - 1.0f;
	float y = 1.0f - (2.0f * Input::mouseY) / window.GetHeight();
	float z = 1.0f;

	glm::vec4 rayClip = glm::vec4(x, y, -1.0f, 1.0f);
	glm::vec4 rayEye = glm::inverse(proj) * rayClip;
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0f, 0.0f);

	glm::vec3 rayStart = camera.GetPosition();
	glm::vec3 rayDir = glm::vec3(glm::inverse(view) * rayEye);
	rayDir = glm::normalize(rayDir);

	std::cout << rayDir.x << ", " << rayDir.y << ", " << rayDir.z << "\n";
}