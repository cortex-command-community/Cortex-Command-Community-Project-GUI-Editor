#include "GUI.h"
#include "GUIComboBox.h"

namespace RTE {

	const std::string_view GUIComboBox::c_ControlType = "COMBOBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Create(const std::string &Name, int X, int Y, int Width, int Height) {
		GUIControl::Create(Name, X, Y, Width, Height);

		// Setup the panel
		m_X = X;
		m_Y = Y;
		m_Width = m_DefaultWidth;
		m_Height = m_DefaultHeight;

		if (Width != -1) { m_Width = Width; }
		if (Height != -1) { m_Height = Height; }

		// Make sure the textbox isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		m_TextPanel.Create(0, 0, m_Width - 12, m_Height);
		m_TextPanel.SetVisible(true);
		m_TextPanel.SetLocked(m_DropDownStyle == DropDownStyles::DropDownList);
		m_TextPanel.SetSignalTarget(this);
		AddChild(&m_TextPanel);

		// Create the listpanel
		m_ListPanel.SetVisible(false);
		m_ListPanel.SetSignalTarget(this);
		m_ListPanel.SetMultiSelect(false);
		m_ListPanel.SetHotTracking(true);
		m_ListPanel.EnableScrollbars(false, true);

		// Create the button
		m_Button.Create(m_Width - 17, 0, 17, m_Height);
		m_Button.SetSignalTarget(this);
		AddChild(&m_Button);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the textbox isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		m_TextPanel.Create(0, 0, m_Width - 12, m_Height);
		m_TextPanel.SetVisible(true);
		m_TextPanel.SetSignalTarget(this);
		AddChild(&m_TextPanel);

		// Create the listpanel
		m_ListPanel.SetVisible(false);
		m_ListPanel.SetSignalTarget(this);
		m_ListPanel.SetMultiSelect(false);
		m_ListPanel.SetHotTracking(true);
		m_ListPanel.EnableScrollbars(false, true);
		m_ListPanel.SetMouseScrolling(true);


		// Create the button
		m_Button.Create(m_Width - 17, 0, 17, m_Height);
		m_Button.SetSignalTarget(this);
		AddChild(&m_Button);

		// Load the info
		Props->GetPropertyValue("Dropheight", &m_DropHeight);
		m_DropHeight = std::max(m_DropHeight, 20);

		std::string value;
		Props->GetPropertyValue("DropDownStyle", &value);
		if (value == "DropDownList") {
			m_DropDownStyle = DropDownStyles::DropDownList;
		} else if (value == "DropDown") {
			m_DropDownStyle = DropDownStyles::DropDown;
		}
		m_TextPanel.SetLocked(m_DropDownStyle == DropDownStyles::DropDownList);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIComboBox::~GUIComboBox() {
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Activate() {
		m_TextPanel.SetPositionAbs(m_X, m_Y);

		if (m_ParentControl) {
			//m_ParentControl->AddChild(&m_ListPanel);
			m_ListPanel.Create(m_X, m_Y + m_Height, m_Width, m_DropHeight);
			m_ListPanel.ChangeSkin(m_Skin);
			m_CreatedList = true;

			m_Button.Create(m_X + m_Width - 17, m_Y, 17, m_Height);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Free any old bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}

		// Create a new bitmap
		m_DrawBitmap = m_Skin->CreateBitmap(m_Width, m_Height);

		// Build the background
		m_Skin->BuildStandardRect(m_DrawBitmap, "TextBox", 0, 0, m_Width, m_Height);

		// Setup the skin in the panels too
		m_TextPanel.ChangeSkin(Skin);

		if (m_CreatedList) { m_ListPanel.ChangeSkin(Skin); }

		m_Button.ChangeSkin(Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Draw(GUIScreen *Screen) {
		// Draw the background
		m_DrawBitmap->Draw(Screen->GetBitmap(), m_X, m_Y, nullptr);

		// If selected item has a bitmap AND no text to show, just show the bitmap as the selected thing
		if (m_ListPanel.GetSelected() && m_ListPanel.GetSelected()->m_Name.empty() && m_ListPanel.GetSelected()->m_pBitmap) { m_ListPanel.GetSelected()->m_pBitmap->DrawTrans(Screen->GetBitmap(), m_X + 4, m_Y + 4, nullptr); }

		GUIControl::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ReceiveSignal(GUIControl * Source, GUIEventCode Code, int Data) {
		GUIAssert(Source, "");

		int sourcePanelID = Source->GetUniqueID();

		// ComboBoxButton
		if (sourcePanelID == m_Button.GetUniqueID()) {
			// Clicked and list panel is not visible. open the list panel.
			if (Code == GUIEventCode::Clicked && !m_ListPanel.GetVisible()) {
				m_ListPanel.SetVisible(true);
				m_ListPanel.SetFocus();
				m_ListPanel.CaptureMouse();

				// Force a redraw
				m_ListPanel.EndUpdate();

				// Make this panel go above the rest
				m_ListPanel.ChangeZPosition(GUIControl::ZChange::TopMost);

				// Save the current selection
				if (m_ListPanel.GetSelectedIndex() >= 0 && m_ListPanel.GetSelectedIndex() < m_ListPanel.GetItemList()->size()) { m_OldSelection = m_ListPanel.GetSelectedIndex(); }

				AddEvent(GUIEventType::Notification, GUIEventCode::Dropped);
			}
		} else if (sourcePanelID == m_TextPanel.GetUniqueID()) {
			// Textbox
			// MouseDown
			if (Code == GUIEventCode::MouseDown && m_DropDownStyle == DropDownStyles::DropDownList && Data & GUIControl::MouseButtons::MOUSE_LEFT) {
				// Drop
				m_ListPanel.SetVisible(true);
				m_ListPanel.SetFocus();
				m_ListPanel.CaptureMouse();

				// Force a redraw
				m_ListPanel.EndUpdate();

				// Make this panel go above the rest
				m_ListPanel.ChangeZPosition(GUIControl::ZChange::TopMost);

				// Save the current selection
				if (m_ListPanel.GetSelectedIndex() >= 0 && m_ListPanel.GetSelectedIndex() < m_ListPanel.GetItemList()->size()) { m_OldSelection = m_ListPanel.GetSelectedIndex(); }

				AddEvent(GUIEventType::Notification, GUIEventCode::Dropped);
			}

		} else if (sourcePanelID == m_ListPanel.GetUniqueID()) {
			// ListPanel
			// MouseMove
			if (Code == GUIEventCode::MouseMove) {
				m_Button.SetPushed(false);
				return;
			}

			int mouseX = 0;
			int mouseY = 0;
			m_OwningManager->GetInputController()->GetMousePosition(&mouseX, &mouseY);
			// Mouse down anywhere outside the list panel.
			if (Code == GUIEventCode::Click) {
				// Hide the list panel
				m_ListPanel.SetVisible(false);
				m_ListPanel.ReleaseMouse();
				m_OwningManager->SetFocus(nullptr);
				m_Button.SetPushed(false);

				// Restore the old selection
				m_ListPanel.SetSelectedIndex(m_OldSelection);

				AddEvent(GUIEventType::Notification, GUIEventCode::Closed);
			} else if (Code == GUIEventCode::MouseUp && m_ListPanel.PointInsideList(mouseX, mouseY)) {
				// Select on mouse up instead of down so we don't accidentally click stuff behind the disappearing listbox immediately after. Also only work if inside the actual list, and not its scrollbars.
				// Hide the list panel
				m_ListPanel.SetVisible(false);
				m_ListPanel.ReleaseMouse();
				m_OwningManager->SetFocus(nullptr);
				m_Button.SetPushed(false);

				AddEvent(GUIEventType::Notification, GUIEventCode::Closed);

				// Set the text to the item in the list panel
				GUIListPanel::Item* Item = m_ListPanel.GetSelected();
				if (Item) {
					m_TextPanel.SetText(Item->m_Name);
				} else {
					// Restore the old selection
					m_ListPanel.SetSelectedIndex(m_OldSelection);
				}
			}

			if (m_DropDownStyle == DropDownList) {
				// Set the text to the item in the list panel
				if (const GUIListPanel::Item* Item = m_ListPanel.GetSelected()) { m_TextPanel.SetText(Item->m_Name); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ClearList() {
		m_TextPanel.SetText("");
		m_ListPanel.ClearList();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Move(int X, int Y) {
		GUIControl::SetPositionAbs(X, Y);

		m_ListPanel.SetPositionAbs(m_X, m_Y + m_Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Resize(int Width, int Height) {
		// Make sure the textbox isn't too small
		Width = std::max(Width, m_MinWidth);
		Height = std::max(Height, m_MinHeight);
		Height = std::min(Height, 20);

		GUIControl::SetSize(Width, Height);

		m_TextPanel.SetSize(m_Width - 12, m_Height);
		m_TextPanel.SetPositionAbs(m_X, m_Y);

		m_Button.SetPositionAbs(m_X + m_Width - 13, m_Y + 1);
		m_Button.SetSize(12, m_Height - 2);
		m_ListPanel.SetSize(m_Width, m_DropHeight);
		m_ListPanel.SetPositionAbs(m_X, m_Y + m_Height);

		// Force a bitmap rebuild
		ChangeSkin(m_Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::DeleteItem(int Index) {
		m_ListPanel.DeleteItem(Index);

		// Update the selection
		const GUIListPanel::Item *Item = m_ListPanel.GetSelected();
		if (!Item) {
			m_ListPanel.SetSelectedIndex(0);
			Item = m_ListPanel.GetSelected();
		}

		if (Item) {
			m_TextPanel.SetText(Item->m_Name);
		} else {
			m_TextPanel.SetText("");
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetSelectedIndex(int Index) {
		m_ListPanel.SetSelectedIndex(Index);
		m_OldSelection = Index;

		// Set the text to the item in the list panel
		if (const GUIListPanel::Item *Item = m_ListPanel.GetSelected()) { m_TextPanel.SetText(Item->m_Name); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIComboBox::RollbackSelection() {
		// Restore the previous selection
		if (m_OldSelection >= 0 && m_OldSelection < m_ListPanel.GetItemList()->size() && m_OldSelection != m_ListPanel.GetSelectedIndex()) {
			m_ListPanel.SetSelectedIndex(m_OldSelection);
			// Set the text to the item in the list panel
			if (const GUIListPanel::Item *Item = m_ListPanel.GetSelected()) {
				m_TextPanel.SetText(Item->m_Name);
				return true;
			}
		}

		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetDropHeight(int Drop) {
		m_DropHeight = Drop;
		m_DropHeight = std::max(m_DropHeight, 20);

		// Change the list panel
		m_ListPanel.SetSize(m_Width, m_DropHeight);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::StoreProperties() {
		m_Properties.AddProperty("DropHeight", m_DropHeight);
		if (m_DropDownStyle == DropDownStyles::DropDownList) {
			m_Properties.AddProperty("DropDownStyle", "DropDownList");
		} else if (m_DropDownStyle == DropDownStyles::DropDown) {
			m_Properties.AddProperty("DropDownStyle", "DropDown");
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetDropDownStyle(int Style) {
		if (Style == DropDownStyles::DropDown || Style == DropDownStyles::DropDownList) { m_DropDownStyle = Style; }

		m_TextPanel.SetLocked(m_DropDownStyle == DropDownStyles::DropDownList);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIComboBox::GetText() {
		if (m_DropDownStyle != DropDownStyles::DropDown) {
			return "";
		}
		return m_TextPanel.GetText();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetText(const std::string &Text) {
		if (m_DropDownStyle == DropDownStyles::DropDown) { m_TextPanel.SetText(Text); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ApplyProperties(GUIProperties *Props) {
		GUIControl::ApplyProperties(Props);

		m_Properties.GetPropertyValue("Dropheight", &m_DropHeight);
		m_DropHeight = std::max(m_DropHeight, 20);

		std::string value;
		m_Properties.GetPropertyValue("DropDownStyle", &value);
		if (value == "DropDownList") {
			m_DropDownStyle = DropDownStyles::DropDownList;
		} else if (value == "DropDown") {
			m_DropDownStyle = DropDownStyles::DropDown;
		}
		m_TextPanel.SetLocked(m_DropDownStyle == DropDownList);

		// Force a rebuild of the bitmap
		ChangeSkin(m_Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::ChangeSkin(GUISkin *Skin) {
		// Free any old bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}

		// Create a new bitmap. Same width, but double the height to allow for both up
		// AND down states
		m_DrawBitmap = Skin->CreateBitmap(m_Width, m_Height * 2);

		// Create the button image
		Skin->BuildStandardRect(m_DrawBitmap, "ComboBox_ButtonUp", 0, 0, m_Width, m_Height);
		Skin->BuildStandardRect(m_DrawBitmap, "ComboBox_ButtonDown", 0, m_Height, m_Width, m_Height);

		// Draw the arrow
		std::string Filename;
		Skin->GetValue("ComboBox_Arrow", "Filename", &Filename);
		GUIBitmap *Arrow = Skin->CreateBitmap(Filename);
		if (!Arrow) {
			return;
		}

		int ColorKey;
		Skin->GetValue("ComboBox_Arrow", "ColorKeyIndex", &ColorKey);
		ColorKey = Skin->ConvertColor(ColorKey, m_DrawBitmap->GetColorDepth());
		Arrow->SetColorKey(ColorKey);

		int Values[4];
		GUIRect Rect;
		Skin->GetValue("ComboBox_Arrow", "Rect", Values, 4);
		SetRect(&Rect, Values[0], Values[1], Values[0] + Values[2], Values[1] + Values[3]);

		Arrow->DrawTrans(m_DrawBitmap, (m_Width / 2) - (Values[2] / 2), (m_Height / 2) - (Values[3] / 2), &Rect);

		Arrow->DrawTrans(m_DrawBitmap, (m_Width / 2) - (Values[2] / 2) + 1, m_Height + (m_Height / 2) - (Values[3] / 2) + 1, &Rect);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::Draw(GUIScreen *Screen) {
		GUIRect Rect;
		SetRect(&Rect, 0, m_Pushed ? m_Height : 0, m_Width, m_Pushed ? m_Height * 2 : m_Height);

		m_DrawBitmap->Draw(Screen->GetBitmap(), m_X, m_Y, &Rect);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::Create(int X, int Y, int Width, int Height) {
		m_X = X;
		m_Y = Y;
		m_Width = Width;
		m_Height = Height;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		if (Buttons & GUIControl::MouseButtons::MOUSE_LEFT) {
			m_Pushed = true;
			SendSignal(GUIEventCode::Clicked, Buttons);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIComboBoxButton::~GUIComboBoxButton() {
		// Free the drawing bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}
	}
}