#ifndef _GUICONTROL_
#define _GUICONTROL_

#include "GUIConstants.h"

namespace RTE {

	class GUIControlManager;
	class GUIWriter;

	/// <summary>
	/// A base class inherited by all controls.
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

#pragma region Global Macro Definitions
		#define GUIControlOverrideMethods \
			const std::string_view & GetControlType() const override { return c_ControlType; }
#pragma endregion

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIControl object in system memory.
		/// </summary>
		GUIControl() { Clear(); }

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		virtual void Create(const std::string &name, int posX, int posY, int width = -1, int height = -1);

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="reference">Properties.</param>
		virtual void Create(GUIProperties *reference);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// Called when the control has been destroyed.
		/// </summary>
		virtual void Destroy() {}
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
		bool Save(GUIWriter *writer);
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the enabled state of the control.
		/// </summary>
		/// <returns></returns>
		virtual bool GetEnabled();

		/// <summary>
		/// Sets the enabled state of the control.
		/// </summary>
		/// <param name="enabled">Enabled.</param>
		virtual void SetEnabled(bool enabled);

		/// <summary>
		/// Gets the visibility of the control.
		/// </summary>
		/// <returns></returns>
		virtual bool GetVisible();

		/// <summary>
		/// Sets the visibility of the control.
		/// </summary>
		/// <param name="visible">Visible.</param>
		virtual void SetVisible(bool visible);

		/// <summary>
		/// Returns the panel of the control.
		/// </summary>
		/// <returns>0 if the control does not have a panel, otherwise the topmost panel.</returns>
		virtual GUIPanel * GetPanel() { return m_Panel.get(); }

		/// <summary>
		/// Gets the parent of this control.
		/// </summary>
		/// <returns></returns>
		GUIControl * GetParent() { return m_ControlParent; }

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
		/// Returns the IsContainer value.
		/// </summary>
		/// <returns></returns>
		bool IsContainer() const { return m_IsContainer; }

		/// <summary>
		/// Gets the rectangle of the control.
		/// </summary>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		void GetControlRect(int *posX, int *posY, int *width, int *height) const;
#pragma endregion

#pragma region
		/// <summary>
		/// Gets the children list.
		/// </summary>
		/// <returns></returns>
		std::vector<GUIControl *> * GetChildren() { return &m_ControlChildren; }

		/// <summary>
		/// Adds a child to this control
		/// </summary>
		/// <param name="control">Control.</param>
		void AddChild(GUIControl *control);

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

#pragma region Concrete Methods
		/// <summary>
		/// Add a new event to the queue.
		/// </summary>
		/// <param name="eventType">Type.</param>
		/// <param name="eventCode">Message.</param>
		/// <param name="eventData">Data.</param>
		void AddEvent(GUIEventType eventType, GUIEventCode eventCode, int eventData = 0);
#pragma endregion

#pragma region Virtual Methods
		/// <summary>
		/// Called when the control is activated and ready for use.
		/// </summary>
		virtual void Activate() {}

		/// <summary>
		/// Called when the skin has been changed.
		/// </summary>
		/// <param name="newSkin">New skin pointer.</param>
		virtual void ChangeSkin(GUISkin *newSkin) { m_Skin = newSkin; }

		/// <summary>
		/// Called when the control needs to be moved.
		/// </summary>
		/// <param name="newPosX">New position.</param>
		/// <param name="newPosY"></param>
		virtual void Move(int newPosX, int newPosY) {}

		/// <summary>
		/// Called when the control needs to be resized.
		/// </summary>
		/// <param name="newWidth">New size.</param>
		/// <param name="newHeight"></param>
		virtual void Resize(int newWidth, int newHeight) {}
#pragma endregion

#pragma region Class Info
		/// <summary>
		///
		/// </summary>
		/// <returns></returns>
		virtual const std::string_view & GetControlType() const = 0;
#pragma endregion

	protected:

		std::unique_ptr<GUIPanel> m_Panel;

		GUISkin *m_Skin;
		int m_SkinPreset;
		GUIProperties m_Properties;
		GUIControl *m_ControlParent;
		std::vector<GUIControl *> m_ControlChildren;

		bool m_IsContainer;

		// For the GUI editor
		int m_MinWidth;
		int m_MinHeight;
		int m_DefWidth;
		int m_DefHeight;

		GUIControlManager *m_ControlManager;

	private:

		/// <summary>
		/// Clears all the member variables of this GUIControl, effectively resetting the members of this abstraction level only.
		/// </summary>
		void Clear();
	};
};
#endif