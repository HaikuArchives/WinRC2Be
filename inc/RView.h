/* RView.h - BWindow/BMenu Creation
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides a 'LoadResource' win32-like interface
 */

#ifndef __RVIEW__HH
#define __RVIEW__HH

class RViewList;
class BWindow;
class BMenu;

extern RViewList *	LoadResourceView(unsigned id, void *param = 0);
extern BWindow	 *	LoadResource(unsigned id, void *pParam = 0);
extern BMenu	 *	LoadMenu(unsigned id);

#endif // __RVIEW__HH
