#include "GUIEditorApp.h"
#include "GUIEditorUtil.h"

#include "GUIButton.h"
#include "GUICheckbox.h"
#include "AllegroBitmap.h"
#include "RTEError.h"
#include "TimerMan.h"

namespace RTEGUI {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/// <summary>
	/// Quit Handler for Allegro.
	/// </summary>
	void QuitHandler() { g_GUIEditor.OnQuitButton(); }

	/// <summary>
	/// 
	/// </summary>
	void ResizeHandler(RESIZE_DISPLAY_EVENT *resizeInfo) { g_GUIEditor.OnWindowResize(resizeInfo); }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::Clear() {
		m_WindowResized = false;
		m_Quit = false;
		m_ResX = 1024;
		m_ResY = 768;
		m_BackBuffer = nullptr;
		m_ControlManager = nullptr;
		m_EditorManager = nullptr;
		m_PropertyPage = nullptr;
		m_ActiveBoxList = nullptr;
		m_RootControl = nullptr;
		m_EditorBase = nullptr;
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
		set_window_title("Cortex Command: GUI Editor");
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_ResX, m_ResY, 0, 0);
		set_close_button_callback(QuitHandler);
		set_resize_callback(ResizeHandler);

		// Don't want to deal with recreating the backbuffer on window resize so just create one as large as the screen.
		m_BackBuffer = create_bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		clear_to_color(m_BackBuffer, 0);

		m_Screen = std::make_unique<AllegroScreen>(m_BackBuffer);
		m_Input = std::make_unique<AllegroInput>(-1);

		// Initialize the UI

		m_ControlManager = std::make_unique<GUIControlManager>();
		m_ControlManager->Create(m_Screen.get(), m_Input.get(), "Assets/EditorSkin");

		m_EditorManager = std::make_unique<GUIControlManager>();
		m_EditorManager->Create(m_Screen.get(), m_Input.get(), "Assets/EditorSkin");

		m_EditorManager->EnableMouse();

		m_EditorBase.reset(dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("base", "COLLECTIONBOX", nullptr, 0, 0, 1024, 768)));
		m_EditorBase->SetDrawBackground(true);
		m_EditorBase->SetDrawColor(makecol(32, 32, 32));
		m_EditorBase->SetDrawType(GUICollectionBox::Color);

		// Add an area showing the editing box
		GUICollectionBox *editorArea = dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("editArea", "COLLECTIONBOX", m_EditorBase.get(), m_RootOriginX, m_RootOriginY, 640, 480));
		editorArea->SetDrawBackground(true);
		editorArea->SetDrawColor(makecol(64, 64, 64));
		editorArea->SetDrawType(GUICollectionBox::Color);

		// Add the root collection box for the edited document
		GUICollectionBox *Root = dynamic_cast<GUICollectionBox *>(m_ControlManager->AddControl("root", "COLLECTIONBOX", nullptr, m_RootOriginX, m_RootOriginY, 640, 480));
		Root->SetDrawBackground(false);
		m_RootControl.reset(Root);

		// Add the left tool buttons
		GUIButton *Quit = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("btnQuit", "BUTTON", m_EditorBase.get(), 5, 5, 80, 20));
		Quit->SetText("Quit");
		GUIButton *Load = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("btnLoad", "BUTTON", m_EditorBase.get(), 5, 30, 80, 20));
		Load->SetText("Load");
		GUIButton *Add = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("btnAdd", "BUTTON", m_EditorBase.get(), 90, 30, 40, 20));
		Add->SetText("Add");
		GUIButton *Save = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("btnSave", "BUTTON", m_EditorBase.get(), 5, 55, 80, 20));
		Save->SetText("Save");
		GUIButton *SaveAs = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("btnSaveAs", "BUTTON", m_EditorBase.get(), 5, 80, 80, 20));
		SaveAs->SetText("Save As");

		GUIButton *Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_BUTTON", "BUTTON", m_EditorBase.get(), 160, 5, 80, 20));
		Ctrl->SetText("BUTTON");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_CHECKBOX", "BUTTON", m_EditorBase.get(), 160, 30, 80, 20));
		Ctrl->SetText("CHECKBOX");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_COLLECTIONBOX", "BUTTON", m_EditorBase.get(), 160, 55, 80, 20));
		Ctrl->SetText("COLLECTIONBOX");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_COMBOBOX", "BUTTON", m_EditorBase.get(), 160, 80, 80, 20));
		Ctrl->SetText("COMBOBOX");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_LABEL", "BUTTON", m_EditorBase.get(), 250, 5, 80, 20));
		Ctrl->SetText("LABEL");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_LISTBOX", "BUTTON", m_EditorBase.get(), 250, 30, 80, 20));
		Ctrl->SetText("LISTBOX");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_PROGRESSBAR", "BUTTON", m_EditorBase.get(), 250, 55, 80, 20));
		Ctrl->SetText("PROGRESSBAR");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_RADIOBUTTON", "BUTTON", m_EditorBase.get(), 250, 80, 80, 20));
		Ctrl->SetText("RADIOBUTTON");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_SCROLLBAR", "BUTTON", m_EditorBase.get(), 340, 5, 80, 20));
		Ctrl->SetText("SCROLLBAR");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_SLIDER", "BUTTON", m_EditorBase.get(), 340, 30, 80, 20));
		Ctrl->SetText("SLIDER");
		Ctrl = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_TEXTBOX", "BUTTON", m_EditorBase.get(), 340, 55, 80, 20));
		Ctrl->SetText("TEXTBOX");

		m_PropertyPage.reset(dynamic_cast<GUIPropertyPage *>(m_EditorManager->AddControl("props", "PROPERTYPAGE", m_EditorBase.get(), 5, 120, 250, 250)));

		m_ActiveBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorManager->AddControl("active", "LISTBOX", m_EditorBase.get(), 5, 400, 150, 250)));

		GUICheckbox *Snap = dynamic_cast<GUICheckbox *>(m_EditorManager->AddControl("snap", "CHECKBOX", m_EditorBase.get(), 450, 10, 80, 16));
		Snap->SetText("Snap");
		Snap->SetCheck(GUICheckbox::Checked);

		ClearSelection();

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Update() {
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
						std::string Class = (dynamic_cast<GUIButton *>(Event.GetControl()))->GetText();
						GUIControl *Parent = m_RootControl.get();

						// Is the focused control a container?
						if (m_SelectionInfo.Control && m_SelectionInfo.Control->IsContainer()) { Parent = m_SelectionInfo.Control; }

						// Find a suitable control name
						std::string Name = GenerateControlName(Class/*.substr(2)*/);

						if (Parent) { m_ControlManager->AddControl(Name, Class, Parent, 0, 0, -1, -1); }

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
							GUIControl *C = m_SelectionInfo.Control;
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
					if (Event.GetControl()->GetName() == "active" && Event.GetMsg() == GUIListBox::MouseDown) {
						const GUIListPanel::Item *pItem = m_ActiveBoxList->GetSelected();
						if (pItem) {
							// Try to find the box of that name, and select it
							GUIControl *pBoxControl = m_ControlManager->GetControl(pItem->m_Name);
							if (pBoxControl) {
								m_SelectionInfo.GrabbedControl = false;
								m_SelectionInfo.GrabbedHandle = false;
								m_SelectionInfo.Control = pBoxControl;
							}
						}
						// Deselection if clicked on no list item
						else {
							m_SelectionInfo.GrabbedControl = false;
							m_SelectionInfo.GrabbedHandle = false;
							m_SelectionInfo.Control = nullptr;
						}
					}

					// Snap
					if (Event.GetControl()->GetName() == "snap") { m_SnapToGrid = (dynamic_cast<GUICheckbox *>(Event.GetControl()))->GetCheck() == GUICheckbox::Checked; }
					break;

				default:
					break;
			}
		}

		m_EditorManager->Draw();

		m_ControlManager->Draw();

		// Process the editor
		ProcessEditor();

		if (m_SelectionInfo.Control) { DrawSelectedControl(m_SelectionInfo.Control); }

		m_EditorManager->DrawMouse();

		return !m_Quit;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo) {
		m_EditorBase.get()->Resize(resizeInfo->new_w, resizeInfo->new_h);
		m_WindowResized = true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::FlipFrameBuffers() {
		if (m_WindowResized) {
			acknowledge_resize();
			m_WindowResized = false;
		}
		blit(m_BackBuffer, screen, 0, 0, 0, 0, m_BackBuffer->w, m_BackBuffer->h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ClearBackBuffer() const {
		clear_to_color(m_BackBuffer, 0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnQuitButton() {
		if (m_UnsavedChanges) {
			int result = GUIEditorLib::QuitMessageBox("Save changes made?", "Cortex Command: GUI Editor");
			if (result == 1) {
				OnSaveButton();
				m_Quit = true;
			}
			if (result == -1) { m_Quit = true; }
		} else {
			m_Quit = true;
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
		std::array<int, 3> Events;
		std::array<int, 3> States;
		int MouseX;
		int MouseY;
		m_Input->GetMouseButtons(Events.data(), States.data());
		m_Input->GetMousePosition(&MouseX, &MouseY);

		std::array<unsigned char, 256> KeyboardBuffer;
		m_Input->GetKeyboard(KeyboardBuffer.data());

		// Delete key
		if (KeyboardBuffer.at(GUIInput::Key_Delete) == GUIInput::Pushed && !m_PropertyPage->HasTextFocus() && m_SelectionInfo.Control) {
			m_ControlManager->RemoveControl(m_SelectionInfo.Control->GetName(), true);
			m_SelectionInfo.Control = nullptr;
			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;

			m_PropertyPage->ClearValues();
		}

		// Escape key
		if (KeyboardBuffer.at(GUIInput::Key_Escape) == GUIInput::Pushed) {
			// Undo any grab
			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
		}


		// If click released
		if (Events.at(0) == GUIInput::Released) {
			// Move the control after a grab
			if (m_SelectionInfo.GrabbedControl && m_SelectionInfo.TriggerGrab && m_SelectionInfo.Control) {
				// TODO: Check if not moved to another parent
				int DestX = ProcessSnapCoord(MouseX + m_SelectionInfo.GrabX);
				int DestY = ProcessSnapCoord(MouseY + m_SelectionInfo.GrabY);
				m_SelectionInfo.Control->Move(DestX, DestY);

				m_UnsavedChanges = true;
			}

			// Resize/Move control after a grab
			if (m_SelectionInfo.GrabbedHandle && m_SelectionInfo.TriggerGrab && m_SelectionInfo.Control) {
				int X;
				int Y;
				int Width;
				int Height;
				CalculateHandleResize(MouseX, MouseY, &X, &Y, &Width, &Height);

				m_SelectionInfo.Control->Move(X, Y);
				m_SelectionInfo.Control->Resize(Width, Height);

				m_UnsavedChanges = true;
			}

			// Update properties
			if (!ControlUnderMouse(m_PropertyPage.get(), MouseX, MouseY) && m_SelectionInfo.Control) {
				m_SelectionInfo.Control->StoreProperties();

				GUIProperties P;
				P.Update(m_SelectionInfo.Control->GetProperties(), true);
				m_SelectionInfo.Control->GetPanel()->BuildProperties(&P);
				m_PropertyPage->SetPropertyValues(&P);

				m_UnsavedChanges = true;
			}

			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
			m_SelectionInfo.TriggerGrab = false;
		}


		// Check for grabbing handles
		if (!m_SelectionInfo.GrabbedControl && m_SelectionInfo.Control && Events.at(0) == GUIInput::Pushed) {
			int HandleIndex = HandleUnderMouse(m_SelectionInfo.Control, MouseX, MouseY);
			if (HandleIndex != -1) {
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = true;
				m_SelectionInfo.HandleIndex = HandleIndex;

				m_SelectionInfo.GrabX = MouseX;
				m_SelectionInfo.GrabY = MouseY;
				m_SelectionInfo.ClickX = MouseX;
				m_SelectionInfo.ClickY = MouseY;
			}
		}

		// If we've grabbed a control or handle, and we've moved far enough from the starting spot, trigger the grab
		// This prevents quickly selecting a control and slightly moving a couple pixels before releasing
		if ((m_SelectionInfo.GrabbedControl || m_SelectionInfo.GrabbedHandle) && !m_SelectionInfo.TriggerGrab) {
			int MoveDist = 4;

			if (std::fabs(m_SelectionInfo.ClickX - MouseX) >= MoveDist || std::fabs(m_SelectionInfo.ClickY - MouseY) >= MoveDist) { m_SelectionInfo.TriggerGrab = true; }
		}

		// Check if mouse clicked on a control
		if (!m_SelectionInfo.GrabbedControl && !m_SelectionInfo.GrabbedHandle && Events.at(0) == GUIInput::Pushed) {
			GUIControl *C = ControlUnderMouse(m_RootControl.get(), MouseX, MouseY);
			if (C && C != m_RootControl.get()) {
				int X;
				int Y;
				int Width;
				int Height;
				C->GetControlRect(&X, &Y, &Width, &Height);

				m_SelectionInfo.GrabbedHandle = false;

				m_SelectionInfo.GrabbedControl = true;
				m_SelectionInfo.GrabX = X - MouseX;
				m_SelectionInfo.GrabY = Y - MouseY;
				m_SelectionInfo.ClickX = MouseX;
				m_SelectionInfo.ClickY = MouseY;

				m_SelectionInfo.Control = C;

				// Set the properties
				C->StoreProperties();

				GUIProperties P;
				P.Update(C->GetProperties(), true);
				C->GetPanel()->BuildProperties(&P);
				m_PropertyPage->SetPropertyValues(&P);

			} else if (C == m_RootControl.get()) {
				// Unselect control
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = nullptr;

				m_PropertyPage->ClearValues();
			}

			// Update the active box list in case we selected/deselected a top level collection box
			UpdateActiveBoxList();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdateActiveBoxList() const {
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
				if (pBox == m_SelectionInfo.Control) { m_ActiveBoxList->SetSelectedIndex(m_ActiveBoxList->GetItemList()->size() - 1); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIEditorApp::ControlUnderMouse(GUIControl *Parent, int MouseX, int MouseY) {
		assert(Parent);

		// Clicked on the parent?
		int X;
		int Y;
		int Width;
		int Height;
		Parent->GetControlRect(&X, &Y, &Width, &Height);

		if (MouseX < X || MouseX > X + Width) {
			return nullptr;
		}
		if (MouseY < Y || MouseY > Y + Height) {
			return nullptr;
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

	int GUIEditorApp::HandleUnderMouse(GUIControl *Control, int MouseX, int MouseY) const {
		int X;
		int Y;
		int Width;
		int Height;
		Control->GetControlRect(&X, &Y, &Width, &Height);

		int RegionSize = 6;

		int nHandle = 0;

		for (int i = 0; i < 3; i++) {
			if (MouseInsideBox(MouseX, MouseY, X - RegionSize, Y + i * (Height / 2) - RegionSize, RegionSize * 2, RegionSize * 2)) {
				return nHandle;
			}

			nHandle++;
			if (i != 1 && MouseInsideBox(MouseX, MouseY, X + Width / 2 - RegionSize, Y + i * (Height / 2) - RegionSize, RegionSize * 2, RegionSize * 2)) {
				return nHandle;
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
		int HandleSize = 6;

		assert(Control);

		int MouseX;
		int MouseY;
		m_Input->GetMousePosition(&MouseX, &MouseY);

		int X;
		int Y;
		int Width;
		int Height;
		Control->GetControlRect(&X, &Y, &Width, &Height);

		// If we've grabbed the control, draw the selection lines where the mouse is
		if (m_SelectionInfo.GrabbedControl && m_SelectionInfo.TriggerGrab) {
			X = MouseX + m_SelectionInfo.GrabX;
			Y = MouseY + m_SelectionInfo.GrabY;

			X = ProcessSnapCoord(X);
			Y = ProcessSnapCoord(Y);
		}

		// Grabbed handles
		if (m_SelectionInfo.GrabbedHandle && m_SelectionInfo.TriggerGrab) {
			CalculateHandleResize(MouseX, MouseY, &X, &Y, &Width, &Height);
		}


		GUIRect Rect;
		SetRect(&Rect, X - 6, Y - 6, X + Width + 6, Y + Height + 6);
		m_Screen->GetBitmap()->SetClipRect(&Rect);

		m_Screen->GetBitmap()->DrawRectangle(X, Y, Width, Height, 0xFFCCCCCC, false);

		// Draw the handles
		for (int i = 0; i < 3; i++) {
			DrawSelectionHandle(X, Y + i * (Height / 2), HandleSize, HandleSize);
			if (i != 1)
				DrawSelectionHandle(X + Width / 2, Y + i * (Height / 2), HandleSize, HandleSize);

			DrawSelectionHandle(X + Width, Y + i * (Height / 2), HandleSize, HandleSize);
		}

		m_Screen->GetBitmap()->SetClipRect(nullptr);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectionHandle(int xPos, int yPos, int width, int height) const {
		m_Screen->GetBitmap()->DrawRectangle(xPos - width / 2, yPos - height / 2, width, height, 0xFF000000, true);
		m_Screen->GetBitmap()->DrawRectangle(xPos - width / 2, yPos - height / 2, width, height, 0xFFFFFFFF, false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ClearSelection() {
		m_SelectionInfo.GrabbedControl = false;
		m_SelectionInfo.GrabbedHandle = false;
		m_SelectionInfo.TriggerGrab = false;

		m_SelectionInfo.Control = nullptr;
		m_SelectionInfo.HandleIndex = 0;

		m_SelectionInfo.GrabX = 0;
		m_SelectionInfo.GrabY = 0;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::MouseInsideBox(int mouseX, int mouseY, int xPos, int yPos, int width, int height) const {
		return (mouseX >= xPos && mouseX <= xPos + width && mouseY >= yPos && mouseY <= yPos + height) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::CalculateHandleResize(int MouseX, int MouseY, int *X, int *Y, int *Width, int *Height) {
		int CtrlX;
		int CtrlY;
		int CtrlWidth;
		int CtrlHeight;
		m_SelectionInfo.Control->GetControlRect(&CtrlX, &CtrlY, &CtrlWidth, &CtrlHeight);
		GUIControl *Parent = m_SelectionInfo.Control->GetParent();

		int MinSize = 5;

		int ParentX;
		int ParentY;
		int ParentWidth;
		int ParentHeight;
		Parent->GetControlRect(&ParentX, &ParentY, &ParentWidth, &ParentHeight);

		// Left Move/Resize
		if (m_SelectionInfo.HandleIndex == 0 || m_SelectionInfo.HandleIndex == 3 || m_SelectionInfo.HandleIndex == 6) {
			int Diff = MouseX - m_SelectionInfo.GrabX;
			if (CtrlX + Diff < ParentX) { Diff = ParentX - CtrlX; }
			if (CtrlWidth - Diff < MinSize) { Diff = CtrlWidth - MinSize; }

			Diff = ProcessSnapCoord(Diff);

			CtrlX += Diff;
			CtrlWidth -= Diff;
		}
		// Top Move/Resize
		if (m_SelectionInfo.HandleIndex == 0 || m_SelectionInfo.HandleIndex == 1 || m_SelectionInfo.HandleIndex == 2) {
			int Diff = MouseY - m_SelectionInfo.GrabY;
			if (CtrlY + Diff < ParentY) { Diff = ParentY - CtrlY; }
			if (CtrlHeight - Diff < MinSize) { Diff = CtrlHeight - MinSize; }

			Diff = ProcessSnapCoord(Diff);

			CtrlY += Diff;
			CtrlHeight -= Diff;
		}
		// Right Resize
		if (m_SelectionInfo.HandleIndex == 2 || m_SelectionInfo.HandleIndex == 5 || m_SelectionInfo.HandleIndex == 8) {
			int Diff = MouseX - m_SelectionInfo.GrabX;
			if (CtrlX + CtrlWidth + Diff > ParentX + ParentWidth) { Diff = (ParentX + ParentWidth) - (CtrlX + CtrlWidth); }

			Diff = ProcessSnapCoord(Diff);

			CtrlWidth += Diff;
		}
		// Bottom Resize
		if (m_SelectionInfo.HandleIndex == 6 || m_SelectionInfo.HandleIndex == 7 || m_SelectionInfo.HandleIndex == 8) {
			int Diff = MouseY - m_SelectionInfo.GrabY;
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

	std::string GUIEditorApp::GenerateControlName(std::string strControlType) const {
		// 10,000 should be enough
		for (int i = 1; i < 10000; i++) {
			std::string Name = strControlType;
			// Use a lower case version of the string
			std::transform(Name.begin(), Name.end(), Name.begin(), tolower);
			Name.append(std::to_string(i));

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
