#ifndef _GUICONTROLMANAGER_
#define _GUICONTROLMANAGER_

namespace RTE {

	class Timer;
	class GUIWriter;

	/// <summary>
	/// A class used to manage the GUI as a whole and provide the interface between the GUI and the rest of the system.
	/// </summary>
	class GUIControlManager {
		friend class GUIControl;

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIControlManager object in system memory.
		/// </summary>
		GUIControlManager() = default;

		/// <summary>
		/// Creates the data for the control manager.
		/// </summary>
		/// <param name="guiScreen">Screen interface.</param>
		/// <param name="guiInput">Input interface.</param>
		/// <param name="skinDir">Skin directory.</param>
		/// <param name="skinFile"></param>
		/// <returns></returns>
		bool Create(GUIScreen *guiScreen, GUIInput *guiInput, const std::string &skinDir, const std::string &skinFile = "skin.ini");
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Destructor method used to clean up a GUIControlManager object in system memory.
		/// </summary>
		~GUIControlManager() { Destroy(); }

		/// <summary>
		/// Frees all the allocated resources.
		/// </summary>
		void Destroy();
#pragma endregion

#pragma region Layout Handling
		/// <summary>
		/// Loads the layout from a file.
		/// </summary>
		/// <param name="fileName">Filename.</param>
		/// <param name="keepOld">Whether to NOT clear out the manager, but just add the controls loaded to the existing layout.</param>
		/// <returns>True if successful.</returns>
		bool LoadLayout(const std::string &fileName, bool keepOld = false);

		/// <summary>
		/// Saves the layout to a file.
		/// </summary>
		/// <param name="fileName">Filename.</param>
		/// <returns>True if successful.</returns>
		bool SaveLayout(const std::string &fileName) const;
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the GUIScreen that this GUIControlManager is drawing itself to.
		/// </summary>
		/// <returns>Pointer to the GUIScreen that this GUIControlManager is drawing itself to.</returns>
		GUIScreen * GetScreen() const { return m_Screen; }

		/// <summary>
		/// Gets the Skin object currently in use.
		/// </summary>
		/// <returns>A pointer to the currently used skin. Please don't mess it up.</returns>
		GUISkin * GetSkin() const { return m_Skin.get(); }

		/// <summary>
		/// Gets the input controller object.
		/// </summary>
		/// <returns></returns>
		GUIInput * GetInputController() { return m_Input; }

		/// <summary>
		/// Sets the absolute position of this entire GUI on the screen. This is useful if the UI's are being drawn in a different area of the screen than the top left corner.
		/// This will adjust the mouse input to match the offset screen location.
		/// </summary>
		/// <param name="posX">The position.</param>
		/// <param name="posY"></param>
		void SetPosOnScreen(int posX, int posY) const { m_Input->SetMouseOffset(-posX, -posY); }

		/// <summary>
		/// Enables and disables the mouse completely for this.
		/// </summary>
		/// <param name="enable">Enable.</param>
		void EnableMouse(bool enable = true) { m_MouseEnabled = enable; }

		/// <summary>
		/// Sets the cursor type.
		/// </summary>
		/// <param name="cursorType">Cursor type.</param>
		void SetMouseCursorType(MouseCursorType cursorType) { m_CursorType = cursorType; }

		/// <summary>
		/// Gets a unique ID for a panel.
		/// </summary>
		/// <returns></returns>
		int RequestUniqueID() { return m_UniqueIDCounter++; }

		/// <summary>
		/// Give focus to a panel.
		/// </summary>
		/// <param name="newFocusedControl">Control.</param>
		void SetFocusedControl(GUIControl *newFocusedControl);
#pragma endregion

#pragma region 
		/// <summary>
		/// Gets a control.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <returns>GUIControlBaseclass, or 0 if not found.</returns>
		GUIControl * GetControl(const std::string_view &name) const;

		/// <summary>
		/// Checks if a control is under a specific point.
		/// </summary>
		/// <param name="pointX">The absolute point coordinates to check under.</param>
		/// <param name="pointY"></param>
		/// <param name="parent">Parent to check under. Pass null to default to the root control.</param>
		/// <param name="depth">How many levels of children under the parent to look at. If negative, goes as far as it can.</param>
		/// <returns>GUIControl. NULL if no control under the point.</returns>
		GUIControl * GetControlUnderPoint(int pointX, int pointY, GUIControl *parent = nullptr, int depth = -1);

		/// <summary>
		/// Gets the control list.
		/// </summary>
		/// <returns>Pointer.</returns>
		std::deque<GUIControl *> * GetControlList() { return &m_AllControls; }

		/// <summary>
		/// Manually creates a control.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <param name="type">Type.</param>
		/// <param name="parent">Parent.</param>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		/// <returns>GUIControlBaseclass created. 0 if not created.</returns>
		GUIControl * AddControl(const std::string &name, const std::string &type, GUIControl *parent, int posX, int posY, int width, int height);

		/// <summary>
		/// Manually creates a control.
		/// </summary>
		/// <param name="properties">Properties.</param>
		/// <returns>GUIControlBaseclass created. 0 if not created.</returns>
		GUIControl * AddControl(GUIProperties *properties);

		/// <summary>
		/// Removes a control by name.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <param name="removeFromParent">RemoveFromParent.</param>
		void RemoveControl(const std::string &name, bool removeFromParent);

		/// <summary>
		/// Clears all the controls.
		/// </summary>
		void ClearAllControls();
#pragma endregion

#pragma region
		/// <summary>
		/// Sets up capturing a mouse for a panel.
		/// </summary>
		/// <param name="panel">Panel.</param>
		void CaptureMouse(GUIControl *panel);

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
		void TrackMouseHover(GUIControl *panel, bool enabled, float delay);
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Gets an event from the queue.
		/// </summary>
		/// <param name="eventPtr">Pointer to variable receiving the Event.</param>
		/// <returns>Returns true when an event was grabbed. Returns false when there was no more events in the queue OR the Event pointer is 0.</returns>
		bool GetEvent(GUIEvent *eventPtr);

		/// <summary>
		/// Changes the skin of the controls.
		/// </summary>
		/// <param name="skinDir">Skin directory.</param>
		/// <param name="skinFile"></param>
		void ChangeSkin(const std::string &skinDir, const std::string &skinFile = "skin.ini") const;

		/// <summary>
		/// Changes the Z Position of a control.
		/// </summary>
		/// <param name="controlID"></param>
		/// <param name="type">Change type.</param>
		/// <returns></returns>
		bool ChangeZPosition(int controlID, ZPosChangeType changeType);

		/// <summary>
		/// Updates the GUI every frame
		/// </summary>
		/// <param name="ignoreKeyboardEvents">Whether keyboard events should be ignored or not. Used to avoid conflicts when custom keyboard handling for GUI elements is preset.</param>
		void Update(bool ignoreKeyboardEvents = false);

		/// <summary>
		/// Draws the mouse.
		/// </summary>
		void DrawMouse() const;

		/// <summary>
		/// Draws the GUI to the back buffer.
		/// </summary>
		void Draw() const { Draw(m_Screen); }

		/// <summary>
		/// Draws the GUI to the back buffer.
		/// </summary>
		/// <param name="targetScreen">The GUIScreen to draw to, overriding the one passed in on construction.</param>
		void Draw(GUIScreen *targetScreen) const;
#pragma endregion

	private:

		GUIScreen *m_Screen = nullptr; // Not owned.
		GUIInput *m_Input = nullptr; // Not owned.
		std::unique_ptr<GUISkin> m_Skin = nullptr;

		std::deque<GUIControl *> m_OrphanControls = {};
		std::deque<GUIControl *> m_AllControls = {};

		std::deque<GUIEvent> m_EventQueue = {};

		MouseCursorType m_CursorType = MouseCursorType::Pointer;

		Timer *m_Timer = nullptr;

		int m_UniqueIDCounter = 0;
		bool m_HoverTrack = false;
		float m_HoverTime = 0;
		GUIControl *m_HoveredControl = nullptr;
		GUIControl *m_CaptruredControl = nullptr;
		GUIControl *m_FocusedControl = nullptr;
		GUIControl *m_MousedOverControl = nullptr;

		bool m_MouseEnabled = true;

		int m_DoubleClickTime = 0;
		int m_DoubleClickSize = 0;
		int m_DoubleClickButtons = 0;
		std::array<float, 3> m_LastMouseDown = { -99999.0F ,-99999.0F ,-99999.0F }; // double click times
		GUIRect m_DoubleClickRect;

		/// <summary>
		/// Goes through the panel list and selects the topmost ('last', render wise) panel on a specific point.
		/// </summary>
		/// <param name="pointX">Mouse Position.</param>
		/// <param name="pointY"></param>
		/// <returns></returns>
		GUIControl * FindTopControl(int pointX, int pointY);

		/// <summary>
		/// Add a new event to the queue.
		/// </summary>
		/// <param name="event">Event point.</param>
		void AddEvent(const GUIEvent &newEvent) { m_EventQueue.emplace_back(newEvent); }

#pragma region Update Breakdown
		/// <summary>
		/// 
		/// </summary>
		void ProcessMouseInput(int keyModifier);

		/// <summary>
		/// 
		/// </summary>
		void ProcessKeyboardInput(int keyModifier);
#pragma endregion

		// Disallow the use of some implicit methods.
		GUIControlManager(const GUIControlManager &reference) = delete;
		GUIControlManager & operator=(const GUIControlManager &rhs) = delete;
	};
};
#endif