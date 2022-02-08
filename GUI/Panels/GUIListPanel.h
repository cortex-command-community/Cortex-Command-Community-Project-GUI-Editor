#ifndef _GUILISTPANEL_
#define _GUILISTPANEL_

#include "GUIScrollPanel.h"
#include "GUIInterface.h"


namespace RTE {

class Entity;

/// <summary>
/// A listbox panel class used for controls requiring a listbox.
/// </summary>
class GUIListPanel : public GUIControl {

public:

    // Item structure
	struct Item {
		int m_ID = 0;
		std::string m_Name;
		std::string m_RightText; // Extra text field displayed right-justified in the item
		int m_ExtraIndex = 0; // Extra index for special indexing or reference that the item is associated with. Menu-specific
		bool m_Selected = false;
		GUIBitmap *m_pBitmap = nullptr; // Can contain a bitmap to display in the list. This is OWNED
		const Entity *m_pEntity = nullptr; // Extra data associated with the item. This is NOT OWNED
		int m_Height = 0;;

		~Item() { delete m_pBitmap; m_pBitmap = nullptr; }

	};


#pragma region Creation
	/// <summary>
	/// Constructor method used to instantiate a GUIListPanel object in system memory.
	/// </summary>
	GUIListPanel() = default;

	/// <summary>
	/// Constructor method used to instantiate a GUIListPanel object in system memory.
	/// </summary>
	/// <param name="owningManager"></param>
	explicit GUIListPanel(GUIControlManager *owningManager) : m_HorzScroll(owningManager), m_VertScroll(owningManager) { m_OwningManager = owningManager; }
#pragma endregion

#pragma region Destruction
	/// <summary>
	/// 
	/// </summary>
	~GUIListPanel() override;
#pragma endregion


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Create
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Create the listpanel
// Arguments:       Position, Size.

    void Create(int X, int Y, int Width, int Height);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AddItem
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Add an item to the list.
// Arguments:       Name, extraText, Text which will be displayed right-justified in the item.
//                  a Bitmap object pointer alternatively pointing to a bitmap to display
//                  in the list. Ownership IS transferred!
//                  An Extra menu-specific index that this item may be associated with.
//                  Object instance associated with the item. Ownership is NOT TRANSFERRED!

    void AddItem(const std::string &Name, const std::string &rightText = "", GUIBitmap *pBitmap = nullptr, const Entity *pEntity = nullptr, const int extraIndex = -1);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ClearList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Clears the list.
// Arguments:       None.

    void ClearList();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ChangeSkin
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the skin has been changed.
// Arguments:       New skin pointer.

    void ChangeSkin(GUISkin *Skin);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          Draw
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Draws the panel
// Arguments:       Screen class

    void Draw(GUIScreen *Screen) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes down on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseDown(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseUp
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse goes up on the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseUp(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseMove
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse moves (over the panel, or when captured).
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseMove(int X, int Y, int Buttons, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseEnter
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse enters the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseEnter(int X, int Y, int Buttons, int Modifier) override { SendSignal(GUIEventCode::MouseEnter, Buttons); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnMouseLeave
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse leaves the panel
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnMouseLeave(int X, int Y, int Buttons, int Modifier) override { SendSignal(GUIEventCode::MouseLeave, Buttons); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnDoubleClick
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the mouse has double-clicked on the pane.
// Arguments:       Mouse Position, Mouse Buttons, Modifier.

	void OnDoubleClick(int X, int Y, int Buttons, int Modifier) override;


	/// <summary>
	/// Called when the mouse scroll wheel is moved.
	/// </summary>
	/// <param name="x">Mouse X position.</param>
	/// <param name="y">Mouse Y position.</param>
	/// <param name="modifier">Activated modifier buttons.</param>
	/// <param name="mouseWheelChange">The amount of wheel movement. Positive is scroll up, negative is scroll down.</param>
	void OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnKeyPress
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key is pressed (OnDown & repeating).
// Arguments:       KeyCode, Modifier.

	void OnKeyPress(int KeyCode, int Modifier) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          OnKeyDown
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when a key goes down.
// Arguments:       KeyCode, Modifier.

	void OnKeyDown(int KeyCode, int Modifier) override { SendSignal(GUIEventCode::KeyDown, KeyCode); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          PointInsideList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Checks if a point is inside the panel, but not on the scrollbars if
//                  they are visible
// Arguments:       X, Y Coordinates of point
// Return value:    A boolean of the check result

    bool PointInsideList(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnGainFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel gains focus.
// Arguments:       None.

	void OnGainFocus() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Virtual Method:  OnLoseFocus
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when the panel looses focus.
// Arguments:       None.

	void OnLoseFocus() override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ReceiveSignal
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Called when receiving a signal.
// Arguments:       Signal source, Signal code, Signal data.

    void ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BeginUpdate
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Locks the control from updating every time a new item is added.
// Arguments:       None.

	void BeginUpdate() { m_UpdateLocked = true; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          EndUpdate
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     UnLocks the control from updating every time a new item is added.
//                  Will automatically update the control.
// Arguments:       None.

    void EndUpdate();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetMultiSelect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the multi-selection value.
// Arguments:       MultiSelect

    void SetMultiSelect(bool MultiSelect);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetMultiSelect
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the multi-selection value.
// Arguments:       None.

	bool GetMultiSelect() const { return m_MultiSelect; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetHotTracking
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the hot tracking value.
// Arguments:       HotTrack.

    void SetHotTracking(bool HotTrack);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSelected
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the selected (or first in the selected list) item.
// Arguments:       None.

    Item * GetSelected();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetItemList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the item list.
// Arguments:       None.

	std::vector<Item *> * GetItemList() { return &m_Items; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetItem
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets an item at the index.
// Arguments:       Index.

    Item * GetItem(int Index);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetItem
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets an item at a specific absolute screen coordinate, if any.
// Arguments:       The absolute screen coordinates to get the item from.

    Item * GetItem(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetItemHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the drawing height of the item passed in.
// Arguments:       Pointer to the Item to get the height of. Ownership is NOT transferred!

    int GetItemHeight(Item *pItem);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetStackHeight
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the height, in pixels, of the stack of items up to a specific one.
//                  E.g. If the specified one is the first (top) in the list, 0 is returned.
//                  If the second one is specified, the height of the first is returned.
//                  If the third is specified, the sum of the first and second items' heights
//                  is returned. If 0 is passed, the entire stack's height is returned.
// Arguments:       Pointer to the Item to get the height up to. Ownership is NOT transferred!
//                  If 0 is passed, the entire stack's height is returned.

    int GetStackHeight(Item *pItem = nullptr);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetScrollVerticalValue
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the scroll value, in pixels, of the vertical axis.
// Arguments:       The scroll value in pixels.

    int GetScrollVerticalValue() const { return m_VertScroll.GetValue(); }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetItemValues
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the values of a specific Item.
// Arguments:       Index and Item reference.

    void SetItemValues(int Index, Item &item);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSelectedIndex
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the selected (or first in the selected list) index.
// Arguments:       None.
// Returns:         Index, or -1 if there is no items selected.

    int GetSelectedIndex();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetSelectedIndex
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Selects the item at the index.
// Arguments:       Index.

    void SetSelectedIndex(int Index);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetAlternateDrawMode
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Selects this to draw its items differently, with lines instead of
//                  rectangles, etc
// Arguments:       The new mode setting.

    void SetAlternateDrawMode(bool enableAltDrawMode = true) { m_AlternateDrawMode = enableAltDrawMode; }



//////////////////////////////////////////////////////////////////////////////////////////
// Method:          GetSelectionList
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Gets the selection list.
// Arguments:       None.

	std::vector<Item *> * GetSelectionList() { return &m_SelectedList; }


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          DeleteItem
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Deletes an item at the index.
// Arguments:       Index.

    void DeleteItem(int Index);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetSize
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the size of the panel.
// Arguments:       Width, Height.

    void SetSize(int Width, int Height) override;


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SetPositionAbs
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the position of the panel.
// Arguments:       X, Y.

    void SetPositionAbs(int X, int Y);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          EnableScrollbars
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Sets the enabled state of both scrollbars.
// Arguments:       Horz, Vert

    void EnableScrollbars(bool Horizontal, bool Vertical);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ScrollToItem
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the vertical scrollbar to show the specific item in the list.
// Arguments:       The item you want to show.

    void ScrollToItem(Item *pItem);


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ScrollToSelected
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the vertical scrollbar to show the first selected item in the
//                  list.
// Arguments:       None.

    void ScrollToSelected();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ScrollToTop
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the vertical scrollbar to show the top of the list
// Arguments:       None.

    void ScrollToTop();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          ScrollToBottom
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the vertical scrollbar to show the bottom of the list
// Arguments:       None.

    void ScrollToBottom();

	/// <summary>
	/// Scrolls the GUIListPanel up.
	/// </summary>
	void ScrollUp();

	/// <summary>
	/// Scrolls the GUIListPanel down.
	/// </summary>
	void ScrollDown();

	/// <summary>
	/// Scrolls the the GUIListPanel to a specific position
	/// </summary>
	/// <param name="position">The position to scroll to.</param>
	void ScrollTo(int position);

	/// <summary>
	/// Sets whether the scroll panel scrolls in a loop or not.
	/// </summary>
	/// <param name="scrollLoop">True to scroll in a loop, false to scroll with edge stopping.</param>
	void SetSelectionScrollingLoop(bool scrollLoop) { m_LoopSelectionScroll = scrollLoop; }


	/// <summary>
	/// Sets whether the list panel can be scrolled with the mouse scroll wheel.
	/// </summary>
	/// <param name="mouseScroll">True to enable scrolling, false to disable.</param>
	void SetMouseScrolling(bool mouseScroll) { m_MouseScroll = mouseScroll; }

	/// <summary>
	/// Gets the thickness (width on vertical, height on horizontal) of the ListPanel's scroll bars.
	/// </summary>
	/// <returns>The thickness (width on vertical, height on horizontal) of the ListPanel's scroll bars, in pixels.</returns>
	int GetScrollBarThickness() const { return m_ScrollBarThickness; }

	/// <summary>
	/// Sets the thickness (width on vertical, height on horizontal) of the ListPanel's scroll bars and adjusts them to the new thickness.
	/// </summary>
	/// <param name="newThickness">The new scroll bar thickness, in pixels.</param>
	void SetScrollBarThickness(int newThickness) { m_ScrollBarThickness = newThickness; AdjustScrollbars(); }

	/// <summary>
	/// Gets the padding around the ListPanel's scrollbars.
	/// </summary>
	/// <returns>Gets the padding around the ListPanel's scrollbars, in pixels.</returns>
	int GetScrollBarPadding() const { return m_ScrollBarPadding; }

	/// <summary>
	/// Sets the padding around the ListPanel's scrollbars and adjusts them to the new padding. Used to better size and position scrollbars within panel bounds, allowing to not overdraw on panel borders.
	/// </summary>
	/// <param name="newPadding">The new scrollbar padding, in pixels.</param>
	void SetScrollBarPadding(int newPadding) { m_ScrollBarPadding = newPadding; AdjustScrollbars(); }

protected:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Build the bitmap.
// Arguments:       UpdateBase, UpdateText.

	void BuildBitmap(bool UpdateBase, bool UpdateText);

private:

	GUIBitmap *m_BaseBitmap = nullptr;
	GUIBitmap *m_FrameBitmap = nullptr;

	bool m_UpdateLocked = false;

	GUIScrollPanel m_HorzScroll;
	GUIScrollPanel m_VertScroll;
	bool m_HorzScrollEnabled = true;
	bool m_VertScrollEnabled = true;
	int m_ScrollBarThickness = 17; //!< The thickness (width on vertical, height on horizontal) of the ListPanel's scroll bars, in pixels.
	int m_ScrollBarPadding = 0; //!< The padding around the scrollbars, in pixels. Used to better size and position scrollbars within panel bounds, allowing to not overdraw on panel borders.

	bool m_CapturedHorz = false;
	bool m_CapturedVert = false;
	bool m_ExternalCapture = false;

	int m_LargestWidth = 0;
	bool m_MultiSelect = false;
	bool m_HotTracking = false;
	int m_LastSelected = -1;
	bool m_LoopSelectionScroll = false;; //!< Whether the list panel scrolls in a loop or not, while scrolling the selection list (as opposed to the scrollbar).
	bool m_MouseScroll = false;; //!< Whether the list panel enables scrolling with the mouse scroll wheel.

	bool m_AlternateDrawMode = false; // This draws items differently, not with boxes etc.

	std::vector<Item *> m_Items;
	std::vector<Item *> m_SelectedList;
	int m_SelectedColorIndex = 0;

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          BuildDrawBitmap
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Build the drawing bitmap.
// Arguments:       None.

    void BuildDrawBitmap();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          AdjustScrollbars
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Adjusts the scrollbars.
// Arguments:       None.

    void AdjustScrollbars();


//////////////////////////////////////////////////////////////////////////////////////////
// Method:          SelectItem
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Selects an item based on mouse position.
// Arguments:       Mouse Position, Modifier.

    void SelectItem(int X, int Y, int Modifier);


	/// <summary>
	/// Perform list scrolling through the scrollbar.
	/// </summary>
	/// <param name="MouseWheelChange">Amount and direction of scrolling. Positive to scroll up, negative to scroll down.</param>
	void ScrollBarScrolling(int mouseWheelChange);


	/// <summary>
	/// Perform list scrolling by changing the currently selected list item.
	/// </summary>
	/// <param name="MouseWheelChange">Amount and direction of scrolling. Positive to scroll up, negative to scroll down.</param>
	void SelectionListScrolling(int mouseWheelChange);
};
};
#endif