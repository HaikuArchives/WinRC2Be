/* RReportItem.h - Report View template
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides a 'Report view' lik interface (based from BListItem)
 */

#ifndef _RREPORT_ITEM_HH
#define _RREPORT_ITEM_HH

#include <GraphicsDefs.h>
#include <ListItem.h> 

class BView;

struct RReportColumn
{
	const char				*	text;
	int 						width;
	rgb_color					color;
	struct RReportColumn	*	next;
};

class RReportItem : public BListItem 
{ 
   public: 
	  RReportItem(); 
	  ~RReportItem();
	  void AddColumn(const char *text, int width, rgb_color color);
	  virtual void DrawItem(BView *owner, BRect frame, bool complete = false); 

   private: 
	   struct RReportColumn	*column;

}; 

#endif // _REPORT_ITEM_HH