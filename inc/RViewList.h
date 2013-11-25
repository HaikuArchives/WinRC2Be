/* RViewList.h - View list
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * Manage list of BView
 */


#ifndef __RVIEWLIST__HH
#define __RVIEWLIST__HH

class BView;
class RViewList
{
public:
	RViewList(BView *item, int _index, int _id = 0) : archive(item), next(0), index(_index), ID(_id) { };
	~RViewList() { };

	RViewList		*	GetNext() { return next; }
	BView			*	GetView() { return archive;}
	int					GetIndex() const { return index;}
	int					GetID()	const { return ID;}
	RViewList		*	Append(BView *view, int _index, int _id = 0) { next = new RViewList(view, _index, _id); return next;}
	RViewList		*	GetLast() { RViewList *pLast = 0, *p = this; while (p) { pLast = p; p = p->next; } return pLast; }
	
public:

	BView		*		archive;
	RViewList	*		next;
	int 				index;
	long 				ID;
};
#endif // __RVIEWLIST__HH
