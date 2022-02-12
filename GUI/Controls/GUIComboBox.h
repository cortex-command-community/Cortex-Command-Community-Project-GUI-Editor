#ifndef _GUICOMBOBOX_
#define _GUICOMBOBOX_

#include "GUIListPanel.h"

namespace RTE {

	class Entity;
	class GUILabel;
	class GUIListBox;

	/// <summary>
	/// A ComboBoxButton control class.
	/// </summary>
	class GUIComboBoxButton : public GUIControl {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIComboBoxButton object in system memory.
		/// </summary>
		/// <param name="owningManager">GUIManager.</param>
		explicit GUIComboBoxButton(GUIControlManager *owningManager) { m_OwningManager = owningManager; }

		/// <summary>
		/// Create the button.
		/// </summary>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		void Create(int posX, int posY, int width, int height);
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		~GUIComboBoxButton() override = default;
#pragma endregion

#pragma region Setters
		/// <summary>
		/// Sets the pushed state of the button.
		/// </summary>
		/// <param name="pushed">Pushed.</param>
		void SetPushed(bool pushed) { m_Pushed = pushed; }
#pragma endregion

#pragma region Event Handling
		/// <summary>
		/// Called when the mouse goes down on the panel.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseDown(int mousePosX, int mousePosY, int buttons, int modifier) override;

		/// <summary>
		/// Called when the mouse goes up on the button.
		/// </summary>
		/// <param name="mousePosX">Mouse Position.</param>
		/// <param name="mousePosY"></param>
		/// <param name="buttons">Mouse Buttons.</param>
		/// <param name="modifier">Modifier.</param>
		void OnMouseUp(int mousePosX, int mousePosY, int buttons, int modifier) override { m_Pushed = false; }
#pragma endregion

#pragma region Virtual Override Methods
		/// <summary>
		/// Called when the skin has been changed.
		/// </summary>
		/// <param name="newSkin">New skin pointer.</param>
		void ChangeSkin(GUISkin *newSkin) override;

		/// <summary>
		/// Draws the button.
		/// </summary>
		/// <param name="tagetScreen">Screen class.</param>
		void Draw(GUIScreen *tagetScreen) override;
#pragma endregion

	private:

		bool m_Pushed = false;
	};




	/// <summary>
	/// A ComboBox control class.
	/// </summary>
	class GUIComboBox : public GUIControl {

	public:

		/// <summary>
		/// Combo Style.
		/// </summary>
		enum DropDownStyles {
			DropDown,
			DropDownList,
		};

		GUIControlOverrideMethods;

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a GUIComboBox object in system memory.
		/// </summary>
		/// <param name="owningManager">GUIControlManager.</param>
		explicit GUIComboBox(GUIControlManager *owningManager) : m_Button(owningManager) { m_OwningManager = owningManager; }

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="name">Name.</param>
		/// <param name="posX">Position.</param>
		/// <param name="posY"></param>
		/// <param name="width">Size.</param>
		/// <param name="height"></param>
		void Create(const std::string_view &name, int posX, int posY, int width = -1, int height = -1) override;

		/// <summary>
		/// Called when the control has been created.
		/// </summary>
		/// <param name="reference">Properties.</param>
		void Create(GUIProperties *reference) override;
#pragma endregion

#pragma region Destruction
		/// <summary>
		/// 
		/// </summary>
		~GUIComboBox() override = default;
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Shows whether the list is currently dropped down or not.
		/// </summary>
		/// <returns>Whether this is currently dropped down and showing the list.</returns>
		bool IsDropped() const;

		/// <summary>
		/// Sets the drop down style of the combo box.
		/// </summary>
		/// <param name="newStyle">Style.</param>
		void SetDropDownStyle(DropDownStyles newStyle);

		/// <summary>
		/// Sets the drop height of the list.
		/// </summary>
		/// <param name="newDropHeight">Height.</param>
		void SetDropHeight(int newDropHeight);

		/// <summary>
		/// Returns the ListPanel component of the control.
		/// </summary>
		/// <returns>The ListPanel component of this ComboBox.</returns>
		GUIListBox * GetListPanel() { return m_ListPanel.get(); }
#pragma endregion

#pragma region Virtual Override Methods
		/// <summary>
		/// Called when the skin has been changed.
		/// </summary>
		/// <param name="newSkin">New skin pointer.</param>
		void ChangeSkin(GUISkin *newSkin) override;

		/// <summary>
		/// Draws the panel
		/// </summary>
		/// <param name="targetScreen">Screen class.</param>
		void Draw(GUIScreen *targetScreen) override;
#pragma endregion


		/// <summary>
		/// Called when receiving a signal.
		/// </summary>
		/// <param name="signalSource">Signal source.</param>
		/// <param name="eventCode">Signal code.</param>
		/// <param name="data">Signal data.</param>
		void ReceiveSignal(GUIControl *signalSource, GUIEventCode eventCode, int data) override;



		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          EndUpdate
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     UnLocks the control from updating every time a new item is added.
		//                  Will automatically update the control.
		// Arguments:       None.

			//void EndUpdate() { m_ListPanel->EndUpdate(); }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          Move
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Called when the control needs to be moved.
		// Arguments:       New position.

		void Move(int X, int Y) override;


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          Resize
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Called when the control needs to be resized.
		// Arguments:       New size.

		void Resize(int Width, int Height) override;


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          AddItem
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Add an item to the list.
		// Arguments:       Name, Extra text, bitmap to show in the list, extra entity data

		void AddItem(const std::string &Name, const std::string &ExtraText = "", GUIBitmap *pBitmap = nullptr, const Entity *pEntity = nullptr);


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          DeleteItem
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Delete an item from the list.
		// Arguments:       Item Index.

		void DeleteItem(int Index);


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          ClearList
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Clears the list.
		// Arguments:       None.

		void ClearList();


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetCount
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Get the item count.
		// Arguments:       None.

		int GetCount();


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetSelectedIndex
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Get the index of the selected item.
		// Arguments:       None.

		int GetSelectedIndex();


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetOldSelectionIndex
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Get the index of the previously selected item before the selection is
		//                  made.
		// Arguments:       None.

		int GetOldSelectionIndex() const { return m_OldSelection; }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          SetSelectedIndex
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Sets the index of the selected item.
		// Arguments:       None.

		void SetSelectedIndex(int Index);


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          RollbackSelection
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Rolls back the selection to the previous selected item.
		// Arguments:       None.
		// Returns:         Whether the rollback worked and was performed.

		bool RollbackSelection();


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetItem
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Returns the Item structure at the index.
		// Arguments:       Index.
		// Returns:         Pointer to the item structure. 0 if the index was invalid.

		GUIListPanel::Item * GetItem(int Index);


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetSelectedItem
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Returns the Item structure at the currently selected index.
		// Arguments:       Index.
		// Returns:         Pointer to the item structure. 0 if nothing valid is selected.

		GUIListPanel::Item * GetSelectedItem() { return GetItem(GetSelectedIndex()); }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          StoreProperties
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Gets the control to store the values into properties.
		// Arguments:       None.

		void StoreProperties() override;


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetDropDownStyle
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Gets the drop down style of the combo box.
		// Arguments:       None.

		int GetDropDownStyle() const { return m_DropDownStyle; }


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          GetText
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Gets text (only if style is DropDown).
		// Arguments:       None.
		// Returns:         Text. Returns empty string is style is not DropDown.

		std::string GetText();


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          SetText
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Sets text (only if style is DropDown).
		// Arguments:       Text.

		void SetText(const std::string &Text);


		//////////////////////////////////////////////////////////////////////////////////////////
		// Method:          ApplyProperties
		//////////////////////////////////////////////////////////////////////////////////////////
		// Description:     Applies new properties to the control.
		// Arguments:       GUIProperties.

		void ApplyProperties(GUIProperties *Props) override;

	private:

		static const std::string_view c_ControlType;

		static constexpr int m_MinWidth = 30;
		static constexpr int m_MinHeight = 10;
		static constexpr int m_DefaultWidth = 60;
		static constexpr int m_DefaultHeight = 20;

		int m_OldSelection = 0;
		bool m_CreatedList = false;

		int m_DropHeight = 80;
		int m_DropDownStyle = DropDownStyles::DropDownList;

		std::unique_ptr<GUILabel> m_TextPanel;
		std::unique_ptr<GUIListBox> m_ListPanel;
		GUIComboBoxButton m_Button;

		/// <summary>
		/// 
		/// </summary>
		void CreateChildren();
	};
};
#endif