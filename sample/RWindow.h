// Generated by WinRC2Be Beta 8 Build Oct 20 2002 - http://www.realtech-vr.com
#ifndef __RWINDOW__HH
#define __RWINDOW__HH
#include <Window.h>
class BView;
class RViewList;
class BMessage;
template <unsigned ID_VALUE> class RWindow : public BWindow 
{
	public:
	RWindow(BRect rect, const char *pTitle, window_look look, window_feel feel, unsigned long flags, bool bIsCenter);
	virtual	bool	QuitRequested();
	virtual	void	MessageReceived(BMessage *msg);
	virtual	bool	OnInitDialog();
	virtual	BView	*	ChildID(long ID);
	void	Clean();
	RViewList	*	m_List;
};
#endif // __RWINDOW__HH
