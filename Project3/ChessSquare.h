#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"

class ChessSquare
{
private:
	GLuint iTexture;

	unsigned char *m_cData;

public:
	ChessSquare(char *cTextureName);
	~ChessSquare();

	void Draw(GLfloat dX, GLfloat dZ);
};

