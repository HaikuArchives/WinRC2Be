/* RIconLoader.h - Icon Loader
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides a Windows .ICO loader (based from MS ICO format)
 */


#ifndef _RICONLOADER_HH
#define _RICONLOADER_HH

typedef unsigned char BYTE;
typedef short WORD;
typedef long DWORD;

typedef struct 
{
	BYTE r,g,b,a;
}RGB;

typedef struct _IconEntry 
{
    BYTE	Width;        /* Width of icon in pixels */
    BYTE	Height;       /* Height of icon in pixels */
    BYTE	NumColors;    /* Maximum number of colors */
    BYTE	Reserved;     /* Not used (always 0) */
    WORD	iUnused;      /* Not used (always 0) */
    WORD	iHotSpotX;    /* Not used (always 0) */
    DWORD	DataSize;     /* Length of icon bitmap in bytes */
    DWORD	DataOffset;   /* Offset position of icon bitmap in file */
} ICONENTRY;

typedef struct _IconData
{
    BYTE	 header[40];   /* Bitmap header data */
    RGB	    *Palette;      /* Color palette */
    BYTE    *XorMap;       /* Icon bitmap */
    BYTE    *AndMap;       /* Display bit mask */
} ICONDATA;

typedef struct _IconFile 
{
    WORD      Reserved;      /* Reserved (always 0) */
    WORD      ResourceType;  /* Resource ID (always 1) */
    WORD      IconCount;     /* Number of icon bitmaps in file */
    ICONENTRY *IconDir;     /* Directory of icon entries */
    ICONDATA  *IconData;    /* Listing of ICO bitmaps */
} ICONFILE;

class RIconLoader 
{
	public:
		RIconLoader();
		~RIconLoader();
		bool Open(const char *filename);
		RGB *Expand4ToRGB(BYTE *src, const RGB *clut, int n);
		RGB *Expand8ToRGB(BYTE *src, const RGB *clut, int n);
		void PreMask(RGB *map, BYTE *andmap, int n, const RGB &opacity);
		void InvertY(RGB *map, int width, int height);
		ICONFILE m_Icon;
};
#endif