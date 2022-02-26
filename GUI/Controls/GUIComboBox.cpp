#include "GUI.h"
#include "GUIComboBox.h"
#include "GUILabel.h"
#include "GUIButton.h"
#include "GUIListBox.h"

namespace RTE {

	const std::string_view GUIComboBox::c_ControlType = "COMBOBOX";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Create(const std::string_view &name, int posX, int posY, int width, int height) {
		GUIControl::Create(name, posX, posY, (width > 0) ? std::max(width, m_MinWidth) : m_DefaultWidth, (height > 0) ? std::max(height, m_MinHeight) : m_DefaultHeight);
		CreateChildren();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Create(GUIProperties *Props) {
		GUIControl::Create(Props);

		// Make sure the textbox isn't too small
		m_Width = std::max(m_Width, m_MinWidth);
		m_Height = std::max(m_Height, m_MinHeight);

		CreateChildren();

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
		//m_TextPanel->SetLocked(m_DropDownStyle == DropDownStyles::DropDownList);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::CreateChildren() {
		m_TextPanel = std::make_unique<GUILabel>(m_OwningManager);

		m_TextPanel->Create("", m_PosX, m_PosY, m_Width - 12, m_Height);
		m_TextPanel->SetVisible(true);
		//m_TextPanel->SetLocked(m_DropDownStyle == DropDownStyles::DropDownList);
		m_TextPanel->SetSignalTarget(this);

		m_ListPanel = std::make_unique<GUIListBox>(m_OwningManager);
		m_ListPanel->Create("", m_PosX, m_PosY + m_Height, m_Width, m_DropHeight);
		//m_ListPanel->ChangeSkin(m_Skin);
		m_ListPanel->SetVisible(false);
		m_ListPanel->SetSignalTarget(this);
		m_ListPanel->SetMultiSelect(false);
		m_ListPanel->SetHotTracking(true);
		m_ListPanel->EnableScrollbars(false, true);

		m_CreatedList = true;

		m_Button.Create(m_PosX + m_Width - 17, m_PosY, 17, m_Height);
		m_Button.SetSignalTarget(this);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ChangeSkin(GUISkin *Skin) {
		GUIControl::ChangeSkin(Skin);

		// Create a new bitmap
		m_DrawBitmap.reset(m_Skin->CreateBitmap(m_Width, m_Height));

		// Build the background
		m_Skin->BuildStandardRect(m_DrawBitmap.get(), "TextBox", 0, 0, m_Width, m_Height);

		// Setup the skin in the panels too
		m_TextPanel->ChangeSkin(Skin);

		if (m_CreatedList) { m_ListPanel->ChangeSkin(Skin); }

		m_Button.ChangeSkin(Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Draw(GUIScreen *Screen) {
		// Draw the background
		m_DrawBitmap->Draw(Screen->GetBitmap(), m_PosX, m_PosY, nullptr);

		m_TextPanel->Draw(Screen->GetBitmap());
		m_ListPanel->Draw(Screen);

		// If selected item has a bitmap AND no text to show, just show the bitmap as the selected thing
		if (m_ListPanel->GetSelected() && m_ListPanel->GetSelected()->m_Name.empty() && m_ListPanel->GetSelected()->m_pBitmap) { m_ListPanel->GetSelected()->m_pBitmap->DrawTrans(Screen->GetBitmap(), m_PosX + 4, m_PosY + 4, nullptr); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ReceiveSignal(GUIControl * Source, GUIEventCode Code, int Data) {
		GUIAssert(Source, "");

		int sourcePanelID = Source->GetUniqueID();

		// ComboBoxButton
		if (sourcePanelID == m_Button.GetUniqueID()) {
			// Clicked and list panel is not visible. open the list panel.
			if (Code == GUIEventCode::Clicked && !m_ListPanel->GetVisible()) {
				m_ListPanel->SetVisible(true);
				m_ListPanel->SetFocus();
				m_ListPanel->CaptureMouse();

				// Force a redraw
				m_ListPanel->EndUpdate();

				// Make this panel go above the rest
				m_ListPanel->ChangeZPosition(ZPosChangeType::MoveToTop);

				// Save the current selection
				if (m_ListPanel->GetSelectedIndex() >= 0 && m_ListPanel->GetSelectedIndex() < m_ListPanel->GetItemList()->size()) { m_OldSelection = m_ListPanel->GetSelectedIndex(); }

				AddEvent(GUIEventType::Notification, GUIEventCode::Dropped);
			}
		} else if (sourcePanelID == m_TextPanel->GetUniqueID()) {
			// Textbox
			// MouseDown
			if (Code == GUIEventCode::MouseDown && m_DropDownStyle == DropDownStyles::DropDownList && Data & GUIControl::MouseButtons::MOUSE_LEFT) {
				// Drop
				m_ListPanel->SetVisible(true);
				m_ListPanel->SetFocus();
				m_ListPanel->CaptureMouse();

				// Force a redraw
				m_ListPanel->EndUpdate();

				// Make this panel go above the rest
				m_ListPanel->ChangeZPosition(ZPosChangeType::MoveToTop);

				// Save the current selection
				if (m_ListPanel->GetSelectedIndex() >= 0 && m_ListPanel->GetSelectedIndex() < m_ListPanel->GetItemList()->size()) { m_OldSelection = m_ListPanel->GetSelectedIndex(); }

				AddEvent(GUIEventType::Notification, GUIEventCode::Dropped);
			}

		} else if (sourcePanelID == m_ListPanel->GetUniqueID()) {
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
				m_ListPanel->SetVisible(false);
				m_ListPanel->ReleaseMouse();
				m_OwningManager->SetFocusedControl(nullptr);
				m_Button.SetPushed(false);

				// Restore the old selection
				m_ListPanel->SetSelectedIndex(m_OldSelection);

				AddEvent(GUIEventType::Notification, GUIEventCode::Closed);
			} else if (Code == GUIEventCode::MouseUp && m_ListPanel->PointInsideList(mouseX, mouseY)) {
				// Select on mouse up instead of down so we don't accidentally click stuff behind the disappearing listbox immediately after. Also only work if inside the actual list, and not its scrollbars.
				// Hide the list panel
				m_ListPanel->SetVisible(false);
				m_ListPanel->ReleaseMouse();
				m_OwningManager->SetFocusedControl(nullptr);
				m_Button.SetPushed(false);

				AddEvent(GUIEventType::Notification, GUIEventCode::Closed);

				// Set the text to the item in the list panel
				GUIListBox::Item* Item = m_ListPanel->GetSelected();
				if (Item) {
					m_TextPanel->SetText(Item->m_Name);
				} else {
					// Restore the old selection
					m_ListPanel->SetSelectedIndex(m_OldSelection);
				}
			}

			if (m_DropDownStyle == DropDownList) {
				// Set the text to the item in the list panel
				if (const GUIListBox::Item* Item = m_ListPanel->GetSelected()) { m_TextPanel->SetText(Item->m_Name); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::ClearList() {
		m_TextPanel->SetText("");
		m_ListPanel->ClearList();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIComboBox::GetCount() {
		return m_ListPanel->GetItemList()->size();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIComboBox::GetSelectedIndex() {
		return m_ListPanel->GetSelectedIndex();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Move(int newPosX, int newPosY) {
		GUIControl::Move(newPosX, newPosY);
		m_ListPanel->Move(m_PosX, m_PosY + m_Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::MoveRelative(int relX, int relY) {
		GUIControl::MoveRelative(relX, relY);
		m_ListPanel->Move(m_PosX, m_PosY + m_Height);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::Resize(int newWidth, int newHeight) {
		if (m_Width != newWidth || m_Height != newHeight) {
			GUIControl::Resize(std::max(newWidth, m_MinWidth), std::max(newHeight, m_MinHeight));

			m_TextPanel->Resize(m_Width - 12, m_Height);
			m_TextPanel->Move(m_PosX, m_PosY);

			m_Button.Move(m_PosX + m_Width - 13, m_PosY + 1);
			m_Button.Resize(12, m_Height - 2);

			m_ListPanel->Resize(m_Width, m_DropHeight);
			m_ListPanel->Move(m_PosX, m_PosY + m_Height);

			ChangeSkin(m_Skin);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::AddItem(const std::string &Name, const std::string &ExtraText, GUIBitmap *pBitmap, const Entity *pEntity) {
		m_ListPanel->AddItem(Name, ExtraText, pBitmap, pEntity);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::DeleteItem(int Index) {
		m_ListPanel->DeleteItem(Index);

		// Update the selection
		const GUIListBox::Item *Item = m_ListPanel->GetSelected();
		if (!Item) {
			m_ListPanel->SetSelectedIndex(0);
			Item = m_ListPanel->GetSelected();
		}

		if (Item) {
			m_TextPanel->SetText(Item->m_Name);
		} else {
			m_TextPanel->SetText("");
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetSelectedIndex(int Index) {
		m_ListPanel->SetSelectedIndex(Index);
		m_OldSelection = Index;

		// Set the text to the item in the list panel
		if (const GUIListBox::Item *Item = m_ListPanel->GetSelected()) { m_TextPanel->SetText(Item->m_Name); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIComboBox::RollbackSelection() {
		// Restore the previous selection
		if (m_OldSelection >= 0 && m_OldSelection < m_ListPanel->GetItemList()->size() && m_OldSelection != m_ListPanel->GetSelectedIndex()) {
			m_ListPanel->SetSelectedIndex(m_OldSelection);
			// Set the text to the item in the list panel
			if (const GUIListBox::Item *Item = m_ListPanel->GetSelected()) {
				m_TextPanel->SetText(Item->m_Name);
				return true;
			}
		}

		return false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIListBox::Item * GUIComboBox::GetItem(int Index) {
		return m_ListPanel->GetItem(Index);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetDropHeight(int Drop) {
		m_DropHeight = Drop;
		m_DropHeight = std::max(m_DropHeight, 20);

		// Change the list panel
		m_ListPanel->Resize(m_Width, m_DropHeight);
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

	bool GUIComboBox::IsDropped() const {
		return m_ListPanel->GetVisible();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetDropDownStyle(DropDownStyles Style) {
		if (Style == DropDownStyles::DropDown || Style == DropDownStyles::DropDownList) { m_DropDownStyle = Style; }

		//m_TextPanel->SetLocked(m_DropDownStyle == DropDownStyles::DropDownList);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIComboBox::GetText() {
		if (m_DropDownStyle != DropDownStyles::DropDown) {
			return "";
		}
		return m_TextPanel->GetText();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBox::SetText(const std::string &Text) {
		if (m_DropDownStyle == DropDownStyles::DropDown) { m_TextPanel->SetText(Text); }
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
		//m_TextPanel->SetLocked(m_DropDownStyle == DropDownList);

		// Force a rebuild of the bitmap
		ChangeSkin(m_Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::ChangeSkin(GUISkin *Skin) {
		// Create a new bitmap. Same width, but double the height to allow for both up AND down states
		m_DrawBitmap.reset(Skin->CreateBitmap(m_Width, m_Height * 2));

		// Create the button image
		Skin->BuildStandardRect(m_DrawBitmap.get(), "ComboBox_ButtonUp", 0, 0, m_Width, m_Height);
		Skin->BuildStandardRect(m_DrawBitmap.get(), "ComboBox_ButtonDown", 0, m_Height, m_Width, m_Height);

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

		Arrow->DrawTrans(m_DrawBitmap.get(), (m_Width / 2) - (Values[2] / 2), (m_Height / 2) - (Values[3] / 2), &Rect);

		Arrow->DrawTrans(m_DrawBitmap.get(), (m_Width / 2) - (Values[2] / 2) + 1, m_Height + (m_Height / 2) - (Values[3] / 2) + 1, &Rect);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::Draw(GUIScreen *Screen) {
		GUIRect Rect;
		SetRect(&Rect, 0, m_Pushed ? m_Height : 0, m_Width, m_Pushed ? m_Height * 2 : m_Height);

		m_DrawBitmap->Draw(Screen->GetBitmap(), m_PosX, m_PosY, &Rect);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIComboBoxButton::Create(int X, int Y, int Width, int Height) {
		m_PosX = X;
		m_PosY = Y;
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
}