/* RIconLoader.h - Icon Loader
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides a Windows .ICO loader (based from MS ICO format)
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "RIconLoader.h"

RIconLoader::RIconLoader()
{
	memset(&m_Icon,0,sizeof(m_Icon));
}

bool
RIconLoader::Open(const char *filename)
{
	FILE *fp = fopen(filename,"rb");
	if (!fp) return false;
	fread(&m_Icon, 3, sizeof(WORD), fp);
	m_Icon.IconDir = new ICONENTRY[ m_Icon.IconCount ];
	m_Icon.IconData = new ICONDATA[ m_Icon.IconCount ];
	int i;
	fread(m_Icon.IconDir, m_Icon.IconCount, sizeof(ICONENTRY), fp);
	for (i=0;i<m_Icon.IconCount;i++)
	{
		ICONDATA *data = m_Icon.IconData + i;
		ICONENTRY *dir = m_Icon.IconDir + i;
		int NumColors = dir->NumColors;
		int DataSize = dir->DataSize;
		int sz;
		if (!NumColors) NumColors = 256;
		if (NumColors == 8) NumColors = 16;
		data->XorMap = new BYTE[dir->Width * dir->Height];
		data->AndMap = new BYTE[dir->Width * dir->Height];
		data->Palette = new RGB[NumColors];

		// Read header
		fseek(fp, dir->DataOffset, SEEK_SET);
		fread(data,40,sizeof(BYTE),fp); 
		DataSize-=40;
        
		// Read Palette
		sz = NumColors * sizeof(RGB);		
		fread(data->Palette, sz, sizeof(BYTE), fp);
		DataSize-=sz;
		
		// Read XorMap
		sz = dir->Width * dir->Height;
		
		if (NumColors == 16) sz>>=1;
		fread(data->XorMap,sz, sizeof(BYTE), fp);	
		DataSize-=sz;
	
		// Read AndMap
		sz = (dir->Width * dir->Height) >> 3;
		fread(data->AndMap,sz, sizeof(BYTE), fp);	
		DataSize-=sz;
	}						
	fclose(fp);
	return true;
}

RIconLoader::~RIconLoader()
{
	if (m_Icon.IconCount)
	{
		delete[] m_Icon.IconDir;
		delete[] m_Icon.IconData;
	}
}

RGB *RIconLoader::Expand8ToRGB(BYTE *src, const RGB *clut, int n)
{
	RGB *dest = new RGB[n], *_dest = dest;
	BYTE *_src = src;

	while (n)
	{
		const RGB *d = clut + *src ;
		*dest = *d;
		n--;
		src++;
		dest++;
	}
	delete [] _src;
	return _dest;
}

void RIconLoader::PreMask(RGB *map, BYTE *andmap, int n, const RGB &opacity)
{
	int i;
	for (i=0;i<n;i++)
	{
		BYTE a = andmap[i>>3]&(1<<(7-(i&7)));
		if (a) 
		{
			*map = opacity;
		}
		map++;
	}
}
void RIconLoader::InvertY(RGB *map, int width, int height)
{
	if (height<=1) return;
	int i;
	int w = width * 4;
	RGB *tmp = new RGB[w]; 
	if (!tmp) return;
	for (i=0;i<height/2;i++)
	{
		RGB *a = map + i * width;
		RGB *b = map + (height - 1 - i) * width;		
		memcpy(tmp, a, w);
		memcpy(a, b, w);
		memcpy(b, tmp, w);
	}
	delete[] tmp;
}
RGB *RIconLoader::Expand4ToRGB(BYTE *src, const RGB *clut, int n)
{
	RGB *dest = new RGB[n], *_dest = dest;
	assert(dest);
	BYTE *_src = src;
	n>>=1;
	while (n)
	{
		int lo = (*src) & 15;
		int hi = ((*src)>>4) & 15;		
		dest[0] = clut [ hi ];
		dest[1] = clut [ lo ]; 
		n--;
		src++;
		dest+=2;
	}
	delete [] _src;

	return _dest;
}


