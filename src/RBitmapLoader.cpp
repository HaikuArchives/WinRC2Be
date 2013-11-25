/* RIconLoader.h - Icon Loader
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides a Windows .ICO loader (based from MS ICO format)
 */

#include <Debug.h>
#include <string.h>
#include <stdio.h>

#include <Bitmap.h>
#include <Screen.h>
#include <Directory.h>
#include <Application.h>
#include <Entry.h>

#include <BitmapStream.h>
#include <TranslatorRoster.h>
#include <Roster.h>
#include <Path.h>
#include <Resources.h>

#include "RBitmapLoader.h"
#include "RIconLoader.h"

RBitmapView::RBitmapView(BRect frame, const char *name, uint32 resizeMask, const char *resourceName)
: BView(frame, name, resizeMask, B_WILL_DRAW)
{
	RBitmapLoader l; 
	if (strstr(resourceName,".ico"))
	{
		mMap32 = l.LoadIcon(resourceName);
		SetDrawingMode(B_OP_OVER);
	}
	else
	{
		mMap32 = l.LoadBitmap(resourceName);
	}
	if (mMap32)
	mMap8 = l.ConvertBitmapTo8Bit(mMap32);
	else mMap8 = 0;		
	SetViewColor(230,230,230); 
	return;
}

RBitmapView::~RBitmapView()
{
	if (mMap32) delete mMap32;
	if (mMap8) delete mMap8;
}

void RBitmapView::Draw(BRect )
{
    switch(BScreen().ColorSpace())
    {
        case B_COLOR_8_BIT:
        if (mMap8)
        {
            DrawBitmap(mMap8);
            break;
        }
        default:
        if (mMap32)
        {
            DrawBitmap(mMap32);
        }
    }
    return;
}

static int g_current_id = 0;
RBitmapLoader::RBitmapLoader()
{
	
}

RBitmapLoader::~RBitmapLoader() 
{

}

static BDirectory mAppDir;
static int initied = 0;
static void init()
{	 
	 app_info info;
     BEntry fichier,dummy;
     status_t result;
     be_app->GetAppInfo(&info);
     result = fichier.SetTo(&info.ref);
     if (!result) result = fichier.GetParent(&mAppDir);
     initied = 1;
}

BBitmap *RBitmapLoader::TranslateIcon(int index, const char *name)
{
	BPath path(&mAppDir, name);
	RIconLoader l;
	BBitmap *result = 0;
	if (l.Open(path.Path()))
	{
		ICONDATA *data = l.m_Icon.IconData + index;
		ICONENTRY *dir = l.m_Icon.IconDir + index;
		RGB *sourcebits = 0;
		RGB opacity;
		uint32 opaque = B_TRANSPARENT_MAGIC_RGBA32;
		opacity = *(RGB*)&opaque;
		switch(dir->NumColors) {
			case 0: // 256
			{
				sourcebits = l.Expand8ToRGB(data->XorMap, data->Palette, dir->Width * dir->Height);
			}
			break;
			case 16:
			{
				sourcebits = l.Expand4ToRGB(data->XorMap, data->Palette, dir->Width * dir->Height);
			}
			break;
		}
		
		l.PreMask(sourcebits, data->AndMap, dir->Width * dir->Height, opacity);
		l.InvertY(sourcebits, dir->Width, dir->Height);
 		result = new BBitmap(BRect(0,0, dir->Width-1, dir->Height-1), B_RGB32);
 		if (sourcebits)
 		{ 			
 			BYTE *rvb = ConvertTo24bit(dir->Width, dir->Height, (BYTE*)sourcebits, dir->Width * 4);
 			result->SetBits(rvb, dir->Width * dir->Height * 3, 0, B_RGB32);
 			delete rvb;
 		}	 
	}
	return result;
}
BBitmap *RBitmapLoader::LoadBitmap(const char *name)
{
	// Init App 
	if (!initied) init();
	// try to load image from ressource
	BBitmap *mImage32  = 0;
	app_info info;
	be_app->GetAppInfo(&info);
	BFile resFile(&info.ref, B_READ_WRITE);
	if (resFile.InitCheck() == B_NO_ERROR)
	{
		BResources res(&resFile);
		int32 idFound;
		size_t size;
		if (res.GetResourceInfo('IMAG', name, &idFound, &size))
		{
			// ressource exist, load it into a buffer
			char *image_data = new char[size];
             ASSERT(image_data);
			if (res.ReadResource('IMAG', idFound, image_data, 0, size) == B_NO_ERROR)
			{
				// map a BMemoryIO to point onto these data
				BMemoryIO mem(image_data, size);
				
				// read with translation kit from the data
				BTranslatorRoster *roster = BTranslatorRoster::Default(); 
				BBitmapStream stream; 
				if (roster->Translate(&mem, NULL, NULL, &stream,  B_TRANSLATOR_BITMAP) >= B_OK)
					stream.DetachBitmap(&mImage32); 
			}
			// delete buffer
			delete image_data;
		}
	}

	if (!mImage32)
	{
		printf("Load into resource %s\n",name);
		// if not in ressource, load from file and copy into ressources
		mImage32 = TranslateBitmap(name);
		if (!mImage32) 
		{
			return NULL;
		}
		// try to put into ressource
		BPath path(&mAppDir, name);
		BFile input(path.Path(), B_READ_ONLY);
		if (resFile.InitCheck() == B_NO_ERROR && input.InitCheck() == B_NO_ERROR)
		{
			BResources res(&resFile);
			input.Seek(0, SEEK_END);
			off_t insize = input.Position();
			input.Seek(0, SEEK_SET);
			char *data = new char[insize];
			if (data)
			{
				input.Read(data, insize);
				if (res.AddResource('IMAG',  g_current_id , data, insize, name)==B_OK)
				{
					g_current_id ++;
				}
				delete data;
			}
		}
	}
	return mImage32;
} 

static void GetIcon(BBitmap *mIcon, const char *s, int w, int h)
{
	
	int k;
	char *d = (char*)mIcon->Bits();
	for (k=0;k<h;k++, d+= mIcon->BytesPerRow(), s+= w * 4)
		memcpy(d, s, w * 4);		
}

static void SetIcon(const BBitmap *mIcon, char *s, int w, int h)
{
	
	int k;
	char *d = (char*)mIcon->Bits();
	for (k=0;k<h;k++, d+= mIcon->BytesPerRow(), s+= w * 4)
		memcpy(s, d, w * 4);		
}

BBitmap *RBitmapLoader::LoadIcon(const char *name)
{
	// Init App 
	ASSERT(name);
	if (!initied) init();

	// try to load image from ressource
	BBitmap *mIcon = 0;
	app_info info;
	be_app->GetAppInfo(&info);
	BFile resFile(&info.ref, B_READ_WRITE);
	if (resFile.InitCheck() == B_NO_ERROR)
	{
		BResources res(&resFile);
		int32 idFound;
		size_t size;
		if (res.GetResourceInfo('ICCN', name, &idFound, &size))
		{
			// ressource exist, load it into a buffer
			char *image_data = new char[size];
            ASSERT(image_data);
			if (res.ReadResource('ICCN', idFound, image_data, 0, size) == B_NO_ERROR)
			{
				float *p = (float*)image_data;
				mIcon = new BBitmap(BRect(p[0], p[1], p[2], p[3]), B_RGB32, false, true);
				GetIcon(mIcon, image_data + 4 * sizeof(float), p[2] - p[0], p[3] - p[1]);
			}
			// delete buffer
			delete image_data;
		}
	}

	if (!mIcon)
	{
		printf("Loading into resource %s\n",name);
		// if not in ressource, load from file and copy into ressources
		
		mIcon = TranslateIcon(0, name);		
		if (!mIcon) return 0;

		// try to put into ressource
		if (resFile.InitCheck() == B_NO_ERROR)
		{
			BResources res(&resFile);
			size_t image_size = 4 * sizeof(float) + mIcon->BitsLength();
			char *image_data = new char[image_size];
			float *p = (float*) image_data;
			BRect b = mIcon->Bounds();
			p[0] = b.left;
			p[1] = b.top; 
			p[2] = b.right;
			p[3] = b.bottom;
			SetIcon(mIcon, image_data + 4 * sizeof(float), p[2] - p[0], p[3] - p[1]);
			
			if (res.AddResource('ICCN',  g_current_id , image_data, image_size, name)==B_OK)
			{
				g_current_id++;
			}
			delete image_data;
			res.Sync();
		}
	}
	return mIcon;
} 

BBitmap * RBitmapLoader::TranslateBitmap(const char *name)
{
	 BPath path(&mAppDir, name);
	 BFile file(path.Path(), B_READ_ONLY); 
	 BTranslatorRoster *roster; 
	 BBitmapStream stream; 
	 BBitmap *result = NULL; 
	 roster = BTranslatorRoster::Default();
	 if (roster->Translate(&file, NULL, NULL, &stream,  B_TRANSLATOR_BITMAP) < B_OK) 
		 result = NULL; 
	 else stream.DetachBitmap(&result); 
	 return result; 
}

BBitmap *RBitmapLoader::ConvertBitmapTo8Bit(BBitmap *source)
{
    BRect r;
    BBitmap *dest;
    float sx,sy;
    if (!source) return NULL;
    r = source->Bounds();
    dest = new BBitmap(r, B_COLOR_8_BIT);
    if (!dest) return NULL;
    sx = r.Width()+1;
    sy = r.Height()+1;
    BYTE *sourceBits = (BYTE *)source->Bits();
    long sourceBpr = source->BytesPerRow();
    color_space sourceCs = source->ColorSpace();
    if (sourceCs == B_COLOR_8_BIT)
    {
        dest->SetBits(sourceBits, sourceBpr, 0, sourceCs);
        return dest;
    }
    else 
    if (sourceCs != B_RGB_32_BIT) 
    {
    	return NULL;
    }
  
    BYTE *rvb = ConvertTo24bit(sx, sy, sourceBits, sourceBpr);
    dest->SetBits(rvb, sx * sy * 3, 0, B_RGB_32_BIT);
    delete rvb;
    return dest;
}

unsigned char *RBitmapLoader::ConvertTo24bit(int sx, int sy, unsigned char *sourceBits, long sourceBpr)
{
   	BYTE *rvb = new BYTE[(int)sx * (int)sy * 3];
   	if (!rvb)
   	{
  		return NULL;
   	}
   	long i,j;
   	BYTE *p = rvb;
   	for(j = 0; j <sy; j++, sourceBits+=sourceBpr)
   	{
       	BYTE *s=sourceBits;
       	for(i=0; i<sx; i++, s+=4)
       	{
      		 // source is BBitmap 32, i.e. BGRA (gasp !) ----> :-(
      		 *(p++) = s[2];
      		 *(p++) = s[1];
       	     *(p++) = s[0];
       	}
   	}
	return rvb;
}

