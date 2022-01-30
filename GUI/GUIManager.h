#ifndef _GUIMANAGER_
#define _GUIMANAGER_

namespace RTE {

	class Timer;

	/// <summary>
	/// The main manager that handles all the panels and inputs.
	/// </summary>
	class GUIManager {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIManager object in system memory.
		/// </summary>
		/// <param name="input">Input interface.</param>
		explicit GUIManager(GUIInput *input) { Clear(); Create(input); }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="input">Input interface.</param>
		void Create(GUIInput *input);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to clean up a GUIManager object.
		/// </summary>
		~GUIManager() { Destroy(); }

		/// <summary>
		/// Frees all the allocated resources.
		/// </summary>
		void Destroy();

		/// <summary>
		/// 
		/// </summary>
		void Reset() { Clear(); }
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the input controller object.
		/// </summary>
		/// <returns></returns>
		GUIInput * GetInputController() { return m_Input; }

		/// <summary>
		/// Gets a unique ID for a panel.
		/// </summary>
		/// <returns></returns>
		int GetPanelID() { return m_UniqueIDCount++; }

		/// <summary>
		/// Give focus to a panel.
		/// </summary>
		/// <param name="panel">Panel.</param>
		void SetFocus(GUIPanel *panel);

		/// <summary>
		/// Enables and disables the mouse completely for this.
		/// </summary>
		/// <param name="enable">Enable.</param>
		void EnableMouse(bool enable = true) { m_MouseEnabled = enable; }
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Sets up capturing a mouse for a panel.
		/// </summary>
		/// <param name="panel">Panel.</param>
		void CaptureMouse(GUIPanel *panel);

		/// <summary>
		/// Releases a mouse capture.
		/// </summary>
		void ReleaseMouse();

		/// <summary>
		/// Sets up the manager to enable/disable hover tracking of this panel.
		/// </summary>
		/// <param name="panel">Panel.</param>
		/// <param name="enabled">Enabled.</param>
		/// <param name="delay">Delay (milliseconds).</param>
		void TrackMouseHover(GUIPanel *panel, bool enabled, int delay);

		/// <summary>
		/// Adds a panel to the list.
		/// </summary>
		/// <param name="panel">Pointer to a panel.</param>
		void AddPanel(GUIPanel *panel);

		/// <summary>
		/// Updates the GUI.
		/// </summary>
		/// <param name="ignoreKeyboardEvents">Whether keyboard events should be ignored or not. Used to avoid conflicts when custom keyboard handling for GUI elements is preset.</param>
		void Update(bool ignoreKeyboardEvents = false);

		/// <summary>
		/// Draw all the panels
		/// </summary>
		/// <param name="targetScreen">Screen.</param>
		void Draw(GUIScreen *targetScreen);
#pragma endregion

	private:

		std::vector<GUIPanel *> m_PanelList;
		GUIPanel *m_CapturedPanel;
		GUIPanel *m_FocusPanel;
		GUIPanel *m_MouseOverPanel;

		GUIInput *m_Input;
		bool m_MouseEnabled;
		int m_OldMouseX;
		int m_OldMouseY;

		int m_DoubleClickTime;
		int m_DoubleClickSize;
		int m_DoubleClickButtons;
		float m_LastMouseDown[3];
		GUIRect m_DoubleClickRect;

		bool m_HoverTrack;
		GUIPanel *m_HoverPanel;
		float m_HoverTime;

		int m_UniqueIDCount;

		Timer *m_Timer;

		/// <summary>
		/// Checks if the mouse point is inside a rectangle.
		/// </summary>
		/// <param name="rect">Rectangle</param>
		/// <param name="mousePosX">Mouse position.</param>
		/// <param name="mousePosY"></param>
		/// <returns></returns>
		bool MouseInRect(const GUIRect *rect, int mousePosX, int mousePosY) const;

		/// <summary>
		/// Goes through the panel list and selects the topmost ('last', render wise) panel on a specific point.
		/// </summary>
		/// <param name="pointX">Mouse Position.</param>
		/// <param name="pointY"></param>
		/// <returns></returns>
		GUIPanel * FindTopPanel(int pointX, int pointY);

		/// <summary>
		/// Goes through the panel list and selects the bottommost ('first', render wise) panel on a specific point.
		/// </summary>
		/// <param name="pointX">Mouse Position.</param>
		/// <param name="pointY"></param>
		/// <returns></returns>
		GUIPanel * FindBottomPanel(int pointX, int pointY);

		/// <summary>
		/// Clears all the member variables of this GUIManager.
		/// </summary>
		void Clear();

		// Disallow the use of some implicit methods.
		GUIManager(const GUIManager &reference) = delete;
		GUIManager & operator=(const GUIManager &rhs) = delete;
	};
};
#endif