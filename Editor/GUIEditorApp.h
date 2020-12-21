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

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIEditorApp object in system memory.
		/// </summary>
		GUIEditorApp() = default;

		/// <summary>
		/// Initializes the editor app.
		/// </summary>
		/// <returns>False if initialization failed.</returns>
		bool Initialize();
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Updates the editor app.
		/// </summary>
		/// <returns>False if the editor has quit.</returns>
		bool Update();

		/// <summary>
		/// Draws the editor to the screen.
		/// </summary>
		void DrawEditor();
#pragma endregion

#pragma region File Panel Button Handling
		/// <summary>
		/// Called when the load button has been pushed.
		/// </summary>
		/// <param name="addControls">Whether to add controls as opposed to wiping out the current layout.</param>
		void OnLoadButton(bool addControls = false);

		/// <summary>
		/// Called when the save button has been pushed.
		/// </summary>
		/// <param name="saveAsNewFile">Whether to save to a new file or not.</param>
		void OnSaveButton(bool saveAsNewFile = false);

		/// <summary>
		/// Called when the quit button has been pushed.
		/// </summary>
		void OnQuitButton();
#pragma endregion

#pragma region Allegro Callback Handling
		/// <summary>
		/// Called when the window has been resized.
		/// </summary>
		void OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo);
#pragma endregion

	protected:

		/// <summary>
		/// 
		/// </summary>
		void CreateEditorElements();

		/// <summary>
		/// Process the editor logic.
		/// </summary>
		void ProcessMouseInput();

		void ProcessKeyboardInput();

		/// <summary>
		/// Updates the list of Active top level ControlBoxs found in the editor.
		/// </summary>
		void UpdateActiveBoxList();

		/// <summary>
		/// 
		/// </summary>
		void PopulateActiveBoxList() const;

		/// <summary>
		/// Checks if a control is under the mouse point.
		/// </summary>
		/// <param name="Parent"></param>
		/// <param name="MouseX"></param>
		/// <param name="MouseY"></param>
		/// <returns>GUIControl. NULL if no control under the mouse.</returns>
		GUIControl *ControlUnderMouse(GUIControl *parent, int mousePosX, int mousePosY);

		/// <summary>
		/// Checks if a control's handle is under the mouse point.
		/// </summary>
		/// <param name="Control"></param>
		/// <param name="MouseX"></param>
		/// <param name="MouseY"></param>
		/// <returns>Handle index. -1 if no handle under the mouse.</returns>
		int HandleUnderMouse(GUIControl *control, int mousePosX, int mousePosY) const;

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
		bool MouseInsideBox(int mousePosX, int mousePosY, int xPos, int yPos, int width, int height) const;

		/// <summary>
		/// Draws selection info around a control.
		/// </summary>
		/// <param name="Control">GUIControl.</param>
		void DrawSelectedControl(GUIControl *control);

		/// <summary>
		/// Draws a selection handle.
		/// </summary>
		/// <param name="X">Position.</param>
		/// <param name="Y"></param>
		/// <param name="Width">Size.</param>
		/// <param name="Height"></param>
		void DrawSelectionHandle(int xPos, int yPos, int width, int height) const;

		/// <summary>
		/// Calculates new position/size of a control given a handle movement.
		/// </summary>
		/// <param name="MouseX">Mouse.</param>
		/// <param name="MouseY"></param>
		/// <param name="X">Position.</param>
		/// <param name="Y"></param>
		/// <param name="Width">Size.</param>
		/// <param name="Height"></param>
		void CalculateHandleResize(int mousePosX, int mousePosY, int *xPos, int *yPos, int *width, int *height);

		/// <summary>
		/// Generates a new control name based on the type.
		/// </summary>
		/// <param name="strControlType">Control Type.</param>
		/// <returns></returns>
		std::string GenerateControlName(std::string controlType) const;

		/// <summary>
		/// Calculates the nearest snap position (if snap is on).
		/// </summary>
		/// <param name="Position">Position.</param>
		/// <returns></returns>
		int ProcessSnapCoord(int position) const;

		/// <summary>
		/// 
		/// </summary>
		void UpdatePropertyPage(GUIEvent &editorEvent);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="editorEvent"></param>
		void UpdateGridSize(GUIEvent &editorEvent);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="editorEvent"></param>
		void AddNewControl(GUIEvent &editorEvent);

	private:

		/// <summary>
		/// GUI element selection structure.
		/// </summary>
		struct Selection {
			bool GrabbedControl = false;
			bool GrabbedHandle = false;
			bool TriggerGrab = false;

			GUIControl *Control = nullptr;
			int HandleIndex = 0;

			int GrabX = 0;
			int GrabY = 0;
			int	ClickX = 0;
			int ClickY = 0;

			/// <summary>
			/// Clears selection info.
			/// </summary>
			void ClearSelection() {
				GrabbedControl = false;
				GrabbedHandle = false;
				TriggerGrab = false;
				Control = nullptr;
				HandleIndex = 0;
				GrabX = 0;
				GrabY = 0;
			}
		};

		bool m_Quit = false;
		bool m_WindowResized = false;

		int m_ResX = 1024;
		int m_ResY = 600;
		BITMAP *m_BackBuffer = nullptr;
		std::unique_ptr<AllegroScreen> m_Screen = nullptr;
		std::unique_ptr<AllegroInput> m_Input = nullptr;
		std::unique_ptr<GUIControlManager> m_ControlManager = nullptr;
		std::unique_ptr<GUIControlManager> m_EditorManager = nullptr;
		std::unique_ptr<GUIPropertyPage> m_PropertyPage = nullptr;
		std::unique_ptr<GUIListBox> m_ActiveBoxList = nullptr;
		std::unique_ptr<GUICollectionBox> m_EditorBase = nullptr;
		std::unique_ptr<GUICollectionBox> m_LeftColumn = nullptr;
		GUIControl *m_RootControl = nullptr;
		Selection m_SelectionInfo;
		std::string m_Filename = "";

		// Editor setup
		bool m_UnsavedChanges = false;
		bool m_SnapToGrid = true;
		bool m_Zoom = false;
		int	m_GridSize = 5;
		int m_WorkspacePosX = 335;
		int m_WorkspacePosY = 60;
		int m_WorkspaceWidth = 640;
		int m_WorkspaceHeight = 480;

		// Disallow the use of some implicit methods.
		GUIEditorApp(const GUIEditorApp &reference) = delete;
		GUIEditorApp &operator=(const GUIEditorApp &rhs) = delete;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Callback functions for handling events in Allegro.
	/// </summary>
	static void QuitHandler() { g_GUIEditor.OnQuitButton(); }
	static void ResizeHandler(RESIZE_DISPLAY_EVENT *resizeInfo) { g_GUIEditor.OnWindowResize(resizeInfo); }
}
#endif