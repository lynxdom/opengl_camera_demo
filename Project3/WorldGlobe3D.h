#pragma once

#include <chrono>
#include <vector>
#include "BMPReader.h"

class WorldGlobe3D
{
private:
	struct Vertex
	{
		float dX;
		float dY;
		float dZ;

		float fR;
		float fG;
		float fB;
	};

	std::vector<Vertex> m_GlobeSphere;
	std::vector<Vertex> m_GlobeOutlineSphere;

	bool CreateWireFrameGlobe();
	bool CreateSolidGlobe();

	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;

	std::chrono::high_resolution_clock::time_point m_start;

	const float radius = 5;

	float m_Rotation = 0;

public:
	WorldGlobe3D();
	~WorldGlobe3D();

	void Draw();
};

