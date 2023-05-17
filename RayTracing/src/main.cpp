#include "RayTracing.h"
#include <exception>

struct MemMarker
{
	~MemMarker()
	{

	}
} mem_marker;

int main()
{
	RayTracing* sandbox = new RayTracing(1600, 900, nullptr);
	sandbox->Run();
	delete sandbox;
}