#ifndef _RTEGUICONTROLBASE_
#define _RTEGUICONTROLBASE_

#include "GUIConstants.h"

namespace RTE {

	class GUIControlManager;
	class GUIWriter;

	/// <summary>
	/// 
	/// </summary>
	class GUIControl {

	public:

		/// <summary>
		/// Anchor points.
		/// </summary>
		enum Anchor {
			AnchorLeft = 0x01,
			AnchorTop = 0x02,
			AnchorRight = 0x04,
			AnchorBottom = 0x08
		};

		/// <summary>
		/// Mouse buttons.
		/// </summary>
		enum MouseButtons {
			MOUSE_NONE = 0x00,
			MOUSE_LEFT = 0x01,
			MOUSE_MIDDLE = 0x02,
			MOUSE_RIGHT = 0x04,
		};

#pragma region Global Macro Definitions
		#define GUIControlOverrideMethods \
			std::string_view GetControlType() const override { return c_ControlType; } \
			void GetControlRect(int *posX, int *posY, int *width, int *height) const override { GUIControl::GetRect(posX, posY, width, height); }
#pragma endregion

#pragma region Creation
		/// <summary>
		/// 
		/// </summary>
		GUIControl() = default;

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		virtual void Create(const std::string_view &name, int posX, int posY, int width = -1, int height = -1);

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="reference">Properties.</param>
		virtual void Create(GUIProperties *reference);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		virtual ~GUIControl() = default;
#pragma endregion

#pragma region Property Handling
		/// <summary>
		/// Gets the control properties.
		/// </summary>
		/// <returns></returns>
		GUIProperties * GetProperties() { return &m_Properties; }

		/// <summary>
		/// Gets the control to store the values into properties.
		/// </summary>
		virtual void StoreProperties() {}

		/// <summary>
		/// Applies new properties to the control.
		/// </summary>
		/// <param name="properties">GUIProperties.</param>
		virtual void ApplyProperties(GUIProperties *properties);

		/// <summary>
		/// Saves the control properties.
		/// </summary>
		/// <param name="writer">Writer.</param>
		/// <returns>True if successful.</returns>
		void Save(GUIWriter &writer);
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the parent of this control.
		/// </summary>
		/// <returns></returns>
		GUIControl * GetParent() { return m_ParentControl; }

		/// <summary>
		/// Gets the children list.
		/// </summary>
		/// <returns></returns>
		std::vector<GUIControl *> * GetChildren() { return &m_ChildControls; }

		/// <summary>
		/// Gets the panel's ID.
		/// </summary>
		/// <returns></returns>
		int GetUniqueID() const { return m_UniqueID; }

		/// <summary>
		/// Gets the enabled state of the panel.
		/// </summary>
		/// <returns></returns>
		bool GetEnabled() const { return m_Enabled; }

		/// <summary>
		/// Sets the enabled state of the panel.
		/// </summary>
		/// <param name="enabled">Enabled.</param>
		void SetEnabled(bool enabled) { m_Enabled = enabled; }

		/// <summary>
		/// Gets the visibility of the panel.
		/// </summary>
		/// <returns></returns>
		bool GetVisible() const { return m_Visible; }

		/// <summary>
		/// Sets the visibility of the panel.
		/// </summary>
		/// <param name="visible">Visible.</param>
		void SetVisible(bool visible) { m_Visible = visible; }

		/// <summary>
		/// Returns the IsContainer value.
		/// </summary>
		/// <returns></returns>
		bool IsContainer() const { return m_IsContainer; }

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
		/// Gets the control's name.
		/// </summary>
		/// <returns></returns>
		std::string GetName() const;

		/// <summary>
		/// Returns the anchor flags.
		/// </summary>
		/// <returns></returns>
		int GetAnchor();

		/// <summary>
		/// Gets the control's tooltip string.
		/// </summary>
		/// <returns></returns>
		std::string GetToolTip() const;

		/// <summary>
		/// Sets the control's tooltip string.
		/// </summary>
		/// <param name="toolTip">The new ToolTip for this.</param>
		void SetToolTip(const std::string_view &toolTip) const { m_Properties.SetPropertyValue("ToolTip", toolTip); }

		/// <summary>
		/// Sets the font this panel will be using
		/// </summary>
		/// <param name="newFont">The new font, ownership is NOT transferred!</param>
		virtual void SetFont(GUIFont *newFont) { m_Font = newFont; }
#pragma endregion

#pragma region Positioning and Sizing
		/// <summary>
		/// Gets the x position of the panel.
		/// </summary>
		/// <returns></returns>
		int GetPosX() const { return m_X; }

		/// <summary>
		/// Gets the x position of the panel, relative to its parent
		/// </summary>
		/// <returns></returns>
		int GetRelPosX() const { return m_X - m_ParentControl->GetPosX(); }

		/// <summary>
		/// Gets the y position of the panel
		/// </summary>
		/// <returns></returns>
		int GetPosY() const { return m_Y; }

		/// <summary>
		/// Gets the y position of the panel, relative to its parent.
		/// </summary>
		/// <returns></returns>
		int GetRelPosY() const { return m_Y - m_ParentControl->GetPosY(); }

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
		/// Called when the control needs to be resized.
		/// </summary>
		/// <param name="newWidth">New size.</param>
		/// <param name="newHeight"></param>
		virtual void Resize(int newWidth, int newHeight) {}

		/// <summary>
		/// Centers this in its parent, taking this' dimensions into consideration.
		/// </summary>
		/// <param name="centerX">Which axis to center.</param>
		/// <param name="centerY"></param>
		virtual void CenterInParent(bool centerX, bool centerY);

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
		/// Called when the control needs to be moved.
		/// </summary>
		/// <param name="newPosX">New position.</param>
		/// <param name="newPosY"></param>
		virtual void Move(int newPosX, int newPosY) {}

		/// <summary>
		/// Moves the position of the panel by a relative amount.
		/// </summary>
		/// <param name="newPosX">X.</param>
		/// <param name="newPosY">Y.</param>
		virtual void MoveRelative(int newPosX, int newPosY);

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

		/// <summary>
		/// Gets the rectangle of the control.
		/// </summary>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		virtual void GetControlRect(int *posX, int *posY, int *width, int *height) const;

		/// <summary>
		/// Changes the Z Position of the panel.
		/// </summary>
		/// <param name="type">Change type.</param>
		/// <returns></returns>
		bool ChangeZPosition(ZPosChangeType changeType);

		/// <summary>
		/// Checks if a point is inside the panel.
		/// </summary>
		/// <param name="posX">Coordinates of point.</param>
		/// <param name="posY"></param>
		/// <returns>A boolean of the check result.</returns>
		virtual bool PointInside(int posX, int posY);

		/// <summary>
		/// Recursively goes up the tree from to check the first panel under a point.
		/// </summary>
		/// <param name="posX">X, Y Coordinates of point.</param>
		/// <param name="posY"></param>
		/// <returns>A pointer to the panel. 0 if no panel is under the point.</returns>
		GUIControl * TopPanelUnderPoint(int posX, int posY);

		/// <summary>
		/// Recursively goes down the tree to check the last panel under a point
		/// </summary>
		/// <param name="posX">X, Y Coordinates of point.</param>
		/// <param name="posY"></param>
		/// <returns>A pointer to the panel. 0 if no panel is under the point.</returns>
		GUIControl * BottomPanelUnderPoint(int posX, int posY);
#pragma endregion

#pragma region 
		/// <summary>
		/// Adds a child to this panel.
		/// </summary>
		/// <param name="child">Pointer to the panel to add.</param>
		/// <param name="convertToAbsolutePos"></param>
		void AddChild(GUIControl *child, bool convertToAbsolutePos = true);

		/// <summary>
		/// Removes a child based on name.
		/// </summary>
		/// <param name="name">Child Name.</param>
		void RemoveChild(const std::string_view &name);

		/// <summary>
		/// Removes all the children.
		/// </summary>
		void RemoveAllChildren();
#pragma endregion

#pragma region Event Handling
		/// <summary>
		/// Sets the target panel to receive signals.
		/// </summary>
		/// <param name="target">Target panel.</param>
		void SetSignalTarget(GUIControl *target) { if (target) { m_SignalTarget = target; } }

		/// <summary>
		/// Add a new event to the queue.
		/// </summary>
		/// <param name="eventType">Type.</param>
		/// <param name="eventCode">Message.</param>
		/// <param name="eventData">Data.</param>
		void AddEvent(GUIEventType eventType, GUIEventCode eventCode, int eventData = 0);

		/// <summary>
		/// Called when receiving a signal.
		/// </summary>
		/// <param name="signalSource">Signal source.</param>
		/// <param name="eventCode">Signal code.</param>
		/// <param name="eventData">Signal data.</param>
		virtual void ReceiveSignal(GUIControl *signalSource, GUIEventCode eventCode, int eventData) {}

		/// <summary>
		/// Sends a signal to the target.
		/// </summary>
		/// <param name="eventCode">Signal code.</param>
		/// <param name="eventData">Data.</param>
		void SendSignal(GUIEventCode eventCode, int eventData) { if (m_SignalTarget) { m_SignalTarget->ReceiveSignal(this, eventCode, eventData); } }

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
		virtual void OnMouseWheelChange(int mousePosX, int mousePosY, int modifier, int mouseWheelChange) {}
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
		/// Called when the skin has been changed.
		/// </summary>
		/// <param name="newSkin">New skin pointer.</param>
		virtual void ChangeSkin(GUISkin *newSkin) { m_Skin = newSkin; }

		/// <summary>
		/// Draws the panel.
		/// </summary>
		/// <param name="targetScreen">Screen class.</param>
		virtual void Draw(GUIScreen *targetScreen);
#pragma endregion

#pragma region Class Info
		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		virtual std::string_view GetControlType() const { return ""; }
#pragma endregion

	protected:

		GUIControlManager *m_OwningManager = nullptr; //!< Not owned, owns this!
		GUISkin *m_Skin = nullptr;

		std::unique_ptr<GUIBitmap> m_DrawBitmap = nullptr;

		int m_UniqueID = -1;

		bool m_Enabled = true;
		bool m_Visible = true;

		int m_X = 0; // absolute coordinates
		int m_Y = 0;
		int m_Width = 0;
		int m_Height = 0;

		bool m_GotFocus = false;
		bool m_Captured = false;

		GUIControl *m_ParentControl = nullptr; //!< Not owned!

		GUIFont *m_Font = nullptr;
		int m_FontColor = 0;
		int m_FontSelectColor = 0;
		int m_FontShadow = 0;
		int m_FontKerning = 1;

		GUIProperties m_Properties;
		std::vector<GUIControl *> m_ChildControls;

		bool m_IsContainer = false;

#pragma region Property Handling
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
		std::string WriteValue(const std::string &name, bool value) const { return (name + " = " + (value ? "True" : "False") + "\n"); }
#pragma endregion

		/// <summary>
		/// Sets up the manager to enable/disable hover tracking of this panel.
		/// </summary>
		/// <param name="enabled">Enabled.</param>
		/// <param name="delay">Delay (milliseconds).</param>
		void TrackMouseHover(bool enabled, float delay);

	private:

		GUIRect m_Rect;

		GUIControl *m_SignalTarget = nullptr;
	};
}
#endif