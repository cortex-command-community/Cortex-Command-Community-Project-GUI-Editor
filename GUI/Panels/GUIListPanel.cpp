#include "GUI.h"
#include "GUIListPanel.h"

#define RIGHTTEXTWIDTH 36

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::Create(int X, int Y, int Width, int Height) {
		GUIControl::Create("", X, Y, Width, Height);

		GUIAssert(m_OwningManager, "");

		// Initial size & positions
		m_HorzScroll.Create(0 + m_ScrollBarPadding, Height - m_ScrollBarThickness - m_ScrollBarPadding, Width - (m_ScrollBarPadding * 2), m_ScrollBarThickness);
		m_HorzScroll.SetOrientation(GUIScrollPanel::Horizontal);
		m_HorzScroll.SetVisible(false);
		m_HorzScroll.SetValue(0);
		m_HorzScroll.SetSignalTarget(this);

		m_VertScroll.Create(Width - m_ScrollBarThickness - m_ScrollBarPadding, 0 + m_ScrollBarPadding, m_ScrollBarThickness, Height - (m_ScrollBarPadding * 2));
		m_VertScroll.SetOrientation(GUIScrollPanel::Vertical);
		m_VertScroll.SetVisible(false);
		m_VertScroll.SetValue(0);
		m_VertScroll.SetSignalTarget(this);

		AddChild(&m_HorzScroll);
		AddChild(&m_VertScroll);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::Destroy() {
		// Destroy the items
		std::vector<Item *>::iterator it;

		for (it = m_Items.begin(); it != m_Items.end(); it++) {
			Item *I = *it;
			if (I) { delete I; }
		}
		m_Items.clear();

		m_HorzScroll.Destroy();
		m_VertScroll.Destroy();

		// Destroy the drawing bitmap
		if (m_FrameBitmap) {
			m_FrameBitmap->Destroy();
			delete m_FrameBitmap;
			m_FrameBitmap = nullptr;
		}

		// Destroy the drawing bitmap
		if (m_DrawBitmap) {
			m_DrawBitmap->Destroy();
			delete m_DrawBitmap;
			m_DrawBitmap = nullptr;
		}

		// Destroy the base bitmap
		if (m_BaseBitmap) {
			m_BaseBitmap->Destroy();
			delete m_BaseBitmap;
			m_BaseBitmap = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ClearList() {
		// Destroy the items
		std::vector<Item *>::iterator it;

		for (it = m_Items.begin(); it != m_Items.end(); it++) {
			Item *I = *it;
			if (I) { delete I; }
		}

		m_Items.clear();

		m_SelectedList.clear();

		// Adjust the scrollbars & text
		AdjustScrollbars();
		ScrollToTop();

		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::AddItem(const std::string &Name, const std::string &rightText, GUIBitmap *pBitmap, const Entity *pEntity, const int extraIndex) {
		Item *I = new Item;
		I->m_Name = Name;
		I->m_RightText = rightText;
		I->m_ExtraIndex = extraIndex;
		I->m_Selected = false;
		I->m_pBitmap = pBitmap;
		I->m_pEntity = pEntity;
		I->m_Height = GetItemHeight(I);
		I->m_ID = m_Items.size();

		m_Items.push_back(I);

		// Calculate the largest width
		if (m_Font) {
			int FWidth = m_Font->CalculateWidth(Name);
			m_LargestWidth = std::max(m_LargestWidth, FWidth);
		}

		// Adjust the scrollbars
		AdjustScrollbars();
		ScrollToItem(I);

		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ChangeSkin(GUISkin *Skin) {
		GUIAssert(Skin, "");

		m_Skin = Skin;

		// Set the scrollpanel's skins
		m_HorzScroll.ChangeSkin(Skin);
		m_VertScroll.ChangeSkin(Skin);

		// Adjust the scrollbars
		AdjustScrollbars();

		// Build the bitmap
		BuildBitmap(true, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::BuildBitmap(bool UpdateBase, bool UpdateText) {
		// Gotta update the text if updating the base
		if (UpdateBase)
			UpdateText = true;

		// Free any old bitmaps
		if (UpdateBase) {
			if (m_FrameBitmap) {
				m_FrameBitmap->Destroy();
				delete m_FrameBitmap;
				m_FrameBitmap = nullptr;
			}
			if (m_DrawBitmap) {
				m_DrawBitmap->Destroy();
				delete m_DrawBitmap;
				m_DrawBitmap = nullptr;
			}
			if (m_BaseBitmap) {
				m_BaseBitmap->Destroy();
				delete m_BaseBitmap;
				m_BaseBitmap = nullptr;
			}
		}

		// Create a new bitmap.
		if (UpdateBase) {
			m_FrameBitmap = m_Skin->CreateBitmap(m_Width, m_Height);
			m_DrawBitmap = m_Skin->CreateBitmap(m_Width, m_Height);
			m_BaseBitmap = m_Skin->CreateBitmap(m_Width, m_Height);
		}

		if (UpdateBase) {
			std::string Filename;

			m_Skin->GetValue("Listbox", "SelectedColorIndex", &m_SelectedColorIndex);
			m_SelectedColorIndex = m_Skin->ConvertColor(m_SelectedColorIndex, m_BaseBitmap->GetColorDepth());

			// Load the font
			m_Skin->GetValue("Listbox", "Font", &Filename);
			m_Font = m_Skin->GetFont(Filename);
			m_Skin->GetValue("Listbox", "FontColor", &m_FontColor);
			m_Skin->GetValue("Listbox", "FontShadow", &m_FontShadow);
			m_Skin->GetValue("Listbox", "FontKerning", &m_FontKerning);
			m_Skin->GetValue("Listbox", "FontSelectColor", &m_FontSelectColor);
			m_FontColor = m_Skin->ConvertColor(m_FontColor, m_BaseBitmap->GetColorDepth());
			m_FontSelectColor = m_Skin->ConvertColor(m_FontSelectColor, m_BaseBitmap->GetColorDepth());

			m_Font->CacheColor(m_FontColor);
			m_Font->CacheColor(m_FontSelectColor);

			// Build only the background                                                 BG   Frame
			m_Skin->BuildStandardRect(m_BaseBitmap, "Listbox", 0, 0, m_Width, m_Height, true, false);

			// Now build only the frame, and draw it on top of the 'text' layer later
			m_Skin->BuildStandardRect(m_FrameBitmap, "Listbox", 0, 0, m_Width, m_Height, false, true);
		}

		if (UpdateText) {
			m_BaseBitmap->Draw(m_DrawBitmap, 0, 0, nullptr);

			// Draw the text onto the drawing bitmap
			BuildDrawBitmap();

			m_FrameBitmap->DrawTrans(m_DrawBitmap, 0, 0, nullptr);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::BuildDrawBitmap() {
		// Draw the items
		std::vector<Item *>::iterator it;
		int Count = 0;
		int Height = m_Height;
		if (m_HorzScroll.GetVisible()) { Height -= m_HorzScroll.GetHeight(); }

		int x = m_HorzScroll.GetValue();
		int y = 1 + (m_VertScroll.GetVisible() ? -m_VertScroll.GetValue() : 0);
		int stackHeight = 0;
		int thirdWidth = m_Width / 3;

		// Go through each item
		for (it = m_Items.begin(); it != m_Items.end(); it++, Count++) {
			stackHeight += GetItemHeight(*it);
			// Only draw the items after the scroll value
			if (m_VertScroll.GetVisible() && stackHeight < m_VertScroll.GetValue()) {
				y += GetItemHeight(*it);
				continue;
			}

			Item *I = *it;

			// Alternate drawing mode
			// TODO: REMOVE MORE H-CODING
			if (m_AlternateDrawMode) {
				int rightTextWidth = I->m_RightText.empty() ? 0 : RIGHTTEXTWIDTH;//thirdWidth / 2;
				int mainTextWidth = (thirdWidth * 2) - rightTextWidth;
				int bitmapWidth = I->m_pBitmap ? I->m_pBitmap->GetWidth() : 0;
				int bitmapHeight = I->m_pBitmap ? I->m_pBitmap->GetHeight() : 0;
				if (!I->m_Name.empty() && !I->m_RightText.empty() && bitmapWidth > thirdWidth) {
					bitmapHeight *= (float)thirdWidth / (float)bitmapWidth;
					bitmapWidth = thirdWidth;
				}

				int textHeight = m_Font->CalculateHeight(I->m_Name, mainTextWidth);
				int itemHeight = std::max(bitmapHeight + 4, textHeight + 2);
				int textX = thirdWidth + 6 - x;
				int textY = y + (itemHeight / 2) + 1;
				int bitmapY = y + (itemHeight / 2) - (bitmapHeight / 2) + 1;

				// Draw the associated bitmap
				if (I->m_pBitmap) {
					// If it was deemed too large, draw it scaled
					if (bitmapWidth == thirdWidth) {
						I->m_pBitmap->DrawTransScaled(m_DrawBitmap, 3 - x, bitmapY, bitmapWidth, bitmapHeight);
						// There's text to compete for space with
					} else if (!I->m_Name.empty()) {
						I->m_pBitmap->DrawTrans(m_DrawBitmap, ((thirdWidth / 2) - (bitmapWidth / 2)) - x + 2, bitmapY, nullptr);
						// No text, just bitmap, so give it more room
					} else {
						I->m_pBitmap->DrawTrans(m_DrawBitmap, ((thirdWidth / 2) - (bitmapWidth / 2)) - x + 4, bitmapY, nullptr);
					}
				}

				// Selected item
				if (I->m_Selected && m_GotFocus) {
					m_DrawBitmap->DrawLine(4, y + 1, m_Width - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() + 2 : 5), y + 1, m_SelectedColorIndex);
					m_DrawBitmap->DrawLine(4, y + itemHeight, m_Width - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() + 2 : 5), y + itemHeight, m_SelectedColorIndex);
					m_Font->SetColor(m_FontSelectColor);
					m_Font->DrawAligned(m_DrawBitmap, x - 6 - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() : 0) + m_Width, textY, I->m_RightText, GUIFont::HAlignment::Right, GUIFont::VAlignment::Middle, m_Width, m_FontShadow);
					m_Font->DrawAligned(m_DrawBitmap, textX, textY, I->m_Name, GUIFont::HAlignment::Left, GUIFont::VAlignment::Middle, mainTextWidth);
				} else {
					// Unselected
					// TODO: Don't hardcode unselected color index
					m_DrawBitmap->DrawLine(4, y + 1, m_Width - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() + 2 : 5), y + 1, 144);
					m_DrawBitmap->DrawLine(4, y + itemHeight, m_Width - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() + 2 : 5), y + itemHeight, 144);
					m_Font->SetColor(m_FontColor);
					m_Font->SetKerning(m_FontKerning);
					m_Font->DrawAligned(m_DrawBitmap, x - 6 - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() : 0) + m_Width, textY, I->m_RightText, GUIFont::HAlignment::Right, GUIFont::VAlignment::Middle, m_Width, m_FontShadow);
					m_Font->DrawAligned(m_DrawBitmap, textX, textY, I->m_Name, GUIFont::HAlignment::Left, GUIFont::VAlignment::Middle, mainTextWidth, m_FontShadow);
				}

				// Draw another line to make sure the last item has two
				if (it == m_Items.end() - 1) { m_DrawBitmap->DrawLine(4, y + itemHeight + 1, m_Width - 5, y + itemHeight + 1, 144); }

				// Save the item height for later use in selection routines etc
				I->m_Height = itemHeight;
				y += itemHeight;
			} else {
				// Regular drawing
				// Selected item
				if (I->m_Selected) {
					m_Font->SetColor(m_SelectedColorIndex);
					m_DrawBitmap->DrawRectangle(1, y, m_Width - 2, m_Font->GetFontHeight(), m_SelectedColorIndex, m_GotFocus); // Filled if we have focus
				}

				if (I->m_Selected && m_GotFocus) {
					m_Font->SetColor(m_FontSelectColor);
					m_Font->DrawAligned(m_DrawBitmap, x - 3 + m_Width - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() : 0), y, I->m_RightText, GUIFont::HAlignment::Right);
					m_Font->Draw(m_DrawBitmap, 4 - x, y, I->m_Name);
				} else {
					// Unselected
					m_Font->SetColor(m_FontColor);
					m_Font->SetKerning(m_FontKerning);
					m_Font->DrawAligned(m_DrawBitmap, x - 3 + m_Width - (m_VertScroll.GetVisible() ? m_VertScroll.GetWidth() : 0), y, I->m_RightText, GUIFont::HAlignment::Right, GUIFont::VAlignment::Top, m_Width, m_FontShadow);
					m_Font->Draw(m_DrawBitmap, 4 - x, y, I->m_Name, m_FontShadow);
				}

				y += GetItemHeight(I);
			}

			// No more room to draw
			if (y > Height) {
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::Draw(GUIScreen *Screen) {
		// Draw the base
		m_DrawBitmap->Draw(Screen->GetBitmap(), m_X, m_Y, nullptr);

		// Draw any children
		GUIControl::Draw(Screen);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnMouseDown(int X, int Y, int Buttons, int Modifier) {
		m_ExternalCapture = IsCaptured();

		// Over a scrollbar
		if (m_VertScroll.GetVisible() && m_VertScroll.PointInside(X, Y)) {
			m_VertScroll.OnMouseDown(X, Y, Buttons, Modifier);
			return;
		}
		if (m_HorzScroll.GetVisible() && m_HorzScroll.PointInside(X, Y)) {
			m_HorzScroll.OnMouseDown(X, Y, Buttons, Modifier);
			return;
		}

		// Give this panel focus
		SetFocus();

		if ((Buttons & GUIControl::MouseButtons::MOUSE_LEFT) && PointInside(X, Y)) {
			SelectItem(X, Y, Modifier);
			SendSignal(GUIEventCode::MouseDown, Buttons);
		} else {
			// Click signifies mouse down anywhere outside the list panel.
			SendSignal(GUIEventCode::Click, Buttons);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnMouseWheelChange(int x, int y, int modifier, int mouseWheelChange) {
		if (!m_MouseScroll) {
			return;
		} else if (m_VertScroll.GetVisible() && m_VertScroll.PointInside(x, y)) {
			ScrollBarScrolling(mouseWheelChange);
		} else if (PointInsideList(x, y) && !m_MultiSelect) {
			SelectionListScrolling(mouseWheelChange);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SelectItem(int X, int Y, int Modifier) {
		std::vector<Item *>::iterator it;
		bool Shift = Modifier & MODI_SHIFT;
		bool Ctrl = Modifier & MODI_CTRL;

		// If multiselect is disabled, then clear all the items
		// If multiselect is enabled, but no useful modifier keys are down,
		//  then clear all the items
		bool ClearSelected = false;
		if (!m_MultiSelect) {
			ClearSelected = true;
		} else if (!Ctrl) {
			ClearSelected = true;
		}

		// Clear the selected items
		if (ClearSelected) {
			for (it = m_Items.begin(); it != m_Items.end(); it++) {
				Item *I = *it;
				I->m_Selected = false;
			}
			m_SelectedList.clear();
		}

		int Height = m_Height;
		if (m_HorzScroll.GetVisible()) { Height -= m_HorzScroll.GetHeight(); }

		int y = m_Y + 1;
		if (m_VertScroll.GetVisible())
			y -= m_VertScroll.GetValue();
		int Count = 0;
		//int stackHeight = 0;
		for (it = m_Items.begin(); it != m_Items.end(); it++, Count++) {
			/*
			stackHeight += GetItemHeight(*it);
			// Only check the items after the scroll value
			if (m_VertScroll.GetVisible() && m_VertScroll.GetValue() > y) {
				y += GetItemHeight(*it);
				continue;
			}
			if (Count < m_VertScroll.GetValue()) {continue;}
			*/
			Item *I = *it;

			// Select/Deselect the item under the mouse
			//if (Y >= y && Y < y+m_Font->GetFontHeight()) {
			if (Y >= y && Y < y + GetItemHeight(I)) {
				// If CTRL is down and not shift, AND the item is already selected
				// Unselect it
				if (Ctrl && !Shift && I->m_Selected && m_MultiSelect) {
					I->m_Selected = false;
					break;
				}

				// Normal select
				if ((!Ctrl && !Shift) || !m_MultiSelect) {
					I->m_Selected = true;
					m_SelectedList.push_back(I);
					m_LastSelected = Count;
					SendSignal(GUIEventCode::Select, 0);
				}

				// Multiselect individual items
				if (m_MultiSelect) {
					// Control key down
					if (Ctrl) {
						I->m_Selected = true;
						m_SelectedList.push_back(I);
						SendSignal(GUIEventCode::Select, 0);
						if (!Shift) { m_LastSelected = Count; }
					}
					// Shift key down
					if (Shift) {
						if (m_LastSelected == -1) {
							I->m_Selected = true;
							m_SelectedList.push_back(I);
							SendSignal(GUIEventCode::Select, 0);
							m_LastSelected = Count;
						} else {
							// Select a list of items
							std::vector<Item *>::iterator sel;
							int Num = 0;
							for (sel = m_Items.begin(); sel != m_Items.end(); sel++, Num++) {
								if (m_LastSelected <= Count) {
									if (Num >= m_LastSelected && Num <= Count) {
										Item *SelItem = *sel;
										SelItem->m_Selected = true;
										m_SelectedList.push_back(SelItem);
										SendSignal(GUIEventCode::Select, 0);
									}
								} else {
									if (Num >= Count && Num <= m_LastSelected) {
										Item *SelItem = *sel;
										SelItem->m_Selected = true;
										m_SelectedList.push_back(SelItem);
										SendSignal(GUIEventCode::Select, 0);
									}
								}
							}
						}
					}
				}
				break;
			}
			//y += m_Font->GetFontHeight();
			y += GetItemHeight(I);

			// End of viewable region
			if (y > m_Y + Height) {
				break;
			}
		}
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnMouseUp(int X, int Y, int Buttons, int Modifier) {
		if (m_CapturedVert) {
			m_VertScroll.OnMouseUp(X, Y, Buttons, Modifier);
		} else if (m_CapturedHorz) {
			m_HorzScroll.OnMouseUp(X, Y, Buttons, Modifier);
		} else {
			if (PointInside(X, Y)) {
				//SendSignal(Select, 0);
			//} else {
				SendSignal(GUIEventCode::MouseUp, Buttons);
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnMouseMove(int X, int Y, int Buttons, int Modifier) {
		if (m_CapturedVert) {
			m_VertScroll.OnMouseMove(X, Y, Buttons, Modifier);
		} else if (m_CapturedHorz) {
			m_HorzScroll.OnMouseMove(X, Y, Buttons, Modifier);
		} else if (PointInsideList(X, Y)) {
			// Using Hot-Tracking
			if (m_HotTracking && GetItem(X, Y) != nullptr && (GetItem(X, Y) != GetSelected())) { SelectItem(X, Y, Modifier); }
			SendSignal(GUIEventCode::MouseMove, Buttons);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnDoubleClick(int X, int Y, int Buttons, int Modifier) {
		if (PointInside(X, Y)) { SendSignal(GUIEventCode::DoubleClick, Buttons); }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::EndUpdate() {
		m_UpdateLocked = false;

		// Invoke an update by called the ChangeSkin function
		ChangeSkin(m_Skin);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollToItem(Item *pItem) {
		if (pItem && m_VertScroll.GetVisible()) {
			int stackHeight = GetStackHeight(pItem);
			int itemHeight = GetItemHeight(pItem);
			// Adjust the vertical scroll bar to show the specified item
			if (stackHeight < m_VertScroll.GetValue()) { m_VertScroll.SetValue(stackHeight); }
			if (stackHeight + itemHeight > m_VertScroll.GetValue() + m_VertScroll.GetPageSize()) { m_VertScroll.SetValue(stackHeight + itemHeight - m_VertScroll.GetPageSize()); }
		}
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollUp() {
		if (m_VertScroll.GetVisible()) { m_VertScroll.SetValue(m_VertScroll.GetValue() - 20); }
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollDown() {
		if (m_VertScroll.GetVisible()) {
			int scrollValueToAdd = 20;
			if (!m_Items.empty()) {
				RTE::GUIListPanel::Item *item = m_Items.back();
				int maximumScrollDistance = GetStackHeight(item) + GetItemHeight(item) - (m_VertScroll.GetPageSize() + m_VertScroll.GetValue());
				scrollValueToAdd = std::clamp(maximumScrollDistance, 0, scrollValueToAdd);
			}
			m_VertScroll.SetValue(m_VertScroll.GetValue() + scrollValueToAdd);
		}
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollTo(int position) {
		if (m_VertScroll.GetVisible()) {
			m_VertScroll.SetValue(position);

			//TODO this was copied from MaxShadow's work. I'm not quite sure of the point of it tbh.
			if (!m_Items.empty()) {
				RTE::GUIListPanel::Item *item = m_Items.back();
				int allItemsHeight = GetStackHeight(item) + GetItemHeight(item);
				if (position + m_VertScroll.GetPageSize() > allItemsHeight) { m_VertScroll.SetValue(allItemsHeight - m_VertScroll.GetPageSize()); }
			}
		}
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollToSelected() {
		if (!m_SelectedList.empty()) {
			ScrollToItem(*(m_SelectedList.begin()));
			BuildBitmap(false, true);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollToTop() {
		m_VertScroll.SetValue(0);
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollToBottom() {
		if (m_Items.empty()) {
			m_VertScroll.SetValue(0);
		} else {
			ScrollToItem(m_Items.back());
		}
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ScrollBarScrolling(int mouseWheelChange) {
		int newValue = 0;
		Item* lastItem = GetItem(GetItemList()->size() - 1);
		int avgItemHeight = static_cast<int>((GetStackHeight(lastItem) + GetItemHeight(lastItem)) / GetItemList()->size());
		if (mouseWheelChange < 0) {
			newValue = m_VertScroll.GetValue() - (mouseWheelChange * avgItemHeight);
			int maxValue = GetStackHeight(lastItem) + GetItemHeight(lastItem) - m_VertScroll.GetPageSize();
			newValue = std::clamp(newValue, maxValue, 0);
		} else {
			newValue = m_VertScroll.GetValue() - (mouseWheelChange * avgItemHeight);
			if (newValue < 0) {
				newValue = 0;
			}
		}
		m_VertScroll.SetValue(newValue);
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SelectionListScrolling(int scrollDir) {
		std::size_t itemListSize = GetItemList()->size();
		if (!itemListSize || !scrollDir) {
			return;
		}

		int oldItemIndex = GetSelectedIndex();
		int newItemIndex = (scrollDir > 0) ? (oldItemIndex - 1) : (oldItemIndex + 1);
		if (newItemIndex < 0 || newItemIndex >= itemListSize) {
			if (m_LoopSelectionScroll) {
				newItemIndex = (newItemIndex + itemListSize) % itemListSize;
			} else {
				newItemIndex = std::clamp(newItemIndex, 0, static_cast<int>(itemListSize - 1));
				if (oldItemIndex == newItemIndex) {
					SendSignal(GUIEventCode::EdgeHit, (newItemIndex < 0) ? 0 : 1);
					return;
				}
			}
		}
		SetSelectedIndex(newItemIndex);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::AdjustScrollbars() {
		// If the update is locked, don't update the scrollbars
		if (m_UpdateLocked) {
			return;
		}
		// Need the font setup
		if (!m_Font) {
			return;
		}
		if (!m_HorzScrollEnabled) { m_HorzScroll.SetVisible(false); }
		if (!m_VertScrollEnabled) { m_VertScroll.SetVisible(false); }

		// Re-adjust the scrollbar positions & sizes, just to be safe
		m_HorzScroll.SetPositionAbs(m_X + m_ScrollBarPadding, m_Y + m_Height - m_ScrollBarThickness - m_ScrollBarPadding);
		m_HorzScroll.SetSize(m_Width - (m_ScrollBarPadding * 2), m_ScrollBarThickness);
		m_VertScroll.SetPositionAbs(m_X + m_Width - m_ScrollBarThickness - m_ScrollBarPadding, m_Y + m_ScrollBarPadding);
		m_VertScroll.SetSize(m_ScrollBarThickness, m_Height - (m_ScrollBarPadding * 2));

		// If there are items wider than the listpanel, make the horizontal scrollpanel visible
		int Width = m_Width - 4;
		if (m_VertScroll.GetVisible()) { Width -= m_VertScroll.GetWidth(); }
		m_HorzScroll.SetVisible(false);
		if (m_LargestWidth > Width && m_HorzScrollEnabled) {
			m_HorzScroll.SetVisible(true);
			m_HorzScroll.SetMaximum(m_LargestWidth);
			m_HorzScroll.SetMinimum(0);
			m_HorzScroll.SetPageSize(Width);
			m_HorzScroll.SetSmallChange(m_Font->CalculateWidth("W"));    // W is one of the largest characters
		}

		if (m_Items.size() > 0) {
			// Get the total height of the item stack.
			int itemStackHeight = GetStackHeight();
			int itemHeight = itemStackHeight / m_Items.size();

			int Height = m_Height - (m_HorzScroll.GetVisible() ? m_HorzScroll.GetHeight() : 0);
			// TODO: remove the page subtraciton?
					// Subtract the frame size
			int Page = Height - 4;
			int Max = itemStackHeight/* - Page*/;
			Max = std::max(Max, 0);

			// Setup the vertical scrollbar
			m_VertScroll.SetPageSize(Page);
			m_VertScroll.SetMaximum(Max);
			m_VertScroll.SetMinimum(0);
			//        m_VertScroll.SetSmallChange(itemHeight);

					// If there is too many items, make the scrollbar visible
			m_VertScroll.SetVisible(false);
			if (itemStackHeight > Height && m_VertScrollEnabled) {
				m_VertScroll.SetVisible(true);

				// Setup the horizontal scrollbar again because this scrollbar just became visible
				// (Or still is visible)
				Width = m_Width - 4 - m_VertScroll.GetWidth();
				m_HorzScroll.SetVisible(false);
				if (m_LargestWidth > Width && m_HorzScrollEnabled) {
					m_HorzScroll.SetVisible(true);
					m_HorzScroll.SetMaximum(m_LargestWidth);
					m_HorzScroll.SetMinimum(0);
					m_HorzScroll.SetPageSize(Width);
					m_HorzScroll.SetSmallChange(m_Font->CalculateWidth("W"));    // W is one of the largest characters
				}
			}
		} else {
			// No items, so no vert scroll bar
			m_VertScroll.SetVisible(false);
		}

		// If both scrollbars are visible, adjust the size so they don't intersect
		if (m_VertScroll.GetVisible() && m_HorzScroll.GetVisible()) {
			m_VertScroll.SetSize(m_VertScroll.GetWidth(), m_Height - m_HorzScroll.GetHeight() - (m_ScrollBarPadding * 2));
			m_HorzScroll.SetSize(m_Width - m_VertScroll.GetWidth() - (m_ScrollBarPadding * 2), m_HorzScroll.GetHeight());
		} else {
			// Normal size
			if (m_VertScroll.GetVisible()) { m_VertScroll.SetSize(m_VertScroll.GetWidth(), m_Height - (m_ScrollBarPadding * 2)); }
			if (m_HorzScroll.GetVisible()) { m_HorzScroll.SetSize(m_Width - (m_ScrollBarPadding * 2), m_HorzScroll.GetHeight()); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnKeyPress(int KeyCode, int Modifier) {
		if (m_LastSelected < 0) {
			return;
		}
		if (m_Items.empty()) {
			return;
		}
		switch (KeyCode) {
			// Up Arrow
			case GUIInput::KeyboardKeys::KeyUpArrow:
				m_LastSelected--;
				break;

				// Down Arrow
			case GUIInput::KeyboardKeys::KeyDownArrow:
				m_LastSelected++;
				break;

				// Home
			case GUIInput::KeyboardKeys::KeyHome:
				m_LastSelected = 0;
				break;

				// End
			case GUIInput::KeyboardKeys::KeyEnd:
				m_LastSelected = m_Items.size() - 1;
				break;

				// Page Up
			case GUIInput::KeyboardKeys::KeyPageUp:
				m_LastSelected -= m_VertScroll.GetPageSize();
				break;

				// Page Down
			case GUIInput::KeyboardKeys::KeyPageDown:
				m_LastSelected += m_VertScroll.GetPageSize();
				break;

				// Different key
			default:
				return;
		}

		// Clear all the items
		std::vector<Item *>::iterator it;
		for (it = m_Items.begin(); it != m_Items.end(); it++) {
			Item *I = *it;
			I->m_Selected = false;
		}
		m_SelectedList.clear();

		// Clamp the value
		m_LastSelected = std::max(m_LastSelected, 0);
		m_LastSelected = std::min(m_LastSelected, static_cast<int>(m_Items.size() - 1));


		// Select the new item
		Item *I = m_Items[m_LastSelected];
		I->m_Selected = true;
		m_SelectedList.push_back(I);
		SendSignal(GUIEventCode::Select, 0);

		int stackHeight = GetStackHeight(I);
		int itemHeight = GetItemHeight(I);

		// Adjust the vertical scroll bar to show the newly selected item
		ScrollToItem(I);

		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIListPanel::PointInsideList(int X, int Y) {
		bool inside = PointInside(X, Y);
		// Exclude the scrollbars if we are meant to
		if (inside && m_HorzScroll.PointInside(X, Y) || m_VertScroll.PointInside(X, Y)) { inside = false; }
		return inside;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnGainFocus() {
		GUIControl::OnGainFocus();

		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::OnLoseFocus() {
		GUIControl::OnLoseFocus();

		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::ReceiveSignal(GUIControl *Source, GUIEventCode Code, int Data) {
		// ChangeValue signal from scrollpanels?
		GUIAssert(Source, "");

		int sourcePanelID = Source->GetUniqueID();

		if ((sourcePanelID == m_VertScroll.GetUniqueID() || sourcePanelID == m_HorzScroll.GetUniqueID()) && Code == GUIEventCode::ChangeValue) {
			BuildBitmap(false, true);
		} else if (sourcePanelID == m_VertScroll.GetUniqueID()) {
			// Vertical Scrollbar
			if (Code == GUIEventCode::Grab) {
				m_CapturedVert = true;
			} else if (Code == GUIEventCode::Release) {
				m_CapturedVert = false;
			}
		} else if (sourcePanelID == m_HorzScroll.GetUniqueID()) {
			// Horizontal Scrollbar
			if (Code == GUIEventCode::Grab) {
				m_CapturedHorz = true;
			} else if (Code == GUIEventCode::Release) {
				m_CapturedHorz = false;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SetMultiSelect(bool MultiSelect) {
		m_MultiSelect = MultiSelect;

		// Multi-select & Hot-Tracking are mutually exclusive
		if (m_MultiSelect) { m_HotTracking = false; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SetHotTracking(bool HotTrack) {
		m_HotTracking = HotTrack;

		// Multi-select & Hot-Tracking are mutually exclusive
		if (m_HotTracking) { m_MultiSelect = false; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIListPanel::Item * GUIListPanel::GetSelected() {
		// Nothing in the list
		if (m_SelectedList.empty()) {
			return nullptr;
		}
		// Get the first item
		return m_SelectedList.at(0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIListPanel::Item * GUIListPanel::GetItem(int Index) {
		if (Index >= 0 && Index < m_Items.size()) {
			return m_Items.at(Index);
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIListPanel::Item * GUIListPanel::GetItem(int X, int Y) {
		int Height = m_Height;
		if (m_HorzScroll.GetVisible()) { Height -= m_HorzScroll.GetHeight(); }

		int y = m_Y + 1;
		if (m_VertScroll.GetVisible()) { y -= m_VertScroll.GetValue(); }
		int Count = 0;
		for (std::vector<Item *>::iterator it = m_Items.begin(); it != m_Items.end(); it++, Count++) {
			Item *pItem = *it;

			// Return the item under the mouse
			if (Y >= y && Y < y + GetItemHeight(pItem)) {
				return pItem;
			}
			y += GetItemHeight(pItem);

			// End of viewable region
			if (y > m_Y + Height) {
				break;
			}
		}
		return nullptr;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIListPanel::GetItemHeight(Item *pItem) {
		int height = 0;

		if (pItem && m_Font) {
			// If the height has already been established earlier, then re-use it
			if (pItem->m_Height > 0) {
				height = pItem->m_Height;
			} else if (m_AlternateDrawMode && pItem->m_pBitmap) {
				// Have to calculate the height if in advanced drawing mode, since the bitmap can be of different heights
				int thirdWidth = m_Width / 3;
				int rightTextWidth = pItem->m_RightText.empty() ? 0 : RIGHTTEXTWIDTH;//thirdWidth / 2;
				int mainTextWidth = (thirdWidth * 2) - rightTextWidth;
				int bitmapWidth = pItem->m_pBitmap ? pItem->m_pBitmap->GetWidth() : 0;
				int bitmapHeight = pItem->m_pBitmap ? pItem->m_pBitmap->GetHeight() : 0;
				if (bitmapWidth > thirdWidth) {
					bitmapHeight *= (float)thirdWidth / (float)bitmapWidth;
					bitmapWidth = thirdWidth;
				}
				int textHeight = m_Font->CalculateHeight(pItem->m_Name, mainTextWidth);
				height = pItem->m_Height = std::max(bitmapHeight + 4, textHeight + 2);
			} else {
				// Non-fancy drawing mode all have same height.
				height = m_Font->GetFontHeight();
			}
		}
		return height;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIListPanel::GetStackHeight(Item *pItem) {
		int height = 0;

		for (std::vector<Item *>::iterator iitr = m_Items.begin(); iitr != m_Items.end(); ++iitr) {
			if ((*iitr) == pItem) {
				break;
			}
			height += GetItemHeight((*iitr));
		}
		return height;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SetItemValues(int Index, Item &item) {
		if (Index >= 0 && Index < m_Items.size()) { *(m_Items.at(Index)) = item; }
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIListPanel::GetSelectedIndex() {
		// Nothing in the list
		if (m_SelectedList.empty()) {
			return -1;
		}
		// Get the first item
		const Item *I = m_SelectedList.at(0);

		return I->m_ID;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SetSelectedIndex(int Index) {
		// Clear the old selection
		std::vector<Item *>::iterator it;
		for (it = m_Items.begin(); it != m_Items.end(); it++) {
			Item *I = *it;
			I->m_Selected = false;
		}
		m_SelectedList.clear();

		// Select the item
		if (Index >= 0 && Index < m_Items.size()) {
			Item *I = m_Items.at(Index);
			I->m_Selected = true;
			m_SelectedList.push_back(I);
			SendSignal(GUIEventCode::Select, 0);

			int stackHeight = GetStackHeight(I);
			int itemHeight = GetItemHeight(I);

			// Adjust the vertical scroll bar to show the newly selected item
			ScrollToItem(I);
		}
		BuildBitmap(false, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::DeleteItem(int Index) {
		if (Index >= 0 && Index < m_Items.size()) {
			const Item *I = m_Items.at(Index);

			// If this item was selected, remove it from the selection list
			if (I->m_Selected) {
				// Find the item
				std::vector<Item *>::iterator it;
				for (it = m_SelectedList.begin(); it != m_SelectedList.end(); it++) {
					if (I->m_ID == (*it)->m_ID) {
						m_SelectedList.erase(it);
						break;
					}
				}
			}

			// Delete and remove it
			delete *(m_Items.begin() + Index);
			m_Items.erase(m_Items.begin() + Index);

			// Reset the id's
			std::vector<Item *>::iterator it;
			int Count = 0;
			for (it = m_Items.begin(); it != m_Items.end(); it++) {
				Item *item = *it;
				item->m_ID = Count++;
			}

			// Adjust the scrollbars
			AdjustScrollbars();

			// Build the bitmap
			BuildBitmap(false, true);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SetSize(int Width, int Height) {
		GUIControl::SetSize(Width, Height);

		// Adjust the scrollbar positions & sizes
		m_HorzScroll.SetPositionAbs(m_X, m_Y + m_Height - 17);
		m_HorzScroll.SetSize(m_Width, 17);
		m_VertScroll.SetPositionAbs(m_X + m_Width - 17, m_Y);
		m_VertScroll.SetSize(17, m_Height);

		// Adjust the scrollbar values
		AdjustScrollbars();

		// Build the bitmap
		BuildBitmap(true, true);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::SetPositionAbs(int X, int Y) {
		GUIControl::SetPositionAbs(X, Y);

		// Adjust the scrollbar positions
		m_HorzScroll.SetPositionAbs(X, Y + m_Height - 17);
		m_VertScroll.SetPositionAbs(X + m_Width - 17, Y);

		// Adjust the scrollbar values
		AdjustScrollbars();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIListPanel::EnableScrollbars(bool Horizontal, bool Vertical) {
		m_HorzScrollEnabled = Horizontal;
		m_VertScrollEnabled = Vertical;

		AdjustScrollbars();
	}
}