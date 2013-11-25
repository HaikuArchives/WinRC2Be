/* RReportItem.cpp - Report View template
 *
 * w32rc2be - Beta 7 - Oct. 18, 2002
 * For conditions of distribution and use, see copyright notice in dxgl.h
 * Copyright (c) 1991-2002 realtech VR
 *
 * This file provides a 'Report view' lik interface (based from BListItem)
 */
#include <stdlib.h>
#include <View.h>
#include <String.h> 
#include "RReportItem.h"

const rgb_color kBlackColor = { 0,0,0,255};
const rgb_color kWhiteColor = { 255,255,255,255};
const rgb_color kHighlight = { 230, 255, 230, 255 };
RReportItem::RReportItem(BView *own, bool bTitle) 
		 : BListItem(), column(0), owner(own), isTitle(bTitle)
{ 
	
} 

RReportItem::~RReportItem()
{
	while (column)
	{
		RReportColumn *c = column->next;
		free((void*)column->text);
		delete column;
		column = c;
	}
}

void RReportItem::AddColumn(const char *text, int width, rgb_color color)
{
	RReportColumn *c = new RReportColumn;
	
	c->text = strdup(text);
	c->width = width;
	c->color = color;
	c->next = 0;
	
	if (width == -1)
	{
		BFont font;
		owner->GetFont(&font);
		c->width = (int)font.StringWidth(text) + 8;
	}

	if (column)
	{
		RReportColumn *p = column;
		while(p)
		{
			if (!p->next)
			{
				p->next = c;
				break;
			}
			p = p->next;
		}
	}
	else
		column = c;
}

void RReportItem::DrawItem(BView *owner, BRect frame, bool complete) 
{
	if (IsSelected() || complete) { 
	  rgb_color color; 
	  if (IsSelected()) { 
		 color = kHighlight; 
	  } 
	  else { 
		 color = owner->ViewColor(); 
	  } 
	  owner->SetHighColor(color); 
	  owner->FillRect(frame); 
   } 
   owner->MovePenTo(frame.left+4, frame.bottom-2); 
 
   if (IsEnabled()) { 
	  owner->SetHighColor(kBlackColor); 
   } 
   else { 
	  owner->SetHighColor(kWhiteColor); 
   } 

   RReportColumn *c = column;
   float x = frame.left;
   rgb_color viewColor = owner->ViewColor();
   rgb_color light = { 255,255,255,255};
   rgb_color dark = tint_color(viewColor, B_DARKEN_3_TINT);
   BFont font;
   owner->GetFont(&font);
	
   while(c)
   {		
   		if ((x>frame.left) && isTitle)
   		{		
   			owner->BeginLineArray(4);
   			owner->AddLine(BPoint(x,frame.bottom-1), BPoint(x,frame.top+1), dark);
   			owner->AddLine(BPoint(x+1,frame.bottom-1), BPoint(x+1,frame.top+1), dark);
   			owner->AddLine(BPoint(x+2,frame.bottom-1), BPoint(x+2,frame.top+1), light);
   			owner->AddLine(BPoint(x+3,frame.bottom-1), BPoint(x+3,frame.top+1), light);
   		
   			owner->EndLineArray();
   		}
   		owner->MovePenTo(x+8, frame.bottom-2); 
   		
   		if (font.StringWidth(c->text)>c->width)
   		{
   			char *t = strdup(c->text);
   			char *e = t;
   			while(*e)
   			{	
   				e++;
   			}
   			
   			while(font.StringWidth(t)>c->width)
   			{
   				*e =0;
   				e--;
   				if (e == t)
   					break;
   			}
   			*e = '.';
   			owner->DrawString(t);
   			free(t);
   		}
   		else
			owner->DrawString(c->text); 
		x+= c->width;
		c = c->next;
		
   }
} 
