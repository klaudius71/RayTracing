#ifndef _RAYTRACING
#define _RAYTRACING

#include "IcebergAPI.h"

class RayTracing : public Iceberg::App
{
public:
	RayTracing(const int width, const int height, const char* const icon_path);
	RayTracing(const RayTracing&) = delete;
	RayTracing& operator=(const RayTracing&) = delete;
	RayTracing(RayTracing&&) = delete;
	RayTracing& operator=(RayTracing&&) = delete;
	~RayTracing() = default;

	virtual void Start() override;
	virtual void Update() override;
	virtual void End() override;

	void RenderFrame();

	static uint32_t ConvertToRGBA(const glm::vec4& color);

private:
	glm::vec4 PerPixel(glm::vec2 fragCoords);
	void Resize(uint32_t width, uint32_t height);

	std::chrono::steady_clock::time_point prevTime;
	float dt;
	std::vector<float> deltaTimes;
	uint32_t deltaTimesIndex = 0;

	std::vector<uint32_t> heights;
	glm::vec3 spherePos{ 0.0f, 0.0f, 0.0f };
	float sphereRadius = 0.5f;

	uint32_t* pixels;
	ImFont* font;
	Iceberg::Image* img1 = nullptr;
	Iceberg::Texture* x_tex = nullptr;
	Iceberg::Texture* square_tex = nullptr;
	Iceberg::Texture* line_tex = nullptr;
	Iceberg::Texture* minimize_tex = nullptr;
};

#endif
