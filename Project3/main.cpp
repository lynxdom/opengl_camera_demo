//  Author : Sean Alexander
//  Date   : 11/02/2015
//  Desciption : Project 3

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "ChessSquare.h"
#include "WorldGlobe3D.h"

#include "CameraObject.h"

#include <iostream>
#include <chrono>

typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

GLint		g_iWidth;
GLint		g_iHeight;

CameraObject *pCameraObject = nullptr;

// Movment Mode
GLboolean	g_bDrag = false;
GLboolean	g_bOrbit = false;
GLboolean	g_bPan = false;

// Event Control
// 0 - Stoped
// 1 - Start
// 2 - Lift
// 3 - Rotate
// 4 - drop
GLint	iBoardState = 0;

// Running mouse mode
GLint		g_iMousex = 0;
GLint		g_iMousey = 0;

// GLUT Call backs
void Display();
void Reshape(int iWidth, int iHeight);
void Init();
void ExitProgram();
void KeyboardSpecialEvent();
void KeyboardEvent(unsigned char cKey, int iX, int iY);
void MouseEventFunc(int button, int state, int x, int y);
void MouseMotionFunc(int x, int y);
void MouseWheel(int button, int dir, int x, int y);
void Idle(void);

void BuildChessBoardWhite();
void BuildChessBoardBlack();
void DrawReferenceBlocks();

void ChessMovement();

// Background
void SetupBackground();
void DrawBackgroung();
GLUquadricObj *qObj = gluNewQuadric();
GLuint iStarmap;

ChessSquare *chessWhiteSquare = nullptr;
ChessSquare *chessBlackSquare = nullptr;
WorldGlobe3D *worldGlobe3d = nullptr;

// Exit the program releasing the 
// memory and objects.
void ExitProgram()
{
	//Clean up the objects
	delete chessWhiteSquare;
	delete chessBlackSquare;
	delete worldGlobe3d;

	gluDeleteQuadric(qObj);

	delete pCameraObject;

	exit(0);
}

void Init()
{
	g_iWidth = 1024;
	g_iHeight = 768;

	pCameraObject = new CameraObject(0.0, 0.0, 45.0,
									 0.0, 0.0, -15.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Idle(void)
{
	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	// initilize the program
	Init();

	// initilize glut
	glutInit(&argc, argv);


	//set the initial window size */
	glutInitWindowSize((int)g_iWidth, (int)g_iHeight);

	//create the window
	glutCreateWindow("Project 3");

	// initilize glew
	GLenum glError = glewInit();
	if (GLEW_OK != glError)
	{
		// Glew couldn't initilize for some bizzare reason.
		std::cout << glewGetErrorString(glError);
		return -1;
	}

	// specify as double bufferred can make the Display faster
	// Color is speicfied to RGBA, four color channels with Red, Green, Blue and Alpha(depth)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	// GLUT callbacks
	glutReshapeFunc(Reshape);
	glutDisplayFunc(Display);
	glutKeyboardFunc(KeyboardEvent);
	glutIdleFunc(Idle);

	// Glut Mouse
	glutMouseFunc(MouseEventFunc);
	glutMotionFunc(MouseMotionFunc);
	glutMouseWheelFunc(MouseWheel);

	try
	{
		//Set up objects and enviroment
		chessWhiteSquare = new ChessSquare("WhiteMarble512x512.bmp");
		chessBlackSquare = new ChessSquare("blue-marble512x512.bmp");

		//Build globe
		worldGlobe3d = new WorldGlobe3D();
	}
	catch (...)
	{
		// Textrue didn't load.
		// message box is handled in the object.
		ExitProgram();
	}

	SetupBackground();

	// Display Loop
	glutMainLoop();
}

void Reshape(int iWidth, int iHeight)
{
	// Set the globals
	g_iHeight = iHeight;
	g_iWidth = iWidth;

	double dAspectRatio = static_cast<GLdouble>(g_iWidth) / static_cast<GLdouble>(g_iHeight);

	/* tell OpenGL to use the whole window for drawing */
	glViewport(0, 0, (GLsizei)g_iWidth, (GLsizei)g_iHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(65.0, dAspectRatio, 0.1, 800.0);

	glutPostRedisplay();
}

void KeyboardEvent(unsigned char cKey, int iX, int iY)
{
	switch (cKey)
	{
		case 'x':
		{
			//exit the program
			ExitProgram();
		}
		break;

		case 'w':
		{
			// Move the camera along the N vector
			pCameraObject->MoveVecN(-0.5f);
		}
		break;

		case 's':
		{
			// Move the camera back along the N vector
			pCameraObject->MoveVecN(0.5f);
		}
		break;

		case 'a':
		{
			// Move the camera along the U vector
			pCameraObject->MoveVecU(-0.5f);
		}
		break;

		case 'd':
		{
			// Move the camera along the U vector
			pCameraObject->MoveVecU(0.5f);
		}
		break;

		case 'm':
		{
			// IF there is a movment sequence already in progress
			// ignore this key.
			if (!iBoardState)
			{
				// Set the start state.
				iBoardState = 1;
			}
		}
	}

	glutPostRedisplay();
}

void Display()
{

	float fAspectRatio = static_cast<GLfloat>(g_iWidth) / static_cast<GLfloat>(g_iHeight);

	float dEye_x = 0.0;
	float dEye_y = 0.0;
	float dEye_z = 0.0;
	float dLookat_x = 0.0;
	float dLookat_y = 0.0;
	float dLookat_z = 0.0;
	float dUp_x = 0.0;
	float dUp_y = 0.0;
	float dUp_z = 0.0;
	
	pCameraObject->GetCameraData(dEye_x,
								 dEye_y,
								 dEye_z,
								 dLookat_x,
								 dLookat_y,
								 dLookat_z,
								 dUp_x,
								 dUp_y,
								 dUp_z);
								 
	// clear the screen to white, which is the background color
	glClearColor(0.0, 0.0, 0.0, 0.0);

	// clear the buffer stored for drawing
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(dEye_x,
			  dEye_y,
			  dEye_z,
			  dLookat_x,
			  dLookat_y,
			  dLookat_z,
			  dUp_x,
			  dUp_y,
			  dUp_z);

	// Apply the background start field.
	DrawBackgroung();

	DrawReferenceBlocks();

	glPushMatrix();

	// Move the chess board so the center
	// is at the Y axis.
	glTranslatef(-17.5f, -10.0f, 17.5f);

	BuildChessBoardWhite();

	//Apply movment transforms based on the
	//current state.
	ChessMovement();
	BuildChessBoardBlack();

	glPopMatrix();

	// Draw the point cloud earth.
	worldGlobe3d->Draw();

	glutSwapBuffers();
}

// Read the mouse events to esablish what, if any,
// movment state the mouse position need to be in.
void MouseEventFunc(int button, int state, int x, int y)
{
	g_iMousex = x;
	g_iMousey = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			g_bDrag = true;
		}
		else
		{
			g_bDrag = false;
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			g_bOrbit = true;
		}
		else
		{
			g_bOrbit = false;
		}
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			g_bPan = true;
		}
		else
		{
			g_bPan = false;
		}
	}
}

// Drag functionality for mouse.
void MouseMotionFunc(int x, int y)
{
	// Get the delta of the mouse 
	// since the last time this function
	// or the button click was called.
	int iDeltaX = g_iMousex - x;
	int iDeltaY = g_iMousey - y;

	if (g_bDrag) // Camera Rotation
	{
		// If there is not change in the delta don't bother
		if (iDeltaX != 0)
		{
			// Apply movment in the x to the camera
			// in the V axis
			pCameraObject->RotateYaw(-RADIAN * iDeltaX);
		}

		if ( iDeltaY != 0 )
		{
			// Apply movment in the y to the camera
			// in the U axis
			pCameraObject->RotatePitch(RADIAN * iDeltaY);
		}

		glutPostRedisplay();
	}
	else if (g_bOrbit) // Orbit around lookat point
	{
		// Rotate the camera delta around the cameras
		// V axis
		pCameraObject->Orbit(-RADIAN * iDeltaX);

		glutPostRedisplay();
	}
	else if (g_bPan) // Pan functionality
	{
		// Move the camera delta in the V and U axis
		pCameraObject->MoveVecV(iDeltaY * 0.1f);
		pCameraObject->MoveVecU(-iDeltaX * 0.1f);

		glutPostRedisplay();
	}

	g_iMousex = x;
	g_iMousey = y;
}

// Zoom control associated with the wheel
// on the mouse
void MouseWheel(int button, int dir, int x, int y)
{
	pCameraObject->Zoom(dir);

	glutPostRedisplay();
}

void BuildChessBoardWhite()
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if ((j + i) % 2 == 0)
			{
				chessWhiteSquare->Draw(i * 5.0f, j * -5.0f);
			}
		}
	}
}

void BuildChessBoardBlack()
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if ((j + i) % 2 != 0)
			{
				chessBlackSquare->Draw(i * 5.0f, j * -5.0f);
			}
		}
	}
}

// Draw 4 wireframe boxes on the X Y Axis for
// reference.
void DrawReferenceBlocks()
{
	glPushMatrix();

	glTranslatef(0.0f, 0.0f, 55.0);

	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_TRIANGLES);
	glVertex3d(-1.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glEnd();
	glutWireCube(5.0);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(55.0f, 0.0f, 0.0);

	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glVertex3d(-1.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glEnd();
	glutWireCube(5.0);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(0.0f, 0.0f, -55.0);

	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(GL_TRIANGLES);
	glVertex3d(-1.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glEnd();
	glutWireCube(5.0);

	glPopMatrix();

	glPushMatrix();

	glTranslatef(-55.0f, 0.0f, 0.0);

	glColor3f(1.0f, 0.0f, 1.0f);

	glBegin(GL_TRIANGLES);
	glVertex3d(-1.0, 0.0, 0.0);
	glVertex3d(0.0, 1.0, 0.0);
	glVertex3d(1.0, 0.0, 0.0);
	glEnd();
	glutWireCube(5.0);

	glPopMatrix();
}

// Draw a sphere around the area and texture map it with 
// a star field.
void DrawBackgroung()
{
	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	
	GLUquadricObj *qObj = gluNewQuadric();
	gluQuadricNormals(qObj, GLU_SMOOTH);
	gluQuadricTexture(qObj, GL_TRUE);


	glBindTexture(GL_TEXTURE_2D, iStarmap);

	glColor3f(1.0f, 1.0f, 1.0f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	gluSphere(qObj, 500.0f, 90, 90);

	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void SetupBackground()
{
	unsigned char *pStarmap = nullptr;
	GLint iWidth = 0;
	GLint iHeight = 0;

	try
	{
		BMPReader bmpReader("starmap.bmp");
		bmpReader.CopyMem(&pStarmap);

		iWidth = bmpReader.GetWidth();
		iHeight = bmpReader.GetWidth();
	}
	catch (...)
	{
		::MessageBox(NULL, "The Texture was unable to be loaded.  \n Please verify 'starmap.bmp' is in the active directory.", "Error!", MB_OK);
		ExitProgram();
	}

	

	glGenTextures(1, &iStarmap);

	glBindTexture(GL_TEXTURE_2D, iStarmap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iWidth, iHeight, 0, GL_BGR, GL_UNSIGNED_BYTE, pStarmap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete pStarmap;
}

// All code beneath this point is dedicated to the movment
// sequence of the chess board.
typedef std::chrono::high_resolution_clock Time;
typedef std::chrono::milliseconds ms;
typedef std::chrono::duration<float> fsec;

std::chrono::high_resolution_clock::time_point m_start;

void ChessMovement()
{
	// Height the chestboard lifts to.
	const float MaxHeight = 1.25f;

	// Each stat represents a step in the chess board movment
	// sequence.
	// Event Control
	// 0 - Stoped
	// 1 - Start
	// 2 - Lift
	// 3 - Rotate
	// 4 - drop
	switch (iBoardState)
	{
		case 0:
		{
			return;
		}
		break;

		case 1:
		{
			m_start = Time::now();
			iBoardState = 2;
		}
		break;

		case 2:
		{
			const float fCycleTime = 1.5;

			std::chrono::high_resolution_clock::time_point currentTime = Time::now();
			std::chrono::duration<double> diff = currentTime - m_start;

			float dProgress = (static_cast<float>(diff.count()) / fCycleTime) * MaxHeight;

			if (diff.count() > fCycleTime)
			{
				iBoardState = 3;
				m_start = Time::now();
			}
			else
			{
				glTranslatef(0.0f, dProgress, 0.0f);
			}
		}
		break;

		case 3:
		{
			const float fCycleTime = 3.5;

			std::chrono::high_resolution_clock::time_point currentTime = Time::now();
			std::chrono::duration<double> diff = currentTime - m_start;

			float dProgress = (static_cast<float>(diff.count()) / fCycleTime) * 360;

			if (diff.count() > fCycleTime)
			{
				iBoardState = 4;
				m_start = Time::now();
			}
			else
			{
				glTranslatef(0.0f, MaxHeight, 0.0f);
				glTranslatef(17.5f, 0.0f, -17.5f);
				glRotatef(dProgress, 0.0f, 0.1f, 0.0f);
				glTranslatef(-17.5f, 0.0f, 17.5f);
			}
		}
		break;

		case 4:
		{
			const float fCycleTime = 1.5;

			std::chrono::high_resolution_clock::time_point currentTime = Time::now();
			std::chrono::duration<double> diff = currentTime - m_start;

			float dProgress = (static_cast<float>(diff.count()) / fCycleTime) * MaxHeight;

			if (diff.count() > fCycleTime)
			{
				iBoardState = 0;
				m_start = Time::now();
			}
			else
			{
				glTranslatef(0.0f, MaxHeight - dProgress, 0.0f);
			}
		}
		break;
	}
}