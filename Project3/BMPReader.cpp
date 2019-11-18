#include "BMPReader.h"
#include <iostream>
#include <fstream>

// The constructor opens and loads the image.
// if there is a failure, the function throws
// an error.
BMPReader::BMPReader(char *fileName) :m_cData(nullptr)
{
	std::fstream input;

	input.open(fileName, std::ios::in | std::ios::binary);

	if (!input.is_open())
	{
		throw(-1);
	}

	input.read((char*)&m_sHeader.header, 2);
	input.read((char*)&m_sHeader.fileSize, 4);
	input.read((char*)&m_sHeader.reserved1, 2);
	input.read((char*)&m_sHeader.reserved2, 2);
	input.read((char*)&m_sHeader.imageOffset, 4);

	m_lDataSize = m_sHeader.fileSize - m_sHeader.imageOffset;

	m_cData = new unsigned char[m_lDataSize];

	input.seekg(0x12);

	input.read(reinterpret_cast<char*>(&m_sSize.iSizeX), 4);
	input.read(reinterpret_cast<char*>(&m_sSize.iSizeY), 4);

	input.seekg(m_sHeader.imageOffset);

	input.read(reinterpret_cast<char*>(m_cData), m_lDataSize);

	input.seekg(m_sHeader.imageOffset);

	input.close();
}

// Take a uninitilized pointer from the caller
// and return a populated memory address.
// The caller is responsibile for releasing
// the memory.
bool BMPReader::CopyMem(unsigned char **data)
{
	if (data == nullptr)
	{ 
		return false;
	}

	*data = new unsigned char[m_lDataSize];

	memcpy(static_cast<void*>(*data),
		   static_cast<void*>(this->m_cData),
		   m_lDataSize);

	return true;
}

BMPReader::~BMPReader()
{
	if (m_cData)
		delete m_cData;
}

// Read a single pixel from the data and return the RGB values.
bool BMPReader::GetPixel(int x, int y, unsigned char &cR,
									   unsigned char &cG,
									   unsigned char &cB)
{
	size_t iOffset = (m_sSize.iSizeX * y + x) * 3;

	cR = m_cData[iOffset];
	cG = m_cData[iOffset + 1];
	cB = m_cData[iOffset + 2];

	return true;
}