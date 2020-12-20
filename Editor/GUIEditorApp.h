#ifndef _GUIEDITORAPP_
#define _GUIEDITORAPP_

#include "GUI.h"
#include "GUIPropertyPage.h"
#include "GUICollectionBox.h"
#include "GUIListBox.h"
#include "AllegroScreen.h"
#include "AllegroInput.h"

#include "Singleton.h"

#define g_GUIEditor GUIEditorApp::Instance()

using namespace RTE;

namespace RTEGUI {

	/// <summary>
	/// GUI Editor Application class that handles the main editor app.
	/// </summary>
	class GUIEditorApp : public Singleton<GUIEditorApp> {

	public:

		/// <summary>
		/// Constructor method used to instantiate a GUIEditorApp object in system memory.
		/// </summary>
		GUIEditorApp() { Clear(); }

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

		/// <summary>
		/// 
		/// </summary>
		void OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo);

		/// <summary>
		/// 
		/// </summary>
		void FlipFrameBuffers();

		/// <summary>
		/// 
		/// </summary>
		void ClearBackBuffer() const;

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
		void UpdateActiveBoxList() const;

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
		int HandleUnderMouse(GUIControl *Control, int MouseX, int MouseY) const;

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
		bool MouseInsideBox(int mouseX, int mouseY, int xPos, int yPos, int width, int height) const;

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
		void DrawSelectionHandle(int xPos, int yPos, int width, int height) const;

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
		std::string GenerateControlName(std::string strControlType) const;

		/// <summary>
		/// Calculates the nearest snap position (if snap is on).
		/// </summary>
		/// <param name="Position">Position.</param>
		/// <returns></returns>
		int ProcessSnapCoord(int position) const;

	private:

		/// <summary>
		/// GUI element selection structure.
		/// </summary>
		struct Selection {
			bool GrabbedControl;
			bool GrabbedHandle;
			bool TriggerGrab;

			GUIControl *Control;
			int HandleIndex;

			int GrabX;
			int GrabY;
			int	ClickX;
			int ClickY;
		};

		bool m_Quit;
		bool m_WindowResized;

		int m_ResX;
		int m_ResY;
		BITMAP *m_BackBuffer;
		std::unique_ptr<AllegroScreen> m_Screen;
		std::unique_ptr<AllegroInput> m_Input;
		std::unique_ptr<GUIControlManager> m_ControlManager;
		std::unique_ptr<GUIControlManager> m_EditorManager;
		std::unique_ptr<GUIPropertyPage> m_PropertyPage;
		std::unique_ptr<GUIListBox> m_ActiveBoxList;
		std::unique_ptr<GUIControl> m_RootControl;
		std::unique_ptr<GUICollectionBox> m_EditorBase;
		Selection m_SelectionInfo;
		std::string m_Filename;

		// Editor setup
		bool m_UnsavedChanges;
		bool m_SnapToGrid;
		int	m_GridSize;
		int m_RootOriginX;
		int m_RootOriginY;

		/// <summary>
		/// 
		/// </summary>
		void Clear();

		// Disallow the use of some implicit methods.
		GUIEditorApp(const GUIEditorApp &reference) = delete;
		GUIEditorApp &operator=(const GUIEditorApp &rhs) = delete;
	};
}
#endif