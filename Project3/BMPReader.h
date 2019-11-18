#pragma once

#include <vector>
#include <Windows.h>

class BMPReader
{
private:
	struct BMPHeader
	{
		__int16 header;
		__int32 fileSize;
		__int16 reserved1;
		__int16 reserved2;
		__int32 imageOffset;
	};

	struct BMPSize
	{
		__int32 iSizeX;
		__int32 iSizeY;
	};

	struct BMPPalletEntry
	{
		DWORD			dIndex;
		unsigned char	dR;
		unsigned char	dG;
		unsigned char	dB;
	};

public:
	BMPReader(char *fileName);
	~BMPReader();

	bool GetPixel(int x, int y, unsigned char &cR,
								unsigned char &cG,
								unsigned char &cB);
	bool CopyMem(unsigned char **data);

	int GetWidth(){ return m_sSize.iSizeX; }
	int GetHeight(){ return m_sSize.iSizeY; }

private:
	unsigned char *m_cData;

	std::vector<int>			m_vecPixels;
	std::vector<BMPPalletEntry> m_vecPallet;

	BMPHeader					m_sHeader;
	BMPSize						m_sSize;

	unsigned long				m_lDataSize;
};

