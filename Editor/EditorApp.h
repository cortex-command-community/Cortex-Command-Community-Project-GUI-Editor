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

		std::array<int, KEY_MAX> m_KeyStates;
		std::array<int, KEY_MAX> m_PrevKeyStates;

		GUIControl *m_RootControl = nullptr;
		Selection m_SelectionInfo;
		std::string m_Filename = "";

		// Editor setup
		bool m_UnsavedChanges = false;
		bool m_Zoom = false;

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