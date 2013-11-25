/* RBitmapLoader.h - Bitmap Loader
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides new RBitmapView derivated from BView and Image loader
 */


#ifndef __RBITMAPLOADER_HH
#define __RBITMAPLOADER_HH

class BBitmap;

#include <View.h>

class BRect;

class RBitmapView : public BView 
{
	public:
		RBitmapView(BRect frame, const char *name, uint32 resizeMask, const char *resourceName);
		virtual ~RBitmapView();
		virtual void Draw(BRect );
	private:
		BBitmap *mMap8;
		BBitmap *mMap32;
};

class RBitmapLoader 
{
	public:
		RBitmapLoader();
		~RBitmapLoader();
		BBitmap *LoadBitmap(const char *name);
		BBitmap *LoadIcon(const char *name);
		BBitmap *TranslateBitmap(const char *name);
		BBitmap *TranslateIcon(int index, const char *name);
		BBitmap *ConvertBitmapTo8Bit(BBitmap *source);
		unsigned char *ConvertTo24bit(int sx, int sy, unsigned char *sourceBits, long sourceBpr);
	private:
		int current_id;
};

#endif // __RBITMAPLOADER_HH