#ifndef _RTEGUIEDITORAPP_
#define _RTEGUIEDITORAPP_

#include "allegro.h"

#include "GUI.h"
#include "GUIPropertyPage.h"
#include "GUICollectionBox.h"
#include "GUIListBox.h"
#include "AllegroScreen.h"
#include "AllegroInput.h"

#include "Singleton.h"

#define g_GUIEditor EditorApp::Instance()

using namespace RTE;

namespace RTEGUI {

	/// <summary>
	/// GUI Editor Application class that handles the main editor app.
	/// </summary>
	class EditorApp : public Singleton<EditorApp> {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a EditorApp object in system memory.
		/// </summary>
		EditorApp() = default;

		/// <summary>
		/// Initializes the editor app.
		/// </summary>
		/// <returns>False if initialization failed.</returns>
		bool Initialize();
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Frees any memory used by the editor's backbuffers, the rest will be handled by the default destructor.
		/// </summary>
		void DestroyBackBuffers();
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

	private:


		bool m_Quit = false;
		bool m_WindowResized = false;

		int m_ResX = 1280;
		int m_ResY = 600;
		BITMAP *m_BackBuffer = nullptr;
		BITMAP *m_ZoomBuffer = nullptr;
		std::unique_ptr<AllegroScreen> m_Screen = nullptr;
		std::unique_ptr<AllegroInput> m_Input = nullptr;
		std::unique_ptr<GUIControlManager> m_ControlManager = nullptr;
		std::unique_ptr<GUIControlManager> m_EditorManager = nullptr;
		std::unique_ptr<GUIPropertyPage> m_PropertyPage = nullptr;
		std::unique_ptr<GUIListBox> m_CollectionBoxList = nullptr;
		std::unique_ptr<GUIListBox> m_ControlsInActiveCollectionBoxList = nullptr;
		std::unique_ptr<GUICollectionBox> m_EditorBase = nullptr;
		std::unique_ptr<GUICollectionBox> m_LeftColumn = nullptr;
		std::unique_ptr<GUICollectionBox> m_RightColumn = nullptr;

		std::array<int, KEY_MAX> m_KeyStates;
		std::array<int, KEY_MAX> m_PrevKeyStates;

		GUIControl *m_RootControl = nullptr;
		Selection m_SelectionInfo;
		std::string m_Filename = "";

		// Editor setup
		bool m_UnsavedChanges = false;
		bool m_Zoom = false;
		int m_WorkspacePosX = 320;
		int m_WorkspacePosY = 60;
		int m_WorkspaceWidth = 640;
		int m_WorkspaceHeight = 480;

#pragma region Creation
		/// <summary>
		/// Create all the editor's GUI elements and makes it ready for use.
		/// </summary>
		void CreateEditorLayout();
#pragma endregion

		/// <summary>
		/// 
		/// </summary>
		void AddItemToCollectionBoxList(GUIControl *control, const std::string &indent) const;

		/// <summary>
		/// 
		/// </summary>
		void PopulateCollectionBoxList() const;

		/// <summary>
		/// 
		/// </summary>
		void PopulateCollectionBoxChildrenList(GUICollectionBox *collectionBox) const;

		/// <summary>
		/// 
		/// </summary>
		void SelectActiveControlInList() const;

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


#pragma region GUI Element Creation
		/// <summary>
		/// Create a new GUI element in the workspace.
		/// </summary>
		/// <param name="editorEvent">The editor event (button press) to create the element from.</param>
		void AddNewControl(GUIEvent &editorEvent);

		/// <summary>
		/// Generates a name for a new GUI element based on the element type.
		/// </summary>
		/// <param name="strControlType">Control Type.</param>
		/// <returns></returns>
		std::string GenerateControlName(std::string controlType) const;
#pragma endregion

#pragma region Updates
		/// <summary>
		/// Updates the list of active top and sub level CollectionBoxes found in the workspace.
		/// </summary>
		void UpdateCollectionBoxList();

		/// <summary>
		/// Updates the snap grid size.
		/// </summary>
		/// <param name="editorEvent">The editor event to get update info from.</param>
		void UpdateGridSize(GUIEvent &editorEvent);

		/// <summary>
		/// Updates the properties of an element in the workspace.
		/// </summary>
		/// <param name="control">The element to update properties for.</param>
		/// <param name="setUnsavedChanges">Whether the editor should ask to save changes before quitting.</param>
		void UpdateControlProperties(GUIControl *control, bool setUnsavedChanges = true);

		/// <summary>
		/// Updates the property page in the left column with all the properties of the currently selected element.
		/// </summary>
		/// <param name="editorEvent">The editor event to get update info from.</param>
		void UpdatePropertyPage(GUIEvent &editorEvent);
#pragma endregion

#pragma region Input Handling
		/// <summary>
		/// Process the mouse input of the editor.
		/// </summary>
		void ProcessMouseInput();

		/// <summary>
		/// Process the keyboard input of the editor.
		/// </summary>
		void ProcessKeyboardInput();
#pragma endregion

		// Disallow the use of some implicit methods.
		EditorApp(const EditorApp &reference) = delete;
		EditorApp &operator=(const EditorApp &rhs) = delete;
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region Allegro Callback Handling
	/// <summary>
	/// Window title bar quit button handling.
	/// </summary>
	static void QuitHandler() { g_GUIEditor.OnQuitButton(); }

	/// <summary>
	/// Window resize handling.
	/// </summary>
	/// <param name="resizeInfo"></param>
	static void ResizeHandler(RESIZE_DISPLAY_EVENT *resizeInfo) { g_GUIEditor.OnWindowResize(resizeInfo); }

	/// <summary>
	/// Window lose focus handling. Used to fix key buffer not clearing when the main window loses focus, making keys pressed before the focus loss "stuck" between updates.
	/// </summary>
	static void SwitchOutHandler() { remove_keyboard(); }

	/// <summary>
	/// Window gain focus handling. Used to fix key buffer not clearing when the main window loses focus, making keys pressed before the focus loss "stuck" between updates.
	/// </summary>
	static void SwitchInHandler() { install_keyboard(); }
#pragma endregion
}
#endif