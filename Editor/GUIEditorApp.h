#ifndef _GUIEDITORAPP_
#define _GUIEDITORAPP_

#include "allegro.h"
#include "winalleg.h"

#include "GUI.h"
#include "GUIPropertyPage.h"
#include "GUIListBox.h"

namespace RTE {

	/// <summary>
	/// GUI Editor Application class that handles the main editor app.
	/// </summary>
	class GUIEditorApp {

	public:

		// Selection structure
		typedef struct {
			bool m_GrabbedControl;
			bool m_GrabbedHandle;
			bool m_TriggerGrab;

			GUIControl *m_Control;
			int m_HandleIndex;

			int m_GrabX;
			int m_GrabY;
			int	m_ClickX;
			int m_ClickY;
		} Selection;

		/// <summary>
		/// Constructor method used to instantiate a GUIEditorApp object in system memory.
		/// </summary>
		GUIEditorApp() { Initialize(); }

		/// <summary>
		/// Destructor method used to clean up a GUIEditorApp object.
		/// </summary>
		~GUIEditorApp() {}

		/// <summary>
		/// Initializes the editor app.
		/// </summary>
		/// <returns>False if initialization failed.</returns>
		bool Initialize();

		/// <summary>
		/// Updates the editor app.
		/// </summary>
		/// <returns>False if the editor has quit.</returns>
		bool Update();

		/// <summary>
		/// Called when the quit button has been pushed.
		/// </summary>
		void OnQuitButton();

	protected:

		/// <summary>
		/// Called when the load button has been pushed.
		/// </summary>
		/// <param name="addControls">Whether to add controls as opposed to wiping out the current layout.</param>
		void OnLoadButton(bool addControls = false);

		/// <summary>
		/// Called when the save button has been pushed.
		/// </summary>
		void OnSaveButton();

		/// <summary>
		/// Called when the save as button has been pushed.
		/// </summary>
		void OnSaveAsButton();

		/// <summary>
		/// Process the editor logic.
		/// </summary>
		void ProcessEditor();

		/// <summary>
		/// Updates the list of Active top level ControlBoxs found in the editor.
		/// </summary>
		void UpdateActiveBoxList();

		/// <summary>
		/// Checks if a control is under the mouse point.
		/// </summary>
		/// <param name="Parent"></param>
		/// <param name="MouseX"></param>
		/// <param name="MouseY"></param>
		/// <returns>GUIControl. NULL if no control under the mouse.</returns>
		GUIControl *ControlUnderMouse(GUIControl *Parent, int MouseX, int MouseY);

		/// <summary>
		/// Checks if a control's handle is under the mouse point.
		/// </summary>
		/// <param name="Control"></param>
		/// <param name="MouseX"></param>
		/// <param name="MouseY"></param>
		/// <returns>Handle index. -1 if no handle under the mouse.</returns>
		int HandleUnderMouse(GUIControl *Control, int MouseX, int MouseY);

		/// <summary>
		/// Checks if the mouse point is inside a box.
		/// </summary>
		/// <param name="MouseX"></param>
		/// <param name="MouseY"></param>
		/// <param name="X"></param>
		/// <param name="Y"></param>
		/// <param name="Width"></param>
		/// <param name="Height"></param>
		/// <returns>True/False.</returns>
		bool MouseInsideBox(int MouseX, int MouseY, int X, int Y, int Width, int Height);

		/// <summary>
		/// Draws selection info around a control.
		/// </summary>
		/// <param name="Control">GUIControl.</param>
		void DrawSelectedControl(GUIControl *Control);

		/// <summary>
		/// Draws a selection handle.
		/// </summary>
		/// <param name="X">Position.</param>
		/// <param name="Y"></param>
		/// <param name="Width">Size.</param>
		/// <param name="Height"></param>
		void DrawSelectionHandle(int X, int Y, int Width, int Height);

		/// <summary>
		/// Clears selection info.
		/// </summary>
		void ClearSelection();

		/// <summary>
		/// Calculates new position/size of a control given a handle movement.
		/// </summary>
		/// <param name="MouseX">Mouse.</param>
		/// <param name="MouseY"></param>
		/// <param name="X">Position.</param>
		/// <param name="Y"></param>
		/// <param name="Width">Size.</param>
		/// <param name="Height"></param>
		void CalculateHandleResize(int MouseX, int MouseY, int *X, int *Y, int *Width, int *Height);

		/// <summary>
		/// Generates a new control name based on the type.
		/// </summary>
		/// <param name="strControlType">Control Type.</param>
		/// <returns></returns>
		std::string GenerateControlName(std::string strControlType);

		/// <summary>
		/// Calculates the nearest snap position (if snap is on).
		/// </summary>
		/// <param name="Position">Position.</param>
		/// <returns></returns>
		int ProcessSnapCoord(int Position);

	private:

		unsigned char m_BlackColor;
		BITMAP *m_pBackBuffer32;
		GUIControlManager *m_pControlManager;
		GUIControlManager *m_pEditorManager;
		GUIPropertyPage	*m_pPropertyPage;
		GUIListBox *m_pActiveBoxList;
		GUIControl *m_pRootControl;
		Selection m_SelectionInfo;
		std::string m_strFilename;

		// Editor setup
		bool m_bDirty;
		bool m_bSnapGrid;
		int	m_nGridSize;
	};
	extern GUIEditorApp g_GUIEditor;
}
#endif