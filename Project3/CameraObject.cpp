#include "CameraObject.h"
#include <iostream>
#include <iomanip>

CameraObject::CameraObject(float dX_eye,
						   float dY_eye,
						   float dZ_eye,
						   float dX_lookAt,
						   float dY_lookAt,
						   float dZ_lookAt) : m_lookat(dX_lookAt, dY_lookAt, dZ_lookAt, 1.0),
											   m_eye(dX_eye, dY_eye, dZ_eye, 1.0),
											   v_up(0.0, 1.0, 0.0, 0.0),
											   v_n(0.0, 0.0, 0.0, 0.0),
											   v_u(0.0, 0.0, 0.0, 0.0),
											   v_v(0.0, 0.0, 0.0, 0.0),
											   m_URot(0.0),
											   m_VRot(0.0),
											   m_NRot(0.0),
											   m_OrbitAngle(0.0)

{
	// Calculate the distance between the eye and lookat
	m_distance = std::sqrt(pow((m_eye.x - m_lookat.x), 2.0f) + pow((m_eye.y - m_lookat.y), 2.0f) + pow((m_eye.z - m_lookat.z), 2.0f));

	CalculateCameraVectors();

	// All vectors have been normalized so 
	// we can assume the magnatude is one for
	// angle calculations.
	glm::vec4 xAxis(1, 0, 0, 0);
	glm::vec4 yAxis(0, 1, 0, 0);
	glm::vec4 zAxis(0, 0, 1, 0);

	m_NRot = std::acos(glm::dot(zAxis, v_n));
	m_VRot = std::acos(glm::dot(yAxis, v_v));
	m_URot = std::acos(glm::dot(xAxis, v_u));
}


CameraObject::~CameraObject()
{
}

// Create N V U vectors based on 
// eye and lookat
void CameraObject::CalculateCameraVectors()
{
	v_n = glm::normalize(m_eye - m_lookat);
	v_u = glm::normalize(Cross4d(v_up, v_n));
	v_v = glm::normalize(Cross4d(v_n, v_u));

	m_lookat = m_eye + (-1.0f * v_n * m_distance);
}

// A custom Cross vector function that can handle
// homogonous vectors.
glm::vec4 CameraObject::Cross4d(glm::vec4 vec1, glm::vec4 vec2)
{
	glm::vec3 vec3_1 = glm::vec3(vec1.x, vec1.y, vec1.z);
	glm::vec3 vec3_2 = glm::vec3(vec2.x, vec2.y, vec2.z);

	glm::vec3 vec3Result = glm::cross(vec3_1, vec3_2);

	// Return homgenous vector
	return glm::vec4(vec3Result.x, vec3Result.y, vec3Result.z, 0);
}

// Return values of camera
void CameraObject::GetCameraData(float& dEye_x,
								 float& dEye_y,
								 float& dEye_z,
								 float& dLookat_x,
								 float& dLookat_y,
								 float& dLookat_z,
								 float& dUp_x,
								 float& dUp_y,
								 float& dUp_z)
{
	dEye_x = this->m_eye.x;
	dEye_y = this->m_eye.y;
	dEye_z = this->m_eye.z;

	dLookat_x = this->m_lookat.x;
	dLookat_y = this->m_lookat.y;
	dLookat_z = this->m_lookat.z;

	dUp_x = this->v_v.x;
	dUp_y = this->v_v.y;
	dUp_z = this->v_v.z;
}


// Move the camera and lookat along
// N vector
void CameraObject::MoveVecN(float fDistance)
{
	m_eye = m_eye + (v_n * fDistance);
	m_lookat = m_lookat + (v_n * fDistance);
}

// Move the camera and lookat along
// V vector
void CameraObject::MoveVecV(float fDistance)
{
	m_eye = m_eye + (v_v * fDistance);
	m_lookat = m_lookat + (v_v * fDistance);

}

// Move the camera and lookat along
// U vector
void CameraObject::MoveVecU(float fDistance)
{
	m_eye = m_eye + (v_u * fDistance);
	m_lookat = m_lookat + (v_u * fDistance);
}

// Rotate the camera around the lookat point.
void CameraObject::CalculateRotation(float fU, float fV, float fN)
{
	glm::mat4x4 nullMatrix({ 1.0, 0.0, 0.0, 0.0 },
						   { 0.0, 1.0, 0.0, 0.0 },
						   { 0.0, 0.0, 1.0, 0.0 },
						   { 0.0, 0.0, 0.0, 1.0 });

	glm::mat4x4 matRotation = GetRotationMatrix(fU, fV, fN);
	glm::mat4x4 matTransformOrigin = glm::translate(nullMatrix, glm::vec3(-m_eye.x, -m_eye.y, -m_eye.z));
	glm::mat4x4 matTransFormPosition = glm::translate(nullMatrix, glm::vec3(m_eye.x, m_eye.y, m_eye.z));

	m_lookat = matTransFormPosition * matRotation * matTransformOrigin * m_lookat;

	CalculateCameraVectors();
}

// Rotate the eye around the lookat point.
void CameraObject::Orbit(float fAngle)
{
	if (m_VRot > 2 * PI)  m_VRot = 0;
	if (m_VRot < 0) m_VRot = 2 * PI;

	m_OrbitAngle += fAngle;

	glm::mat4x4 indentityMatrix({ 1.0, 0.0, 0.0, 0.0 },
								{ 0.0, 1.0, 0.0, 0.0 },
								{ 0.0, 0.0, 1.0, 0.0 },
								{ 0.0, 0.0, 0.0, 1.0 });

	glm::vec3 vecRotAngle(this->v_v.x, this->v_v.y, this->v_v.z);

	glm::mat4x4 matRotation = glm::rotate(indentityMatrix, fAngle, vecRotAngle);
	glm::mat4x4 matTransformOrigin = glm::translate(indentityMatrix, glm::vec3(-m_lookat.x, -m_lookat.y, -m_lookat.z));
	glm::mat4x4 matTransFormPosition = glm::translate(indentityMatrix, glm::vec3(m_lookat.x, m_lookat.y, m_lookat.z));

	m_eye = matTransFormPosition * matRotation * matTransformOrigin * m_eye;

	fAngle = 0;

	CalculateCameraVectors();
}

// Change the distance between the eye
// and lookat.
void CameraObject::Zoom(int iZoom)
{
	m_distance -= iZoom;

	if (m_distance < 0.1f)
	{
		m_distance = 0.1f;
		return;
	}
	else if (m_distance > 25.0f)
	{
		m_distance = 25.0f;
		return;
	}

	m_eye = m_eye + (v_n * -static_cast<float>(iZoom));
}

void CameraObject::RotateYaw(float fAngle)
{
	m_VRot += fAngle;

	if (m_VRot > 2 * PI)  
			m_VRot = 0.0f;
	if (m_VRot < 0) 
			m_VRot = 2 * PI;

	CalculateRotation(0.0f, fAngle, 0.0f);
}

void CameraObject::RotatePitch(float fAngle)
{
	m_URot += fAngle;

	if (m_URot > 2 * PI)  
			m_URot = 0.0f;
	if (m_URot < 0) 
			m_URot = 2 * PI;

	CalculateRotation(fAngle, 0.0f, 0.0f);
}

// Get a composit rotation matrix using the parameters
glm::mat4x4 CameraObject::GetRotationMatrix(float fU, float fV, float fN)
{
	glm::mat4x4 indentityMatrix({ 1.0, 0.0, 0.0, 0.0 },
								{ 0.0, 1.0, 0.0, 0.0 },
								{ 0.0, 0.0, 1.0, 0.0 },
								{ 0.0, 0.0, 0.0, 1.0 });

	// Loaded the matrixes up seperatly for the sake of easy debuging.
	glm::mat4x4 matRot_x = glm::rotate(indentityMatrix, fU, glm::vec3(v_u.x, v_u.y, v_u.z));
	glm::mat4x4 matRot_y = glm::rotate(indentityMatrix, fV, glm::vec3(v_v.x, v_v.y, v_v.z));
	glm::mat4x4 matRot_z = glm::rotate(indentityMatrix, fN, glm::vec3(v_n.x, v_n.y, v_n.z));

	v_up = matRot_z * v_up;

	return matRot_z * matRot_y * matRot_x;
}

// Diagnostic function to print a mat4x4 to the console
void CameraObject::Print(glm::mat4x4 mat)
{
	std::cout ;
	std::cout << "[ " << std::setw(5) << mat[0][0] << ", " << std::setw(5) << mat[1][0] << ", " << std::setw(5) << mat[2][0] << ", " << std::setw(5) << mat[3][0] << "]" << std::endl;
	std::cout << "[ " << std::setw(5) << mat[0][1] << ", " << std::setw(5) << mat[1][1] << ", " << std::setw(5) << mat[2][1] << ", " << std::setw(5) << mat[3][1] << "]" << std::endl;
	std::cout << "[ " << std::setw(5) << mat[0][2] << ", " << std::setw(5) << mat[1][2] << ", " << std::setw(5) << mat[2][2] << ", " << std::setw(5) << mat[3][2] << "]" << std::endl;
	std::cout << "[ " << std::setw(5) << mat[0][3] << ", " << std::setw(5) << mat[1][3] << ", " << std::setw(5) << mat[2][3] << ", " << std::setw(5) << mat[3][3] << "]" << std::endl;
}