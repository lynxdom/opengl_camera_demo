#include "WorldGlobe3D.h"
#include "GL\freeglut.h"
#include <iostream>

const float PI = 3.14159265359f;

WorldGlobe3D::WorldGlobe3D()
{
	if (!CreateWireFrameGlobe() || !CreateSolidGlobe())
	{
		throw - 1;
	}


	m_start = Time::now();
}


WorldGlobe3D::~WorldGlobe3D()
{
}

// Pre caclulate the wireframe point cloud
bool WorldGlobe3D::CreateWireFrameGlobe()
{
	BMPReader *bmpReader = nullptr;

	try
	{
		bmpReader = new BMPReader("TBWM02_LB_BlankOutlineWorldMap.bmp");
	}
	catch (...)
	{
		::MessageBox(NULL, "The Texture was unable to be loaded.  \n Please verify 'TBWM02_LB_BlankOutlineWorldMap.bmp' is in the active directory.", "Error!", MB_OK);
		return false;
	}

	long lWidth = bmpReader->GetWidth() - 1;
	long lHeight = bmpReader->GetHeight() - 1;

	for (long v = lHeight; v > 1; --v)
	{
		// Height changes only on diffrent rows
		float phi = (PI / lHeight) * v;

		for (long u = 1; u < lWidth; ++u)
		{
			unsigned char cRed = 0,
						  cGreen = 0,
						  cBlue = 0;

			bmpReader->GetPixel(lWidth - u, lHeight - v, 
								cRed, 
								cGreen, 
								cBlue);

			if (!(	(cRed == 255) &&
					(cGreen == 255) &&
					(cBlue == 255)))
			{
				float theta = ((2 * PI) / lWidth) * u;

				float dX = std::cos(theta) * std::sin(phi) * radius;
				float dZ = std::sin(theta) * std::sin(phi) * radius;
				float dY = std::cos(phi) * radius;

				Vertex oVer;

				oVer.dX = dX;
				oVer.dY = dY;
				oVer.dZ = dZ;

				oVer.fR = 0.0f;
				oVer.fG = 1.0f;
				oVer.fB = 0.0f;

				m_GlobeOutlineSphere.push_back(oVer);
			}
		}
	}

	delete bmpReader;

	return true;
}

// Pre calculate the points for the solid globe.
bool WorldGlobe3D::CreateSolidGlobe()
{
	BMPReader *bmpReader = nullptr;

	try
	{
		bmpReader = new BMPReader("TBWM02_LB_BlankOutlineWorldMap.bmp");
	}
	catch (...)
	{
		::MessageBox(NULL, "The Texture was unable to be loaded.  \n Please verify 'TBWM02_LB_BlankOutlineWorldMap.bmp' is in the active directory.", "Error!", MB_OK);
		return false;
	}

	long lWidth = bmpReader->GetWidth() - 1;
	long lHeight = bmpReader->GetHeight() - 1;

	for (long v = 1; v < lHeight; ++v)
	{
		// Height changes only on diffrent rows
		float phi = (PI / lHeight) * v;

		for (long u = 1; u < lWidth; ++u)
		{
			unsigned char cRed = 0,
						  cGreen = 0,
						  cBlue = 0;

			bmpReader->GetPixel(lWidth - u, lHeight - v,
								cRed,
								cGreen,
								cBlue);

			float theta = ((2 * PI) / lWidth) * u;

			float dX = std::cos(theta) * std::sin(phi) * radius;
			float dZ = std::sin(theta) * std::sin(phi) * radius;
			float dY = std::cos(phi) * radius;

			Vertex oVer;

			oVer.dX = dX;
			oVer.dY = dY;
			oVer.dZ = dZ;

			oVer.fR = static_cast<float>(cRed) / 255.0f;
			oVer.fG = static_cast<float>(cGreen) / 255.0f;
			oVer.fB = static_cast<float>(cBlue) / 255.0f;

			m_GlobeSphere.push_back(oVer);
		}
	}

	delete bmpReader;

	return true;
}

void WorldGlobe3D::Draw()
{
	const float fCycleTime = 4.0;

	glPushMatrix();

	std::chrono::high_resolution_clock::time_point currentTime = Time::now();
	std::chrono::duration<double> diff = currentTime - m_start;
	
	double dProgress = ((diff.count() / fCycleTime) * radius * 2) - radius;

	if (diff.count() > fCycleTime + 1)
	{	
		m_start = Time::now();
	}



	float fSphereX = 17.5f;
	float fSphereY = 15.0f;
	float fSphereZ = -17.5f;

	glTranslated(fSphereX, fSphereY, fSphereZ);
	glRotatef(12.0f, 0.0f, 0.0f, 1.0f);
	glRotatef(m_Rotation, 0.0f, 1.0f, 0.0f);

	

	long lSizeSolid = m_GlobeSphere.size();
	long lSizeWire = m_GlobeOutlineSphere.size();

	glPointSize(1);

	glBegin(GL_POINTS);
	{
		for (long i = 0; i < lSizeWire; ++i)
		{
			Vertex ver = m_GlobeOutlineSphere[i];

			if ((ver.dY < dProgress) &&
				(ver.dY > dProgress - 1.90))
			{
				

				glColor3f(ver.fR,
					ver.fG,
					ver.fB);

				glVertex3d(ver.dX,
					ver.dY,
					ver.dZ);
			}
		}
	}
	glEnd();

	glPointSize(4);

	glBegin(GL_POINTS);
	{
		for (long i = 0; i < lSizeSolid; ++i)
		{
			Vertex ver = m_GlobeSphere[i];


			if ((ver.dY > dProgress) ||
				(ver.dY < dProgress - 1.90))
			{
				glPointSize(3);

				glColor3f(ver.fR,
							ver.fG,
							ver.fB);

				glVertex3d(ver.dX,
							ver.dY,
							ver.dZ);
			}
		}
	}
	glEnd();

	m_Rotation += 0.5;

	if (m_Rotation > 359)
	{
		m_Rotation = 0;
	}

	glPopMatrix();
}

