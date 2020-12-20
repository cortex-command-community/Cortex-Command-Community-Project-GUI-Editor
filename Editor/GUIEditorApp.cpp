#include "GUIEditorApp.h"
#include "GUIEditorLib.h"

#include "GUICollectionBox.h"
#include "GUIButton.h"
#include "GUICheckbox.h"
#include "AllegroBitmap.h"
#include "RTEError.h"
#include "TimerMan.h"

namespace RTE {

	volatile bool g_Quit;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::Clear() {
		m_ResX = 1024;
		m_ResY = 768;
		m_BackBuffer = nullptr;
		m_ControlManager = nullptr;
		m_EditorManager = nullptr;
		m_PropertyPage = nullptr;
		m_ActiveBoxList = nullptr;
		m_RootControl = nullptr;
		m_Filename.clear();
		m_UnsavedChanges = false;
		m_SnapToGrid = true;
		m_GridSize = 5;
		m_RootOriginX = 300;
		m_RootOriginY = 120;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Initialize() {
		set_color_depth(32);
		set_color_conversion(COLORCONV_MOST);
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_ResX, m_ResY, 0, 0);

		m_BackBuffer = create_bitmap(m_ResX, m_ResY);
		clear_to_color(m_BackBuffer, 0);

		g_Screen = std::make_unique<AllegroScreen>(m_BackBuffer);
		g_Input = std::make_unique<AllegroInput>(-1);

		// Initialize the UI
		m_ControlManager = std::make_unique<GUIControlManager>();
		m_ControlManager->Create(g_Screen.get(), g_Input.get(), "Assets/EditorSkin");

		m_EditorManager = std::make_unique<GUIControlManager>();
		m_EditorManager->Create(g_Screen.get(), g_Input.get(), "Assets/EditorSkin");

		m_EditorManager->EnableMouse();


		GUICollectionBox * g_EdBase = (GUICollectionBox *)m_EditorManager->AddControl("base", "COLLECTIONBOX", nullptr, 0, 0, 1024, 768);
		g_EdBase->SetDrawBackground(true);
		g_EdBase->SetDrawColor(makecol(32, 32, 32));
		g_EdBase->SetDrawType(GUICollectionBox::Color);

		// Add an area showing the editing box
		GUICollectionBox *g_EdArea = (GUICollectionBox *)m_EditorManager->AddControl("editArea", "COLLECTIONBOX", g_EdBase, m_RootOriginX, m_RootOriginY, 640, 480);
		g_EdArea->SetDrawBackground(true);
		g_EdArea->SetDrawColor(makecol(64, 64, 64));
		g_EdArea->SetDrawType(GUICollectionBox::Color);

		// Add the root collection box for the edited document
		GUICollectionBox *Root = (GUICollectionBox *)m_ControlManager->AddControl("root", "COLLECTIONBOX", nullptr, m_RootOriginX, m_RootOriginY, 640, 480);
		Root->SetDrawBackground(false);
		m_RootControl.reset(Root);

		// Add the left tool buttons
		GUIButton *Quit = (GUIButton *)m_EditorManager->AddControl("btnQuit", "BUTTON", g_EdBase, 5, 5, 80, 20);
		Quit->SetText("Quit");
		GUIButton *Load = (GUIButton *)m_EditorManager->AddControl("btnLoad", "BUTTON", g_EdBase, 5, 30, 80, 20);
		Load->SetText("Load");
		GUIButton *Add = (GUIButton *)m_EditorManager->AddControl("btnAdd", "BUTTON", g_EdBase, 90, 30, 40, 20);
		Add->SetText("Add");
		GUIButton *Save = (GUIButton *)m_EditorManager->AddControl("btnSave", "BUTTON", g_EdBase, 5, 55, 80, 20);
		Save->SetText("Save");
		GUIButton *SaveAs = (GUIButton *)m_EditorManager->AddControl("btnSaveAs", "BUTTON", g_EdBase, 5, 80, 80, 20);
		SaveAs->SetText("Save As");

		GUIButton *Ctrl = (GUIButton *)m_EditorManager->AddControl("C_BUTTON", "BUTTON", g_EdBase, 160, 5, 80, 20);
		Ctrl->SetText("BUTTON");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_CHECKBOX", "BUTTON", g_EdBase, 160, 30, 80, 20);
		Ctrl->SetText("CHECKBOX");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_COLLECTIONBOX", "BUTTON", g_EdBase, 160, 55, 80, 20);
		Ctrl->SetText("COLLECTIONBOX");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_COMBOBOX", "BUTTON", g_EdBase, 160, 80, 80, 20);
		Ctrl->SetText("COMBOBOX");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_LABEL", "BUTTON", g_EdBase, 250, 5, 80, 20);
		Ctrl->SetText("LABEL");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_LISTBOX", "BUTTON", g_EdBase, 250, 30, 80, 20);
		Ctrl->SetText("LISTBOX");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_PROGRESSBAR", "BUTTON", g_EdBase, 250, 55, 80, 20);
		Ctrl->SetText("PROGRESSBAR");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_RADIOBUTTON", "BUTTON", g_EdBase, 250, 80, 80, 20);
		Ctrl->SetText("RADIOBUTTON");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_SCROLLBAR", "BUTTON", g_EdBase, 340, 5, 80, 20);
		Ctrl->SetText("SCROLLBAR");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_SLIDER", "BUTTON", g_EdBase, 340, 30, 80, 20);
		Ctrl->SetText("SLIDER");
		Ctrl = (GUIButton *)m_EditorManager->AddControl("C_TEXTBOX", "BUTTON", g_EdBase, 340, 55, 80, 20);
		Ctrl->SetText("TEXTBOX");

		m_PropertyPage.reset((GUIPropertyPage *)m_EditorManager->AddControl("props", "PROPERTYPAGE", g_EdBase, 5, 120, 250, 250));

		m_ActiveBoxList.reset((GUIListBox *)m_EditorManager->AddControl("active", "LISTBOX", g_EdBase, 5, 400, 150, 250));

		GUICheckbox *Snap = (GUICheckbox *)m_EditorManager->AddControl("snap", "CHECKBOX", g_EdBase, 450, 10, 80, 16);
		Snap->SetText("Snap");
		Snap->SetCheck(GUICheckbox::Checked);

		ClearSelection();

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Update() {
		clear_to_color(m_BackBuffer, 0);

		m_EditorManager->Update();

		// Process the editor events
		GUIEvent Event;
		while (m_EditorManager->GetEvent(&Event)) {
			switch (Event.GetType()) {

				// Command
				case GUIEvent::Command:
					// Quit
					if (Event.GetControl()->GetName() == "btnQuit") {
						OnQuitButton();
						break;
					}

					// Load
					if (Event.GetControl()->GetName() == "btnLoad") {
						OnLoadButton();
						break;
					}

					// Add
					if (Event.GetControl()->GetName() == "btnAdd") {
						OnLoadButton(true);
						break;
					}

					// Save
					if (Event.GetControl()->GetName() == "btnSave") {
						OnSaveButton();
						break;
					}

					// Save As
					if (Event.GetControl()->GetName() == "btnSaveAs") {
						OnSaveAsButton();
						break;
					}

					// Add a control
					if (Event.GetControl()->GetName().substr(0, 2).compare("C_") == 0) {
						std::string Class = ((GUIButton *)Event.GetControl())->GetText();
						GUIControl *Parent = m_RootControl.get();

						// Is the focused control a container?
						if (m_SelectionInfo.m_Control) {
							if (m_SelectionInfo.m_Control->IsContainer())
								Parent = m_SelectionInfo.m_Control;
						}

						// Find a suitable control name
						std::string Name = GenerateControlName(Class/*.substr(2)*/);

						if (Parent) {
							m_ControlManager->AddControl(Name, Class, Parent, 0, 0, -1, -1);
						}

						break;
					}

					break;

				// Notification
				case GUIEvent::Notification:

					// Property Page changed
					if (Event.GetControl()->GetName() == "props") {

						if (Event.GetMsg() == GUIPropertyPage::Enter) {
							// Update the focused control properties

							// Apply the properties
							GUIControl *C = m_SelectionInfo.m_Control;
							if (C) {
								C->ApplyProperties(m_PropertyPage->GetPropertyValues());
								// Update the active box list in case the name of a top-level box changed
								UpdateActiveBoxList();
							}

							m_UnsavedChanges = true;
						}

						if (Event.GetMsg() == GUIPropertyPage::Changed) {
							// The properties are dirty and need to be updated
							m_UnsavedChanges = true;
						}
					}

					// Active Box changed
					if (Event.GetControl()->GetName() == "active") {
						// Clicks in the list
						if (Event.GetMsg() == GUIListBox::MouseDown) {
							GUIListPanel::Item *pItem = m_ActiveBoxList->GetSelected();
							if (pItem) {
								// Try to find the box of that name, and select it
								GUIControl *pBoxControl = m_ControlManager->GetControl(pItem->m_Name);
								if (pBoxControl) {
									m_SelectionInfo.m_GrabbedControl = false;
									m_SelectionInfo.m_GrabbedHandle = false;
									m_SelectionInfo.m_Control = pBoxControl;
								}
							}
							// Deselection if clicked on no list item
							else {
								m_SelectionInfo.m_GrabbedControl = false;
								m_SelectionInfo.m_GrabbedHandle = false;
								m_SelectionInfo.m_Control = NULL;
							}
						}
					}


					// Snap
					if (Event.GetControl()->GetName() == "snap") {
						m_SnapToGrid = ((GUICheckbox *)Event.GetControl())->GetCheck() == GUICheckbox::Checked;
					}
					break;
			}
		}

		m_EditorManager->Draw();

		m_ControlManager->Draw();

		// Process the editor
		ProcessEditor();

		if (m_SelectionInfo.m_Control) { DrawSelectedControl(m_SelectionInfo.m_Control); }

		m_EditorManager->DrawMouse();

		blit(m_BackBuffer, screen, 0, 0, 0, 0, m_BackBuffer->w, m_BackBuffer->h);

		return !g_Quit;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnQuitButton() {
		if (m_UnsavedChanges) {
			int result = GUIEditorLib::QuitMessageBox("Save changes made?", "Cortex Command: GUI Editor");
			if (result == 1) {
				OnSaveButton();
				g_Quit = true;
			}
			if (result == -1) { g_Quit = true; }
		} else {
			g_Quit = true;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnLoadButton(bool addControls) {
		std::string strFilename;
		if (GUIEditorLib::DisplayLoadGUIFile(&strFilename)) {
			m_ControlManager->Load(strFilename, addControls);

			GUIControl *C = m_ControlManager->GetControlList()->front();
			m_RootControl.reset(C);

			C->Move(m_RootOriginX, m_RootOriginY);

			C->StoreProperties();

			GUIProperties cProps;
			cProps.Update(C->GetProperties(), true);
			C->GetPanel()->BuildProperties(&cProps);

			// Clear settings
			m_UnsavedChanges = false;
			ClearSelection();
			m_PropertyPage->ClearValues();

			UpdateActiveBoxList();

			m_Filename = strFilename;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnSaveAsButton() {
		std::string strFilename;
		if (GUIEditorLib::DisplaySaveGUIFile(&strFilename)) {
			// Move the root object to the origin before saving
			m_RootControl->Move(0, 0);

			m_ControlManager->Save(strFilename);

			// Move it back
			m_RootControl->Move(m_RootOriginX, m_RootOriginY);

			m_Filename = strFilename;

			m_UnsavedChanges = false;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnSaveButton() {
		if (m_Filename.size() == 0) {
			OnSaveAsButton();
		} else {
			// Move the root object to the origin before saving
			m_RootControl->Move(0, 0);

			m_ControlManager->Save(m_Filename);

			// Move it back
			m_RootControl->Move(m_RootOriginX, m_RootOriginY);

			m_UnsavedChanges = false;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ProcessEditor() {
		int Events[3], States[3];
		int MouseX, MouseY;
		g_Input->GetMouseButtons(Events, States);
		g_Input->GetMousePosition(&MouseX, &MouseY);

		unsigned char KeyboardBuffer[256];
		g_Input->GetKeyboard(KeyboardBuffer);

		// Delete key
		if (KeyboardBuffer[GUIInput::Key_Delete] == GUIInput::Pushed && !m_PropertyPage->HasTextFocus()) {
			// Remove the control
			if (m_SelectionInfo.m_Control) {
				m_ControlManager->RemoveControl(m_SelectionInfo.m_Control->GetName(), true);

				m_SelectionInfo.m_Control = NULL;
				m_SelectionInfo.m_GrabbedControl = false;
				m_SelectionInfo.m_GrabbedHandle = false;

				m_PropertyPage->ClearValues();
			}
		}

		// Escape key
		if (KeyboardBuffer[GUIInput::Key_Escape] == GUIInput::Pushed) {
			// Undo any grab
			m_SelectionInfo.m_GrabbedControl = false;
			m_SelectionInfo.m_GrabbedHandle = false;
		}


		// If click released
		if (Events[0] == GUIInput::Released) {
			// Move the control after a grab
			if (m_SelectionInfo.m_GrabbedControl && m_SelectionInfo.m_TriggerGrab) {
				if (m_SelectionInfo.m_Control) {
					// TODO: Check if not moved to another parent
					int DestX = ProcessSnapCoord(MouseX + m_SelectionInfo.m_GrabX);
					int DestY = ProcessSnapCoord(MouseY + m_SelectionInfo.m_GrabY);
					m_SelectionInfo.m_Control->Move(DestX, DestY);

					m_UnsavedChanges = true;
				}
			}

			// Resize/Move control after a grab
			if (m_SelectionInfo.m_GrabbedHandle && m_SelectionInfo.m_TriggerGrab) {
				if (m_SelectionInfo.m_Control) {
					int X, Y, Width, Height;
					CalculateHandleResize(MouseX, MouseY, &X, &Y, &Width, &Height);

					m_SelectionInfo.m_Control->Move(X, Y);
					m_SelectionInfo.m_Control->Resize(Width, Height);

					m_UnsavedChanges = true;
				}
			}

			// Update properties
			if (!ControlUnderMouse(m_PropertyPage.get(), MouseX, MouseY)) {
				if (m_SelectionInfo.m_Control) {
					m_SelectionInfo.m_Control->StoreProperties();

					GUIProperties P;
					P.Update(m_SelectionInfo.m_Control->GetProperties(), true);
					m_SelectionInfo.m_Control->GetPanel()->BuildProperties(&P);
					m_PropertyPage->SetPropertyValues(&P);

					m_UnsavedChanges = true;
				}
			}

			m_SelectionInfo.m_GrabbedControl = false;
			m_SelectionInfo.m_GrabbedHandle = false;
			m_SelectionInfo.m_TriggerGrab = false;
		}


		// Check for grabbing handles
		if (!m_SelectionInfo.m_GrabbedControl && m_SelectionInfo.m_Control && Events[0] == GUIInput::Pushed) {
			int HandleIndex = HandleUnderMouse(m_SelectionInfo.m_Control, MouseX, MouseY);
			if (HandleIndex != -1) {
				m_SelectionInfo.m_GrabbedControl = false;
				m_SelectionInfo.m_GrabbedHandle = true;
				m_SelectionInfo.m_HandleIndex = HandleIndex;

				m_SelectionInfo.m_GrabX = MouseX;
				m_SelectionInfo.m_GrabY = MouseY;
				m_SelectionInfo.m_ClickX = MouseX;
				m_SelectionInfo.m_ClickY = MouseY;
			}
		}

		// If we've grabbed a control or handle, and we've moved far enough from the starting spot, trigger the grab
		// This prevents quickly selecting a control and slightly moving a couple pixels before releasing
		if ((m_SelectionInfo.m_GrabbedControl || m_SelectionInfo.m_GrabbedHandle) && !m_SelectionInfo.m_TriggerGrab) {
			int MoveDist = 4;

			if (fabsf(m_SelectionInfo.m_ClickX - MouseX) >= MoveDist || fabsf(m_SelectionInfo.m_ClickY - MouseY) >= MoveDist) { m_SelectionInfo.m_TriggerGrab = true; }
		}



		// Check if mouse clicked on a control
		if (!m_SelectionInfo.m_GrabbedControl && !m_SelectionInfo.m_GrabbedHandle && Events[0] == GUIInput::Pushed) {
			GUIControl *C = ControlUnderMouse(m_RootControl.get(), MouseX, MouseY);
			if (C && C != m_RootControl.get()) {
				int X, Y, Width, Height;
				C->GetControlRect(&X, &Y, &Width, &Height);

				m_SelectionInfo.m_GrabbedHandle = false;

				m_SelectionInfo.m_GrabbedControl = true;
				m_SelectionInfo.m_GrabX = X - MouseX;
				m_SelectionInfo.m_GrabY = Y - MouseY;
				m_SelectionInfo.m_ClickX = MouseX;
				m_SelectionInfo.m_ClickY = MouseY;

				m_SelectionInfo.m_Control = C;

				// Set the properties
				C->StoreProperties();

				GUIProperties P;
				P.Update(C->GetProperties(), true);
				C->GetPanel()->BuildProperties(&P);
				m_PropertyPage->SetPropertyValues(&P);

			} else if (C == m_RootControl.get()) {
				// Unselect control
				m_SelectionInfo.m_GrabbedControl = false;
				m_SelectionInfo.m_GrabbedHandle = false;
				m_SelectionInfo.m_Control = nullptr;

				m_PropertyPage->ClearValues();
			}

			// Update the active box list in case we selected/deselected a top level collection box
			UpdateActiveBoxList();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdateActiveBoxList() {
		// Clear the list so we can repopulate it
		m_ActiveBoxList->ClearList();

		// Go through all the top-level (directly under root) controls and add only the ControlBoxs to the list here
		std::vector<GUIControl *> *pControls = m_ControlManager->GetControlList();
		GUICollectionBox *pBox = nullptr;
		for (std::vector<GUIControl *>::iterator itr = pControls->begin(); itr != pControls->end(); itr++) {
			// Look for CollectionBoxes with the root control as parent
			if ((pBox = dynamic_cast<GUICollectionBox *>(*itr)) && pBox->GetParent() == m_RootControl.get()) {
				m_ActiveBoxList->AddItem(pBox->GetName());
				// Check if this is selected in the editor, and if so, select it in the list too
				if (pBox == m_SelectionInfo.m_Control) { m_ActiveBoxList->SetSelectedIndex(m_ActiveBoxList->GetItemList()->size() - 1); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIEditorApp::ControlUnderMouse(GUIControl *Parent, int MouseX, int MouseY) {
		assert(Parent);

		// Clicked on the parent?
		int X, Y, Width, Height;
		Parent->GetControlRect(&X, &Y, &Width, &Height);

		if (MouseX < X || MouseX > X + Width) {
			return NULL;
		}
		if (MouseY < Y || MouseY > Y + Height) {
			return NULL;
		}

		// Check children
		std::vector<GUIControl *> *List = Parent->GetChildren();
		std::vector<GUIControl *>::reverse_iterator it;

		assert(List);

		for (it = List->rbegin(); it != List->rend(); it++) {
			GUIControl *C = ControlUnderMouse(*it, MouseX, MouseY);
			if (C) {
				return C;
			}
		}

		// Return this
		return Parent;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorApp::HandleUnderMouse(GUIControl *Control, int MouseX, int MouseY) {
		int X, Y, Width, Height;
		Control->GetControlRect(&X, &Y, &Width, &Height);

		int RegionSize = 6;

		int nHandle = 0;

		for (int i = 0; i < 3; i++) {
			if (MouseInsideBox(MouseX, MouseY, X - RegionSize, Y + i * (Height / 2) - RegionSize, RegionSize * 2, RegionSize * 2)) {
				return nHandle;
			}

			nHandle++;
			if (i != 1) {
				if (MouseInsideBox(MouseX, MouseY, X + Width / 2 - RegionSize, Y + i * (Height / 2) - RegionSize, RegionSize * 2, RegionSize * 2)) {
					return nHandle;
				}
			}

			nHandle++;

			if (MouseInsideBox(MouseX, MouseY, X + Width - RegionSize, Y + i * (Height / 2) - RegionSize, RegionSize * 2, RegionSize * 2)) {
				return nHandle;
			}

			nHandle++;
		}

		// Not over any handle
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectedControl(GUIControl *Control) {
		int i;
		int HandleSize = 6;

		assert(Control);

		int MouseX, MouseY;
		g_Input->GetMousePosition(&MouseX, &MouseY);

		int X, Y, Width, Height;
		Control->GetControlRect(&X, &Y, &Width, &Height);

		// If we've grabbed the control, draw the selection lines where the mouse is
		if (m_SelectionInfo.m_GrabbedControl && m_SelectionInfo.m_TriggerGrab) {
			X = MouseX + m_SelectionInfo.m_GrabX;
			Y = MouseY + m_SelectionInfo.m_GrabY;

			X = ProcessSnapCoord(X);
			Y = ProcessSnapCoord(Y);
		}

		// Grabbed handles
		if (m_SelectionInfo.m_GrabbedHandle && m_SelectionInfo.m_TriggerGrab) {
			CalculateHandleResize(MouseX, MouseY, &X, &Y, &Width, &Height);
		}


		GUIRect Rect;
		SetRect(&Rect, X - 6, Y - 6, X + Width + 6, Y + Height + 6);
		g_Screen->GetBitmap()->SetClipRect(&Rect);

		g_Screen->GetBitmap()->DrawRectangle(X, Y, Width, Height, 0xFFCCCCCC, false);

		// Draw the handles
		for (int i = 0; i < 3; i++) {
			DrawSelectionHandle(X, Y + i * (Height / 2), HandleSize, HandleSize);
			if (i != 1)
				DrawSelectionHandle(X + Width / 2, Y + i * (Height / 2), HandleSize, HandleSize);

			DrawSelectionHandle(X + Width, Y + i * (Height / 2), HandleSize, HandleSize);
		}

		g_Screen->GetBitmap()->SetClipRect(NULL);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectionHandle(int xPos, int yPos, int width, int height) const {
		g_Screen->GetBitmap()->DrawRectangle(xPos - width / 2, yPos - height / 2, width, height, 0xFF000000, true);
		g_Screen->GetBitmap()->DrawRectangle(xPos - width / 2, yPos - height / 2, width, height, 0xFFFFFFFF, false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ClearSelection() {
		m_SelectionInfo.m_GrabbedControl = false;
		m_SelectionInfo.m_GrabbedHandle = false;
		m_SelectionInfo.m_TriggerGrab = false;

		m_SelectionInfo.m_Control = nullptr;
		m_SelectionInfo.m_HandleIndex = 0;

		m_SelectionInfo.m_GrabX = 0;
		m_SelectionInfo.m_GrabY = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::MouseInsideBox(int mouseX, int mouseY, int xPos, int yPos, int width, int height) const {
		return (mouseX >= xPos && mouseX <= xPos + width && mouseY >= yPos && mouseY <= yPos + height) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::CalculateHandleResize(int MouseX, int MouseY, int *X, int *Y, int *Width, int *Height) {
		int CtrlX, CtrlY, CtrlWidth, CtrlHeight;
		m_SelectionInfo.m_Control->GetControlRect(&CtrlX, &CtrlY, &CtrlWidth, &CtrlHeight);
		GUIControl *Parent = m_SelectionInfo.m_Control->GetParent();

		int MinSize = 5;

		int ParentX, ParentY, ParentWidth, ParentHeight;
		Parent->GetControlRect(&ParentX, &ParentY, &ParentWidth, &ParentHeight);

		// Left Move/Resize
		if (m_SelectionInfo.m_HandleIndex == 0 || m_SelectionInfo.m_HandleIndex == 3 || m_SelectionInfo.m_HandleIndex == 6) {
			int Diff = MouseX - m_SelectionInfo.m_GrabX;
			if (CtrlX + Diff < ParentX) { Diff = ParentX - CtrlX; }
			if (CtrlWidth - Diff < MinSize) { Diff = CtrlWidth - MinSize; }

			Diff = ProcessSnapCoord(Diff);

			CtrlX += Diff;
			CtrlWidth -= Diff;
		}
		// Top Move/Resize
		if (m_SelectionInfo.m_HandleIndex == 0 || m_SelectionInfo.m_HandleIndex == 1 || m_SelectionInfo.m_HandleIndex == 2) {
			int Diff = MouseY - m_SelectionInfo.m_GrabY;
			if (CtrlY + Diff < ParentY) { Diff = ParentY - CtrlY; }
			if (CtrlHeight - Diff < MinSize) { Diff = CtrlHeight - MinSize; }

			Diff = ProcessSnapCoord(Diff);

			CtrlY += Diff;
			CtrlHeight -= Diff;
		}
		// Right Resize
		if (m_SelectionInfo.m_HandleIndex == 2 || m_SelectionInfo.m_HandleIndex == 5 || m_SelectionInfo.m_HandleIndex == 8) {
			int Diff = MouseX - m_SelectionInfo.m_GrabX;
			if (CtrlX + CtrlWidth + Diff > ParentX + ParentWidth) { Diff = (ParentX + ParentWidth) - (CtrlX + CtrlWidth); }

			Diff = ProcessSnapCoord(Diff);

			CtrlWidth += Diff;
		}
		// Bottom Resize
		if (m_SelectionInfo.m_HandleIndex == 6 || m_SelectionInfo.m_HandleIndex == 7 || m_SelectionInfo.m_HandleIndex == 8) {
			int Diff = MouseY - m_SelectionInfo.m_GrabY;
			if (CtrlY + CtrlHeight + Diff > ParentY + ParentHeight) { Diff = (ParentY + ParentHeight) - (CtrlY + CtrlHeight); }

			Diff = ProcessSnapCoord(Diff);

			CtrlHeight += Diff;
		}

		CtrlWidth = std::max(CtrlWidth, MinSize);
		CtrlHeight = std::max(CtrlHeight, MinSize);

		*X = CtrlX;
		*Y = CtrlY;
		*Width = CtrlWidth;
		*Height = CtrlHeight;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIEditorApp::GenerateControlName(std::string strControlType) {
		char buf[64];

		// 10,000 should be enough
		for (int i = 1; i < 10000; i++) {
			std::string Name = strControlType;

			// Use a lower case version of the string
			for (int s = 0; s < Name.size(); s++) {
				Name.at(s) = tolower(Name.at(s));
			}

			Name.append(itoa(i, buf, 10));

			// Check if this name exists
			std::vector<GUIControl *> *ControlList = m_ControlManager->GetControlList();
			std::vector<GUIControl *>::iterator it;

			bool Found = false;

			for (it = ControlList->begin(); it != ControlList->end(); it++) {
				GUIControl *C = *it;
				if (C->GetName().compare(Name) == 0) {
					Found = true;
					break;
				}
			}

			if (!Found) {
				return Name;
			}
		}

		// Error: Tried all 10,000 numbers
		return strControlType;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorApp::ProcessSnapCoord(int position) const {
		if (m_SnapToGrid) {
			float fraction = static_cast<float>(position) / static_cast<float>(m_GridSize);
			float value = std::floor(fraction);
			fraction -= value;
			fraction = (fraction >= 0.5F) ? 1 : 0;
			position = static_cast<int>((value + fraction)) * m_GridSize;
		}
		return position;
	}
}