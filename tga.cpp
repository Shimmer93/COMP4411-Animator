#include "tga.h"

#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

typedef unsigned char TGA_BYTE;
typedef unsigned short TGA_SHORT;

#pragma pack(push)
// word-aligned structure, so that the following two
// structures will have the sizes we expect.
#pragma pack(1) 

typedef struct {
	TGA_BYTE bfIDLen;
	TGA_BYTE bfColorMapType;
	TGA_BYTE bfImageType;
	TGA_SHORT bfFirstEntryIndex;
	TGA_SHORT bfColorMapLen;
	TGA_BYTE bfColorMapEntrySize;
	TGA_SHORT bfXOrigin;
	TGA_SHORT bfYOrigin;
	TGA_SHORT bfWidth;
	TGA_SHORT bfHeight;
	TGA_BYTE bfPixelDepth;
	TGA_BYTE bfImageDesc;
} TGA_FILEHEADER;

#pragma pack(pop)

TGA_FILEHEADER tgafh;

unsigned char* readTGA(const char* szFileName, int& iWidth, int& iHeight)
{
	FILE* pfTGAFile;
	TGA_SHORT dwPos;
	unsigned char* pbyData = NULL;

	do {
		if ((pfTGAFile = fopen(szFileName, "rb")) == NULL)
			break;

		fread(&tgafh, sizeof(TGA_FILEHEADER), 1, pfTGAFile);

		cout << "id len: " << (int)tgafh.bfIDLen << endl;
		cout << "color map type: " << (int)tgafh.bfColorMapType << endl;
		cout << "img type: " << (int)tgafh.bfImageType << endl;
		cout << "first index: " << (int)tgafh.bfFirstEntryIndex << endl;
		cout << "color map len: " << (int)tgafh.bfColorMapLen << endl;
		cout << "color map entry size: " << (int)tgafh.bfColorMapEntrySize << endl;
		cout << "x & y origin: " << (int)tgafh.bfXOrigin << " " << (int)tgafh.bfYOrigin << endl;
		cout << "width & height: " << (int)tgafh.bfWidth << " " << (int)tgafh.bfHeight << endl;
		cout << "pixel depth: " << (int)tgafh.bfPixelDepth << endl;
		cout << "image desc: " << (int)tgafh.bfImageDesc << endl;

		if (tgafh.bfImageType != 2)
			break;
		if (tgafh.bfColorMapType == 1)
			break;
		if (tgafh.bfPixelDepth != 32)
			break;

		dwPos = sizeof(TGA_FILEHEADER) + tgafh.bfIDLen;
		fseek(pfTGAFile, dwPos, SEEK_SET);

		iWidth = tgafh.bfWidth;
		iHeight = tgafh.bfHeight;

		int iBytes = iWidth * iHeight * 4;
		pbyData = new unsigned char[iBytes];
		fread(pbyData, iBytes, 1, pfTGAFile);

	} while (false);

	if (pfTGAFile)
		fclose(pfTGAFile);

	return pbyData;
}