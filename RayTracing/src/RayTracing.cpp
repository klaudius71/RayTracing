#include "RayTracing.h"
#include <algorithm>
#include <execution>

RayTracing::RayTracing(const int width, const int height, const char* const icon_path)
	: App(width, height, true, icon_path), img1(nullptr), x_tex(nullptr), square_tex(nullptr), line_tex(nullptr), font(nullptr), pixels(nullptr)
{
}

void RayTracing::Start()
{
	img1 = new Iceberg::Image();
	x_tex = new Iceberg::Texture("assets/textures/x.png");
	square_tex = new Iceberg::Texture("assets/textures/square.png");
	line_tex = new Iceberg::Texture("assets/textures/line.png");
	minimize_tex = new Iceberg::Texture("assets/textures/minimize.png");
	pixels = new uint32_t[img1->GetWidth() * img1->GetHeight()];
	
	font = ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Medium.ttf", 24);

	prevTime = std::chrono::steady_clock::now();
	deltaTimes.resize(200);
}

void RayTracing::Update()
{
	auto curr_time = std::chrono::steady_clock::now();
	std::chrono::duration<float> delta = curr_time - prevTime;
	dt = delta.count() * 1000.0f;
	prevTime = curr_time;

	deltaTimes[deltaTimesIndex++] = dt;
	deltaTimesIndex %= 100;

	glfwSetWindowCaptionArea(App::GetWindow()->GetGLFWWindow(), 0, 0, App::GetWindow()->GetWindowWidth() - 175, 30);

	const Iceberg::Window* wind = GetWindow();
	
	ImGuiIO& io = ImGui::GetIO();
	
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0.0f, 10.0f });
	if (ImGui::BeginMainMenuBar())
	{
		bool maximized = wind->IsMaximized();
	
		ImGui::PushFont(font);
		ImGui::SetCursorPosY(-7.0f);
		ImGui::Text("RayTracing");
		ImGui::PopFont();
	
		float offset = ImGui::GetWindowWidth() - (ImGui::GetWindowHeight() + 20.0f + 4.0f);
		ImGui::SetCursorPosX(offset);
		ImGui::SetCursorPosY(-3.0f);
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 20.0f, 10.0f });
		if (ImGui::ImageButton("Exit", x_tex->GetDescriptorSet(), ImVec2{ImGui::GetWindowHeight() * 0.6f , ImGui::GetWindowHeight() * 0.6f }))
			glfwSetWindowShouldClose(wind->GetGLFWWindow(), GLFW_TRUE);
		
		ImGui::SetCursorPosX(offset - (ImGui::GetWindowHeight() + 20.0f + 4.0f));
		ImGui::SetCursorPosY(-3.0f);
		ImTextureID tex = wind->IsMaximized() ? minimize_tex->GetDescriptorSet() : square_tex->GetDescriptorSet();
		
		if (ImGui::ImageButton("Maximize", tex, ImVec2{ ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f }))
		{
			if (wind->IsMaximized())
				glfwRestoreWindow(wind->GetGLFWWindow());
			else
				glfwMaximizeWindow(wind->GetGLFWWindow());
		}
		
		ImGui::SetCursorPosX(offset - 2 * (ImGui::GetWindowHeight() + 20.0f + 4.0f));
		ImGui::SetCursorPosY(-3.0f);
		if (ImGui::ImageButton("Minimize", line_tex->GetDescriptorSet(), ImVec2{ ImGui::GetWindowHeight() * 0.6f, ImGui::GetWindowHeight() * 0.6f }))
		{
			glfwIconifyWindow(wind->GetGLFWWindow());
		}
		
		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	
		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleVar();
	
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });
	ImGui::Begin("Image");
	ImVec2 window_size = ImGui::GetContentRegionAvail();
#if _DEBUG
	this->Resize((uint32_t)window_size.x / 2, (uint32_t)window_size.y / 2);
#else
	this->Resize((uint32_t)window_size.x, (uint32_t)window_size.y);
#endif
	this->RenderFrame();
	ImGui::Image(img1->GetImGuiTexture(), window_size);
	ImGui::End();
	ImGui::PopStyleVar();
	
	ImGui::Begin("Scene");
	ImGui::Text("Frame Time: %.3fms", std::accumulate(deltaTimes.cbegin(), deltaTimes.cend(), 0.0f) / 100.0f);
	ImGui::Text("Image Size:");
	ImGui::BulletText("Width: %d", img1->GetWidth());
	ImGui::BulletText("Height: %d", img1->GetHeight());
	ImGui::DragFloat3("Sphere Position", glm::value_ptr(spherePos), 0.01f);
	ImGui::DragFloat("Sphere Radius", &sphereRadius, 0.01f, 0.01f, 1000.0f);
	ImGui::End();
}

void RayTracing::End()
{
	delete img1;
	delete x_tex;
	delete square_tex;
	delete line_tex;
	delete minimize_tex;
	delete pixels;
}

void RayTracing::RenderFrame()
{
	std::for_each(std::execution::par, heights.cbegin(), heights.cend(), [&](uint32_t y)
	{
		for (uint32_t x = 0; x < img1->GetWidth(); x++)
		{
			glm::vec2 fragCoords{ (float)x / img1->GetWidth(), (float)y / img1->GetHeight() };
			fragCoords *= 2.0f;
			fragCoords -= 1.0f;
			fragCoords.y = -fragCoords.y;

			glm::vec4 color = PerPixel(fragCoords);
			color.r = glm::clamp(color.r, 0.0f, 1.0f);
			color.g = glm::clamp(color.g, 0.0f, 1.0f);
			color.b = glm::clamp(color.b, 0.0f, 1.0f);

			pixels[y * img1->GetWidth() + x] = ConvertToRGBA(color);
		}
	});
	img1->SetData(pixels, img1->GetWidth() * img1->GetHeight() * sizeof(uint32_t));
}

uint32_t RayTracing::ConvertToRGBA(const glm::vec4& color)
{
	return (uint8_t)(color.a * 255.0f) << 24 | (uint8_t)(color.b * 255.0f) << 16 | (uint8_t)(color.g * 255.0f) << 8 | (uint8_t)(color.r * 255.0f);
}

glm::vec4 RayTracing::PerPixel(glm::vec2 fragCoords)
{
	glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
	glm::vec3 rayDirection(fragCoords.x, fragCoords.y, -1.0f);

	glm::vec3 oc = rayOrigin - spherePos;

	float a = glm::dot(rayDirection, rayDirection);
	float b = 2.0f * glm::dot(oc, rayDirection);
	float c = glm::dot(oc, oc) - sphereRadius * sphereRadius;
	float discriminant = b * b - 4.0f * a * c;
	if (discriminant >= 0.0f)
	{
		float t = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		glm::vec3 hitPoint = rayOrigin + rayDirection * t;
		glm::vec3 normal = glm::normalize(hitPoint - spherePos);
		
		const glm::vec3 lightDir(glm::normalize(glm::vec3(-1, -1, -1)));
		
		float d = glm::max(glm::dot(normal, -lightDir), 0.0f);
		
		glm::vec3 sphereColor = glm::vec3(normal * 0.5f + 0.5f);
		sphereColor *= d;
		return glm::vec4(sphereColor, 1.0f);
	}

	return glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f};
}

void RayTracing::Resize(uint32_t width, uint32_t height)
{
	if (img1->GetWidth() == width && img1->GetHeight() == height)
		return;

	heights.resize(height);
	for (uint32_t i = 0; i < height; i++)
		heights[i] = i;

	img1->Resize(width, height);
	delete[] pixels;
	pixels = new uint32_t[img1->GetWidth() * img1->GetHeight()];
}