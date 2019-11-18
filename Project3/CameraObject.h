#pragma once

#define GLM_FORCE_RADIANS

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

const float PI = 3.14159265359f;
const float RADIAN = PI / 180.0f;

class CameraObject
{
private:
	// Camera Vectors
	glm::vec4 v_n;
	glm::vec4 v_u;
	glm::vec4 v_v;

	glm::vec4 v_up;

	// Camera Points
//	glm::vec4 m_lookat_fixed;
	glm::vec4 m_lookat;
	glm::vec4 m_eye;
	float m_distance;

	float m_URot;
	float m_VRot;
	float m_NRot;

	float m_OrbitAngle;

	glm::mat4x4 matrixM;

private:
	void CalculateCameraVectors();
	glm::vec4 Cross4d(glm::vec4, glm::vec4);

public:
	CameraObject(float,
				 float,
				 float,
				 float,
				 float,
				 float);
	~CameraObject();

	void GetCameraData(float&,
					   float&,
					   float&,
					   float&,
					   float&,
					   float&,
					   float&,
					   float&,
					   float&);

	void MoveVecN(float fDistance);
	void MoveVecV(float fDistance);
	void MoveVecU(float fSistance);

	void RotateYaw(float fAngle);
	void RotatePitch(float fAngle);
	void CameraObject::Orbit(float fAngle);
	void Zoom(int iZoom);

	static void Print(glm::mat4x4);

private:
	glm::mat4x4 GetTranslationMatrix(float fX, float fY, float fZ);
	glm::mat4x4 GetRotationMatrix(float fU, float fV, float fN);
	void CalculateRotation(float fU, float fV, float fN);

};

