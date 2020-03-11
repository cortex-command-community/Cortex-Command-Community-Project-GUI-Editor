
//////////////////////////////////////////////////////////////////////////////////////////
// File:            main.h
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Header file for test application
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jackal@shplorb.com
//                  www.shplorb.com/~jackal

/*
//////////////////////////////////////////////////////////////////////////////////////////
// Method:			InitApp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Initialize the application
bool	InitApp(HINSTANCE hInst, int nCmdShow);



//////////////////////////////////////////////////////////////////////////////////////////
// Method:			WinProc
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Windows callback procedure
long PASCAL WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



//////////////////////////////////////////////////////////////////////////////////////////
// Method:			UpdateApp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Update the application every frame
void	UpdateApp(void);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:			ProcessEditor
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Main processing function for the editor

void	ProcessEditor(void);


void	EDDrawControl(void);

RTE::GUIControl *EDCheckMouse(RTE::GUIControl *Parent);

void	EDDrawHandle(int X, int Y);

void	EDAdjustControl(void);

void	EDCheckHandle(void);

void	EDInvokeUpdate(void);

std::string	EDGetNewControlName(std::string classname);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:			EDSave
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Save the layout

void	EDSave(void);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:			EDLoad
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Load a layout

void	EDLoad(void);



// Grab types
enum {
	Control = 0,
	HandleTopLeft,
	HandleTop,
	HandleTopRight,
	HandleRight,
	HandleBottomRight,
	HandleBottom,
	HandleBottomLeft,
	HandleLeft,
} Grab;


// Simple editor structure
typedef struct {
	RTE::GUIControl	*FocusedControl;

	bool		GrabbedControl;
	int			GrabX, GrabY;
	int			GrabType;

	int			NX, NY, NX2, NY2;

	bool		Snap;
	int			SnapGrid;

	RTE::GUISurface	*Check[2];

	bool		DirtyProperties;

	int			WorkspaceX, WorkspaceY;

} editor_t;
*/




/// 
/// 
/// //////////////////////////////////////////////////////////////////////////////////////////
// File:            Editor.cpp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Entry point file for the GUI Library editor
// Project:         GUI Library
// Author(s):       Jason Boettcher
//                  jboett@internode.on.net



//////////////////////////////////////////////////////////////////////////////////////////
// Inclusions of header files

/*#include <windows.h>
#include <windowsx.h>
#include <stdio.h>*/


#include "TimerMan.h"
#include "GUIEditorApp.h"
#include "GUIEditorLib.h"

#define RECT WINDOWS_RECT

#include "allegro.h"
#include "winalleg.h"
#undef RECT


extern "C" { FILE __iob_func[3] = { *stdin, *stdout, *stderr }; }

using namespace RTE;

namespace RTE {
	
	TimerMan	g_TimerMan;
};

GUIEditorApp g_GUIEditor;

extern HINSTANCE g_hInstance = 0;
extern HWND g_hWnd = 0;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:			main
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Entry point for the GUI Editor

int main(int argc, char *argv[])
{
	if(!g_GUIEditor.Initialize())
		return -1;
	
	// Get windows settings
	g_hWnd = win_get_window();
	g_hInstance = (HINSTANCE)GetWindowLong(g_hWnd, GWL_HINSTANCE);


	// Run editor loop
	while(1) {
		bool bContinue = g_GUIEditor.Update();
		if(!bContinue)
			break;
	}



	return 0;
}
END_OF_MAIN();