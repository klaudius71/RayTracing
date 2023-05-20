#include "RayTracing.h"

struct MemMarker
{
	~MemMarker()
	{

	}
} mem_marker;

int main()
{
	RayTracing* sandbox = new RayTracing(1920, 1080, nullptr);
	sandbox->Run();
	delete sandbox;
}