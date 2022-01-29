#ifndef _GUIPANEL_
#define _GUIPANEL_

#include "GUIConstants.h"

namespace RTE {

	class GUIManager;

	/// <summary>
	/// A rectangle 'window' in the GUI that receives mouse and keyboard events.
	/// </summary>
	class GUIPanel {

	public:

		/// <summary>
		/// Mouse buttons.
		/// </summary>
		enum MouseButtons {
			MOUSE_NONE = 0x00,
			MOUSE_LEFT = 0x01,
			MOUSE_MIDDLE = 0x02,
			MOUSE_RIGHT = 0x04,
		};

		/// <summary>
		/// Mouse Modifiers
		/// </summary>
		enum MouseModifiers {
			MODI_NONE = 0x00,
			MODI_SHIFT = 0x01,
			MODI_CTRL = 0x02,
			MODI_ALT = 0x04,
			MODI_COMMAND = 0x08
		};

		/// <summary>
		/// Z Change
		/// </summary>
		enum ZChange { TopMost, BottomMost };

#pragma region Global Macro Definitions
#define GUIPanelOverrideMethods \
		GUIPanel * GetPanel() override { return this; }
#pragma endregion

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIPanel object in system memory.
		/// </summary>
		GUIPanel() { Clear(); }

		/// <summary>
		/// Constructor method used to instantiate a GUIPanel object in system memory.
		/// </summary>
		/// <param name="manager">Manager.</param>
		explicit GUIPanel(GUIManager *manager) { Clear(); Create(manager); }

		/// <summary>
		/// 
		/// </summary>
		/// <param name="manager">Manager.</param>
		void Create(GUIManager *manager);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		virtual ~GUIPanel() = default;
#pragma endregion

#pragma region Property Handling
		/// <summary>
		/// Loads the base data from a properties page
		/// </summary>
		/// <param name="properties">Pointer to the properties class</param>
		void LoadProperties(const GUIProperties *properties);

		/// <summary>
		/// Adds this panels properties to a properties class.
		/// </summary>
		/// <param name="properties">GUIProperties.</param>
		void BuildProperties(GUIProperties *properties) const;

		/// <summary>
		/// Convert the properties in the panel to a string.
		/// </summary>
		/// <returns></returns>
		std::string ToString() const;
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the panel's ID.
		/// </summary>
		/// <returns></returns>
		int GetPanelID() const { return m_ID; }

		/// <summary>
		/// Gets the enabled state of the panel.
		/// </summary>
		/// <returns></returns>
		bool _GetEnabled() const { return m_Enabled; }

		/// <summary>
		/// Sets the enabled state of the panel.
		/// </summary>
		/// <param name="enabled">Enabled.</param>
		void _SetEnabled(bool enabled) { m_Enabled = enabled; }

		/// <summary>
		/// Gets the visibility of the panel.
		/// </summary>
		/// <returns></returns>
		bool _GetVisible() const { return m_Visible; }

		/// <summary>
		/// Sets the visibility of the panel.
		/// </summary>
		/// <param name="visible">Visible.</param>
		void _SetVisible(bool visible) { m_Visible = visible; }

		/// <summary>
		/// Gets the panel's captured state.
		/// </summary>
		/// <returns></returns>
		bool IsCaptured() const { return m_Captured; }

		/// <summary>
		/// Sets the panel's captured state.
		/// </summary>
		/// <param name="captured">Captured.</param>
		void SetCaptureState(bool captured) { m_Captured = captured; }

		/// <summary>
		/// Gets the focus value of the panel.
		/// </summary>
		/// <returns></returns>
		bool HasFocus() const { return m_GotFocus; }

		/// <summary>
		/// Sets the focus of this panel.
		/// </summary>
		void SetFocus();

		/// <summary>
		/// Gets the x position of the panel.
		/// </summary>
		/// <returns></returns>
		int GetXPos() const { return m_X; }

		/// <summary>
		/// Gets the x position of the panel, relative to its parent
		/// </summary>
		/// <returns></returns>
		int GetRelXPos() const { return m_X - m_Parent->GetXPos(); }

		/// <summary>
		/// Gets the y position of the panel
		/// </summary>
		/// <returns></returns>
		int GetYPos() const { return m_Y; }

		/// <summary>
		/// Gets the y position of the panel, relative to its parent.
		/// </summary>
		/// <returns></returns>
		int GetRelYPos() const { return m_Y - m_Parent->GetYPos(); }

		/// <summary>
		/// Adjusts the absolute position of the panel.
		/// </summary>
		/// <param name="newPosX">X.</param>
		/// <param name="newPosY">Y.</param>
		/// <param name="moveChildren">Whether to move the children too.</param>
		virtual void SetPositionAbs(int newPosX, int newPosY, bool moveChildren = true);

		/// <summary>
		/// Sets the position of the panel, relative to its parent.
		/// </summary>
		/// <param name="newPosX">X.</param>
		/// <param name="newPosY">Y.</param>
		virtual void SetPositionRel(int newPosX, int newPosY);

		/// <summary>
		/// Moves the position of the panel by a relative amount.
		/// </summary>
		/// <param name="newPosX">X.</param>
		/// <param name="newPosY">Y.</param>
		virtual void MoveRelative(int newPosX, int newPosY);

		/// <summary>
		/// Centers this in its parent, taking this' dimensions into consideration.
		/// </summary>
		/// <param name="centerX">Which axis to center.</param>
		/// <param name="centerY"></param>
		virtual void CenterInParent(bool centerX, bool centerY);

		/// <summary>
		/// Gets the width of the panel.
		/// </summary>
		/// <returns></returns>
		int GetWidth() const { return m_Width; }

		/// <summary>
		/// Gets the height of the panel.
		/// </summary>
		/// <returns></returns>
		int GetHeight() const { return m_Height; }

		/// <summary>
		/// Adjusts the size of the panel.
		/// </summary>
		/// <param name="newWidth">Width.</param>
		/// <param name="newHeight">Height.</param>
		virtual void SetSize(int newWidth, int newHeight) { m_Width = newWidth; m_Height = newHeight; }

		/// <summary>
		/// Gets the Z index of the panel.
		/// </summary>
		/// <returns></returns>
		int GetZPos() const { return m_ZPos; }

		/// <summary>
		/// Sets the Z index of the panel.
		/// </summary>
		/// <param name="newZ">ZPos.</param>
		void SetZPos(int newZ) { m_ZPos = newZ; }

		/// <summary>
		/// Gets the parent of this panel.
		/// </summary>
		/// <returns></returns>
		GUIPanel * GetParentPanel() { return m_Parent; }

		/// <summary>
		/// Sets the font this panel will be using
		/// </summary>
		/// <param name="newFont">The new font, ownership is NOT transferred!</param>
		virtual void SetFont(GUIFont *newFont) { m_Font = newFont; }

		/// <summary>
		/// Gets the rectangle of the panel.
		/// </summary>
		/// <returns></returns>
		GUIRect * GetRect();

		/// <summary>
		/// Gets the rectangle of the panel.
		/// </summary>
		/// <param name="posX">X.</param>
		/// <param name="posY">Y.</param>
		/// <param name="width">Width.</param>
		/// <param name="height">Height.</param>
		void GetRect(int *posX, int *posY, int *width, int *height) const;
#pragma endregion

#pragma region Concrete Methods
		/// <summary>
		/// Sets up the panel for use with the manager.
		/// </summary>
		/// <param name="manager">Pointer to the manager to use.</param>
		/// <param name="zPos">ZPosition.</param>
		void Setup(GUIManager *manager, int zPos);

		/// <summary>
		/// Recursively goes up the tree from to check the first panel under a point.
		/// </summary>
		/// <param name="posX">X, Y Coordinates of point.</param>
		/// <param name="posY"></param>
		/// <returns>A pointer to the panel. 0 if no panel is under the point.</returns>
		GUIPanel * TopPanelUnderPoint(int posX, int posY);

		/// <summary>
		/// Recursively goes down the tree to check the last panel under a point
		/// </summary>
		/// <param name="posX">X, Y Coordinates of point.</param>
		/// <param name="posY"></param>
		/// <returns>A pointer to the panel. 0 if no panel is under the point.</returns>
		GUIPanel * BottomPanelUnderPoint(int posX, int posY);

		/// <summary>
		/// Changes the Z Position of the panel.
		/// </summary>
		/// <param name="type">Change type.</param>
		void ChangeZPosition(int type);
#pragma endregion

#pragma region 
		/// <summary>
		/// Adds a child to this panel.
		/// </summary>
		/// <param name="child">Pointer to the panel to add.</param>
		/// <param name="convertToAbsolutePos"></param>
		void AddChild(GUIPanel *child, bool convertToAbsolutePos = true);

		/// <summary>
		/// Removes a child based on name.
		/// </summary>
		/// <param name="child">Child Name.</param>
		void RemoveChild(const GUIPanel *child);
#pragma endregion

#pragma region Event Handling
		/// <summary>
		/// Sets the target panel to receive signals.
		/// </summary>
		/// <param name="target">Target panel.</param>
		void SetSignalTarget(GUIPanel *target) { if (target) { m_SignalTarget = target; } }

		/// <summary>
		/// Called when receiving a signal.
		/// </summary>
		/// <param name="signalSource">Signal source.</param>
		/// <param name="eventCode">Signal code.</param>
		/// <param name="eventData">Signal data.</param>
		virtual void ReceiveSignal(GUIPanel *signalSource, GUIEventCode eventCode, int eventData) {}

		/// <summary>
		/// Called when the panel gains focus.
		/// </summary>
		virtual void OnGainFocus() { m_GotFocus = true; }

		/// <summary>
		/// Called when the panel looses focus.
		/// </summary>
		virtual void OnLoseFocus() { m_GotFocus = false; }
#pragma endregion

#pragma region Mouse Event Handling
		/// <summary>
		/// Captures the mouse.
		/// </summary>
		void CaptureMouse();

		/// <summary>
		/// Releases the mouse.
		/// </summary>
		void ReleaseMouse();

		/// <summary>
		/// Called when the mouse goes down on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnMouseDown(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse goes up on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnMouseUp(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse has double-clicked on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnDoubleClick(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse moves (over the panel, or when captured).
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnMouseMove(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse enters the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnMouseEnter(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse leaves the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnMouseLeave(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse is hovering over the panel (has to be enabled).
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnMouseHover(int mousePosX, int mousePosY, int buttons, int modifier) {}

		/// <summary>
		/// Called when the mouse scroll wheel is moved.
		/// </summary>
		/// <param name="x">Mouse X position.</param>
		/// <param name="y">Mouse Y position.</param>
		/// <param name="modifier">Activated modifier buttons.</param>
		/// <param name="mouseWheelChange">The amount of wheel movement. Positive is scroll up, negative is scroll down.</param>
		virtual void OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) {}
#pragma endregion

#pragma region Keyboard Event Handling
		/// <summary>
		/// Called when a key goes down.
		/// </summary>
		/// <param name="keyCode">KeyCode.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnKeyDown(int keyCode, int modifier) {}

		/// <summary>
		/// Called when a key goes up.
		/// </summary>
		/// <param name="keyCode">KeyCode.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnKeyUp(int keyCode, int modifier) {}

		/// <summary>
		/// Called when a key is pressed (OnDown & repeating).
		/// </summary>
		/// <param name="keyCode">KeyCode.</param>
		/// <param name="modifier">Modifier.</param>
		virtual void OnKeyPress(int keyCode, int modifier) {}
#pragma endregion

#pragma region Virtual Methods
		/// <summary>
		/// Checks if a point is inside the panel.
		/// </summary>
		/// <param name="posX">Coordinates of point.</param>
		/// <param name="posY"></param>
		/// <returns>A boolean of the check result.</returns>
		virtual bool PointInside(int posX, int posY);

		/// <summary>
		/// Draws the panel.
		/// </summary>
		/// <param name="targetScreen">Screen class.</param>
		virtual void Draw(GUIScreen *targetScreen);
#pragma endregion

	protected:

		int m_X; // absolute coordinates
		int m_Y;
		int m_Width;
		int m_Height;

		bool m_Visible;
		bool m_Enabled;
		bool m_GotFocus;
		bool m_Captured;
		GUIManager *m_Manager;
		GUIPanel *m_Parent;

		GUIFont *m_Font;
		int m_FontColor;
		int m_FontShadow;
		int m_FontKerning;

#pragma region Property Handling
		/// <summary>
		/// Applies new properties to the panel.
		/// </summary>
		/// <param name="properties">GUIProperties.</param>
		void _ApplyProperties(const GUIProperties *properties);

		/// <summary>
		/// Writes a single value to string.
		/// </summary>
		/// <param name="name">Value name.</param>
		/// <param name="value">Value.</param>
		/// <returns></returns>
		std::string WriteValue(const std::string &name, int value) const { return (name + " = " + std::to_string(value) + "\n"); }

		/// <summary>
		/// Writes a single value to string.
		/// </summary>
		/// <param name="name">Value name.</param>
		/// <param name="value">Value.</param>
		/// <returns></returns>
		std::string WriteValue(const std::string &name, bool value) const {return (name + " = " + (value ? "True" : "False") + "\n"); }
#pragma endregion

#pragma region Event Handling
		/// <summary>
		/// Sends a signal to the target.
		/// </summary>
		/// <param name="eventCode">Signal code.</param>
		/// <param name="eventData">Data.</param>
		void SendSignal(GUIEventCode eventCode, int eventData) { if (m_SignalTarget) { m_SignalTarget->ReceiveSignal(this, eventCode, eventData); } }
#pragma endregion

		/// <summary>
		/// Sets up the manager to enable/disable hover tracking of this panel.
		/// </summary>
		/// <param name="enabled">Enabled.</param>
		/// <param name="delay">Delay (milliseconds).</param>
		void TrackMouseHover(bool enabled, int delay);

		/// <summary>
		/// Changes the Z position of a child panel.
		/// </summary>
		/// <param name="child">Child panel.</param>
		/// <param name="type">Change type.</param>
		void _ChangeZ(GUIPanel *child, int type);

	private:

		std::vector<GUIPanel *> m_Children;
		GUIRect m_Rect;

		int m_ID;
		int m_ZPos;

		GUIPanel *m_SignalTarget;

		/// <summary>
		/// Clears all the member variables of this GUIControl, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear();
	};
};
#endif