#include "ChessSquare.h"
#include "BMPReader.h"

#include <sstream>

ChessSquare::ChessSquare(char *cTextureName) : m_cData(nullptr)
{
	BMPReader *bmpReader = nullptr;

	try
	{
		bmpReader = new BMPReader(cTextureName);
	}
	catch (...)
	{
		std::stringstream errorMessage;

		errorMessage << "The Texture was unable to be loaded.  \n Please verify '" << cTextureName << "' is in the active directory.";

		::MessageBox(NULL, errorMessage.str().c_str(), "Error!", MB_OK);
		throw -1;
	}

	bmpReader->CopyMem(&m_cData);

	glGenTextures(1, &iTexture);

	glBindTexture(GL_TEXTURE_2D, iTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmpReader->GetWidth(), bmpReader->GetWidth(), 0, GL_BGR, GL_UNSIGNED_BYTE, m_cData);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	delete bmpReader;
}


ChessSquare::~ChessSquare()
{
	glDeleteTextures(1, &iTexture);
	delete m_cData;
}

void ChessSquare::Draw(GLfloat dX, GLfloat dZ)
{
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glBindTexture(GL_TEXTURE_2D, iTexture);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);
		//top
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dX + 2.5f, 0.0f, dZ + 2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dX - 2.5f, 0.0f, dZ + 2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dX - 2.5f, 0.0f, dZ - 2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dX + 2.5f, 0.0f, dZ - 2.5f);
		//bottom
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dX + 2.5f, -1.0f, dZ + 2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dX - 2.5f, -1.0f, dZ + 2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dX - 2.5f, -1.0f, dZ - 2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dX + 2.5f, -1.0f, dZ - 2.5f);
		//Right
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dX + 2.5f, 0.0f, dZ - 2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dX + 2.5f, -1.0f, dZ - 2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dX + 2.5f, -1.0f, dZ + 2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dX + 2.5f, 0.0f, dZ + 2.5f);
		//Left
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dX - 2.5f, 0.0f, dZ - 2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dX - 2.5f, -1.0f, dZ - 2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dX - 2.5f, -1.0f, dZ + 2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dX - 2.5f, 0.0f, dZ + 2.5f);
		//Back
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dX - 2.5f, 0.0f, dZ + 2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dX - 2.5f, -1.0f, dZ + 2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dX + 2.5f, -1.0f, dZ + 2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dX + 2.5f, 0.0f, dZ + 2.5f);
		//Front
		glTexCoord2f(0.0f, 0.0f); glVertex3f(dX - 2.5f, 0.0f, dZ - 2.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(dX - 2.5f, -1.0f, dZ - 2.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(dX + 2.5f, -1.0f, dZ - 2.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(dX + 2.5f, 0.0f, dZ - 2.5f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}