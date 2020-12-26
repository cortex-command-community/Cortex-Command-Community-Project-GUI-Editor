#include "GUIEditorApp.h"
#include "GUIEditorUtil.h"
#include "GUIButton.h"
#include "GUICheckbox.h"
#include "GUILabel.h"
#include "GUITextBox.h"
#include "allegro.h"
#include "winalleg.h"

namespace RTEGUI {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Initialize() {
		set_color_depth(32);
		set_color_conversion(COLORCONV_MOST);
		set_window_title("Cortex Command GUI Editor");
		set_gfx_mode(GFX_AUTODETECT_WINDOWED, m_ResX, m_ResY, 0, 0);
		set_close_button_callback(QuitHandler);
		set_resize_callback(ResizeHandler);
		set_display_switch_callback(SWITCH_OUT, SwitchOutHandler);
		set_display_switch_callback(SWITCH_IN, SwitchInHandler);

		// Don't want to deal with recreating the backbuffer on window resize so just create one as large as the screen.
		m_BackBuffer = create_bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		clear_to_color(m_BackBuffer, 0);

		m_Screen = std::make_unique<AllegroScreen>(m_BackBuffer);
		m_Input = std::make_unique<AllegroInput>(-1);

		// Initialize the UI
		m_ControlManager = std::make_unique<GUIControlManager>();
		m_ControlManager->Create(m_Screen.get(), m_Input.get(), "Assets", "EditorSkin.ini");

		m_EditorManager = std::make_unique<GUIControlManager>();
		m_EditorManager->Create(m_Screen.get(), m_Input.get(), "Assets", "EditorSkin.ini");
		m_EditorManager->EnableMouse();

		CreateEditorLayout();

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::CreateEditorLayout() {
		m_EditorBase.reset(dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("EditorBase", "COLLECTIONBOX", nullptr, 0, 0, m_ResX, m_ResY)));
		m_EditorBase->SetDrawBackground(true);
		m_EditorBase->SetDrawColor(makecol(32, 32, 32));
		m_EditorBase->SetDrawType(GUICollectionBox::Color);

		m_LeftColumn.reset(dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("LeftColumn", "COLLECTIONBOX", nullptr, 0, 0, 290, m_ResY)));
		m_LeftColumn->SetDrawBackground(true);
		m_LeftColumn->SetDrawColor(makecol(23, 23, 23));
		m_LeftColumn->SetDrawType(GUICollectionBox::Color);

		GUICollectionBox *filePanel = dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("FilePanel", "COLLECTIONBOX", m_LeftColumn.get(), 5, 5, 270, 55));
		filePanel->SetDrawType(GUICollectionBox::Panel);

		GUIButton *toolboxButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("LoadButton", "BUTTON", filePanel, 5, 5, 85, 20));
		toolboxButton->SetText("Load");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("AddButton", "BUTTON", filePanel, 5, 30, 85, 20));
		toolboxButton->SetText("Add File");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("SaveButton", "BUTTON", filePanel, 95, 5, 85, 20));
		toolboxButton->SetText("Save");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("SaveAsButton", "BUTTON", filePanel, 95, 30, 85, 20));
		toolboxButton->SetText("Save As");
		toolboxButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("QuitButton", "BUTTON", filePanel, 185, 5, 80, 20));
		toolboxButton->SetText("Quit");

		GUICollectionBox *editorControls = dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("EditorControlsPanel", "COLLECTIONBOX", m_LeftColumn.get(), filePanel->GetRelXPos(), filePanel->GetRelYPos() + 65, 270, 155));
		editorControls->SetDrawType(GUICollectionBox::Panel);

		GUICollectionBox *elementPanel = dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("NewElementPanel", "COLLECTIONBOX", editorControls, 5, 25, 260, 105));
		elementPanel->SetDrawType(GUICollectionBox::Panel);

		GUIButton *elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_COLLECTIONBOX", "BUTTON", elementPanel, 5, 5, 80, 20));
		elementButton->SetText("CollectionBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_LISTBOX", "BUTTON", elementPanel, 5, 30, 80, 20));
		elementButton->SetText("ListBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_BUTTON", "BUTTON", elementPanel, 5, 55, 80, 20));
		elementButton->SetText("Button");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_COMBOBOX", "BUTTON", elementPanel, 5, 80, 80, 20));
		elementButton->SetText("ComboBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_LABEL", "BUTTON", elementPanel, 90, 5, 80, 20));
		elementButton->SetText("Label");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_CHECKBOX", "BUTTON", elementPanel, 90, 30, 80, 20));
		elementButton->SetText("CheckBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_TEXTBOX", "BUTTON", elementPanel, 90, 55, 80, 20));
		elementButton->SetText("TextBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_RADIOBUTTON", "BUTTON", elementPanel, 90, 80, 80, 20));
		elementButton->SetText("RadioButton");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_SCROLLBAR", "BUTTON", elementPanel, 175, 5, 80, 20));
		elementButton->SetText("ScrollBar");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_SLIDER", "BUTTON", elementPanel, 175, 30, 80, 20));
		elementButton->SetText("Slider");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_PROGRESSBAR", "BUTTON", elementPanel, 175, 55, 80, 20));
		elementButton->SetText("ProgressBar");
		elementButton = dynamic_cast<GUIButton *>(m_EditorManager->AddControl("C_TAB", "BUTTON", elementPanel, 175, 80, 80, 20));
		elementButton->SetText("Tab");

		GUILabel *newElementLabel = dynamic_cast<GUILabel *>(m_EditorManager->AddControl("NewElementLabel", "LABEL", editorControls, elementPanel->GetRelXPos() + 5, elementPanel->GetRelYPos() - 20, 100, 20));
		newElementLabel->SetText("Add New Element :");

		GUILabel *gridSizeLabel = dynamic_cast<GUILabel *>(m_EditorManager->AddControl("GridSizeLabel", "LABEL", editorControls, 10, 135, 75, 15));
		gridSizeLabel->SetText("Grid Size :");
		GUITextBox *gridSizeTextbox = dynamic_cast<GUITextBox *>(m_EditorManager->AddControl("GridSizeTextBox", "TEXTBOX", editorControls, gridSizeLabel->GetRelXPos() + 55, gridSizeLabel->GetRelYPos(), 30, 15));
		gridSizeTextbox->SetText(std::to_string(m_GridSize));

		GUICheckbox *snapCheckbox = dynamic_cast<GUICheckbox *>(m_EditorManager->AddControl("SnapCheckBox", "CHECKBOX", editorControls, gridSizeLabel->GetRelXPos() + 110, gridSizeLabel->GetRelYPos(), 75, 15));
		snapCheckbox->SetText("Snap to Grid");
		snapCheckbox->SetCheck(GUICheckbox::Checked);

		// Only allow workspace zoom if the screen resolution is FHD or above, smaller resolutions can't fully display it
		if (m_BackBuffer->w >= 1920 && m_BackBuffer->h >= 1080) {
			GUICheckbox *zoomCheckBox = dynamic_cast<GUICheckbox *>(m_EditorManager->AddControl("ZoomCheckBox", "CHECKBOX", editorControls, snapCheckbox->GetRelXPos() + 100, snapCheckbox->GetRelYPos(), 75, 15));
			zoomCheckBox->SetText("Zoom");
			zoomCheckBox->SetCheck(GUICheckbox::Unchecked);
		}

		m_PropertyPage.reset(dynamic_cast<GUIPropertyPage *>(m_EditorManager->AddControl("PropertyPage", "PROPERTYPAGE", m_LeftColumn.get(), editorControls->GetRelXPos(), editorControls->GetRelYPos() + 165, 270, 360)));

		m_RightColumn.reset(dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("RightColumn", "COLLECTIONBOX", nullptr, m_ResX - 290, 0, 290, m_ResY)));
		m_RightColumn->SetDrawBackground(true);
		m_RightColumn->SetDrawColor(makecol(23, 23, 23));
		m_RightColumn->SetDrawType(GUICollectionBox::Color);

		m_ActiveCollectionBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorManager->AddControl("ActiveCollectionBoxes", "LISTBOX", m_RightColumn.get(), 15, 5, 270, 200)));
		m_ControlsInActiveCollectionBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorManager->AddControl("ControlsInActiveCollectionBox", "LISTBOX", m_RightColumn.get(), 15, m_ActiveCollectionBoxList->GetRelYPos() + 210, 270, 380)));

		// Add an area showing the editing box
		GUICollectionBox *workspace = dynamic_cast<GUICollectionBox *>(m_EditorManager->AddControl("Workspace", "COLLECTIONBOX", m_EditorBase.get(), m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight));
		workspace->SetDrawBackground(true);
		workspace->SetDrawColor(makecol(64, 64, 64));
		workspace->SetDrawType(GUICollectionBox::Color);

		// Add the root collection box for the edited document
		GUICollectionBox *rootBox = dynamic_cast<GUICollectionBox *>(m_ControlManager->AddControl("root", "COLLECTIONBOX", nullptr, m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight));
		rootBox->SetDrawBackground(false);
		m_RootControl = rootBox;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdateGridSize(GUIEvent &editorEvent) {
		std::string newValue = dynamic_cast<GUITextBox *>(editorEvent.GetControl())->GetText();
		if (newValue.empty()) { newValue = "1"; }

		bool validEntry = true;
		for (const char &stringChar : newValue) {
			if (!std::isdigit(stringChar)) {
				validEntry = false;
				break;
			}
		}
		m_GridSize = validEntry ? std::clamp(std::stoi(newValue), 0, 255) : m_GridSize;
		dynamic_cast<GUITextBox *>(editorEvent.GetControl())->SetText(std::to_string(m_GridSize));
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdatePropertyPage(GUIEvent &editorEvent) {
		if (editorEvent.GetMsg() == GUIPropertyPage::Enter) {
			// Update the focused control properties
			GUIControl *control = m_SelectionInfo.Control;
			if (control) { control->ApplyProperties(m_PropertyPage->GetPropertyValues()); }
			m_UnsavedChanges = true;
		}
		if (editorEvent.GetMsg() == GUIPropertyPage::Changed) {
			// The properties are dirty and need to be updated
			m_UnsavedChanges = true;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdateControlProperties(GUIControl *control, bool setUnsavedChanges) {
		control->StoreProperties();

		GUIProperties properties;
		properties.Update(control->GetProperties(), true);
		control->GetPanel()->BuildProperties(&properties);
		m_PropertyPage->SetPropertyValues(&properties);

		if (setUnsavedChanges) { m_UnsavedChanges = true; }
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::UpdateActiveBoxList() {
		const GUIListPanel::Item *item = m_ActiveCollectionBoxList->GetSelected();
		if (item) {
			// Try to find the box of that name, and select it
			GUIControl *boxControl = m_ControlManager->GetControl(item->m_Name);
			if (boxControl) {
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = boxControl;
			}
		} else {
			// Deselection if clicked on no list item
			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
			m_SelectionInfo.Control = nullptr;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::PopulateActiveBoxList() const {
		m_ActiveCollectionBoxList->ClearList();
		GUICollectionBox *collectionBox = nullptr;

		// Go through all the top-level (directly under root) controls and add only the CollectionBoxes to the list here
		for (GUIControl *control : *m_ControlManager->GetControlList()) {
			// Look for CollectionBoxes with the root control as parent
			if ((collectionBox = dynamic_cast<GUICollectionBox *>(control)) && collectionBox->GetParent() == m_RootControl) {
				m_ActiveCollectionBoxList->AddItem(collectionBox->GetName());
				// Check if this is selected in the editor, and if so, select it in the list too
				if (collectionBox == m_SelectionInfo.Control) { m_ActiveCollectionBoxList->SetSelectedIndex(m_ActiveCollectionBoxList->GetItemList()->size() - 1); }
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIEditorApp::ControlUnderMouse(GUIControl *parent, int mousePosX, int mousePosY) {
		assert(parent);

		// Clicked on the parent?
		int xPos;
		int yPos;
		int width;
		int height;
		parent->GetControlRect(&xPos, &yPos, &width, &height);

		if (mousePosX < xPos || mousePosX > xPos + width) {
			return nullptr;
		}
		if (mousePosY < yPos || mousePosY > yPos + height) {
			return nullptr;
		}

		// Check children
		std::vector<GUIControl *> *list = parent->GetChildren();
		std::vector<GUIControl *>::reverse_iterator it;

		assert(list);

		for (it = list->rbegin(); it != list->rend(); it++) {
			GUIControl *control = ControlUnderMouse(*it, mousePosX, mousePosY);
			if (control) {
				return control;
			}
		}

		// Return this
		return parent;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorApp::HandleUnderMouse(GUIControl *control, int mousePosX, int mousePosY) const {
		int xPos;
		int yPos;
		int width;
		int height;
		control->GetControlRect(&xPos, &yPos, &width, &height);

		int regionSize = 6;
		int handle = 0;

		for (int i = 0; i < 3; i++) {
			if (MouseInsideBox(mousePosX, mousePosY, xPos - regionSize, yPos + i * (height / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (i != 1 && MouseInsideBox(mousePosX, mousePosY, xPos + width / 2 - regionSize, yPos + i * (height / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (MouseInsideBox(mousePosX, mousePosY, xPos + width - regionSize, yPos + i * (height / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;
		}

		// Not over any handle
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::MouseInsideBox(int mousePosX, int mousePosY, int xPos, int yPos, int width, int height) const {
		return (mousePosX >= xPos && mousePosX <= xPos + width && mousePosY >= yPos && mousePosY <= yPos + height) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::CalculateHandleResize(int mousePosX, int mousePosY, int *xPos, int *yPos, int *width, int *height) {
		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		m_SelectionInfo.Control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);
		GUIControl *parent = m_SelectionInfo.Control->GetParent();

		int minSize = 5;

		int parentPosX;
		int parentPosY;
		int parentWidth;
		int parentHeight;
		parent->GetControlRect(&parentPosX, &parentPosY, &parentWidth, &parentHeight);

		// Left Move/Resize
		if (m_SelectionInfo.HandleIndex == 0 || m_SelectionInfo.HandleIndex == 3 || m_SelectionInfo.HandleIndex == 6) {
			int diff = mousePosX - m_SelectionInfo.GrabX;
			if (controlPosX + diff < parentPosX) { diff = parentPosX - controlPosX; }
			if (controlWidth - diff < minSize) { diff = controlWidth - minSize; }

			diff = ProcessSnapCoord(diff);

			controlPosX += diff;
			controlWidth -= diff;
		}
		// Top Move/Resize
		if (m_SelectionInfo.HandleIndex == 0 || m_SelectionInfo.HandleIndex == 1 || m_SelectionInfo.HandleIndex == 2) {
			int diff = mousePosY - m_SelectionInfo.GrabY;
			if (controlPosY + diff < parentPosY) { diff = parentPosY - controlPosY; }
			if (controlHeight - diff < minSize) { diff = controlHeight - minSize; }

			diff = ProcessSnapCoord(diff);

			controlPosY += diff;
			controlHeight -= diff;
		}
		// Right Resize
		if (m_SelectionInfo.HandleIndex == 2 || m_SelectionInfo.HandleIndex == 5 || m_SelectionInfo.HandleIndex == 8) {
			int diff = mousePosX - m_SelectionInfo.GrabX;
			if (controlPosX + controlWidth + diff > parentPosX + parentWidth) { diff = (parentPosX + parentWidth) - (controlPosX + controlWidth); }

			diff = ProcessSnapCoord(diff);

			controlWidth += diff;
		}
		// Bottom Resize
		if (m_SelectionInfo.HandleIndex == 6 || m_SelectionInfo.HandleIndex == 7 || m_SelectionInfo.HandleIndex == 8) {
			int diff = mousePosY - m_SelectionInfo.GrabY;
			if (controlPosY + controlHeight + diff > parentPosY + parentHeight) { diff = (parentPosY + parentHeight) - (controlPosY + controlHeight); }

			diff = ProcessSnapCoord(diff);

			controlHeight += diff;
		}

		controlWidth = std::max(controlWidth, minSize);
		controlHeight = std::max(controlHeight, minSize);

		*xPos = controlPosX;
		*yPos = controlPosY;
		*width = controlWidth;
		*height = controlHeight;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int GUIEditorApp::ProcessSnapCoord(int position) const {
		if (m_SnapToGrid) {
			float unsnappedPosition = std::round(static_cast<float>(position) / static_cast<float>(m_GridSize));
			position = static_cast<int>(unsnappedPosition) * m_GridSize;
		}
		return position;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::AddNewControl(GUIEvent &editorEvent) {
		std::string controlClass = editorEvent.GetControl()->GetName().substr(2, std::string::npos);
		GUIControl *parent = m_RootControl;

		// Is the focused control a container?
		if (m_SelectionInfo.Control && m_SelectionInfo.Control->IsContainer()) { parent = m_SelectionInfo.Control; }

		// Find a suitable control name
		std::string name = GenerateControlName(controlClass);

		if (parent) { m_ControlManager->AddControl(name, controlClass, parent, 0, 0, -1, -1); }

		PopulateActiveBoxList();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string GUIEditorApp::GenerateControlName(std::string controlType) const {
		for (int i = 1; i < 1000; i++) {
			std::string controlName = controlType;
			std::transform(controlName.begin(), controlName.end(), controlName.begin(), tolower);
			controlName.append(std::to_string(i));

			// Check if this name exists
			bool found = false;
			for (GUIControl *control : *m_ControlManager->GetControlList()) {
				if (control->GetName() == controlName) {
					found = true;
					break;
				}
			}
			if (!found) {
				return controlName;
			}
		}
		return controlType;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool GUIEditorApp::Update() {
		m_EditorManager->Update();
		GUIEvent editorEvent;
		while (m_EditorManager->GetEvent(&editorEvent)) {
			switch (editorEvent.GetType()) {
				case GUIEvent::Command:
					if (editorEvent.GetControl()->GetName() == "QuitButton") {
						OnQuitButton();
					} else if (editorEvent.GetControl()->GetName() == "LoadButton") {
						OnLoadButton();
					} else if (editorEvent.GetControl()->GetName() == "AddButton") {
						OnLoadButton(true);
					} else if (editorEvent.GetControl()->GetName() == "SaveButton") {
						OnSaveButton();
					} else if (editorEvent.GetControl()->GetName() == "SaveAsButton") {
						OnSaveButton(true);
					} else if (editorEvent.GetControl()->GetName().substr(0, 2).compare("C_") == 0) {
						AddNewControl(editorEvent);
					}
					break;
				case GUIEvent::Notification:
					if (editorEvent.GetControl()->GetName() == "ActiveCollectionBoxes" && editorEvent.GetMsg() == GUIListBox::MouseDown) {
						UpdateActiveBoxList();
					} else if (editorEvent.GetControl()->GetName() == "PropertyPage") {
						UpdatePropertyPage(editorEvent);
					} else if (editorEvent.GetControl()->GetName() == "GridSizeTextBox" && editorEvent.GetMsg() == GUITextBox::Enter) {
						UpdateGridSize(editorEvent);
					} else if (editorEvent.GetControl()->GetName() == "SnapCheckBox") {
						m_SnapToGrid = (dynamic_cast<GUICheckbox *>(editorEvent.GetControl()))->GetCheck() == GUICheckbox::Checked;
					} else if (editorEvent.GetControl()->GetName() == "ZoomCheckBox") {
						m_Zoom = (dynamic_cast<GUICheckbox *>(editorEvent.GetControl()))->GetCheck() == GUICheckbox::Checked;
					}
					break;
				default:
					break;
			}
		}
		ProcessMouseInput();
		ProcessKeyboardInput();

		return !m_Quit;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ProcessMouseInput() {
		std::array<int, 3> mouseButtons;
		std::array<int, 3> mouseStates;
		int mousePosX;
		int mousePosY;
		m_Input->GetMouseButtons(mouseButtons.data(), mouseStates.data());
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		// If click released
		if (mouseButtons.at(0) == GUIInput::Released) {
			// Move the control after a grab
			if (m_SelectionInfo.GrabbedControl && m_SelectionInfo.TriggerGrab && m_SelectionInfo.Control) {
				// TODO: Check if not moved to another parent
				int destX = ProcessSnapCoord(mousePosX + m_SelectionInfo.GrabX);
				int destY = ProcessSnapCoord(mousePosY + m_SelectionInfo.GrabY);
				m_SelectionInfo.Control->Move(destX, destY);

				m_UnsavedChanges = true;
			}

			// Resize/Move control after a grab
			if (m_SelectionInfo.GrabbedHandle && m_SelectionInfo.TriggerGrab && m_SelectionInfo.Control) {
				int xPos;
				int yPos;
				int width;
				int height;
				CalculateHandleResize(mousePosX, mousePosY, &xPos, &yPos, &width, &height);

				m_SelectionInfo.Control->Move(xPos, yPos);
				m_SelectionInfo.Control->Resize(width, height);

				m_UnsavedChanges = true;
			}

			// Update properties
			if (!ControlUnderMouse(m_PropertyPage.get(), mousePosX, mousePosY) && m_SelectionInfo.Control) { UpdateControlProperties(m_SelectionInfo.Control); }

			m_SelectionInfo.GrabbedControl = false;
			m_SelectionInfo.GrabbedHandle = false;
			m_SelectionInfo.TriggerGrab = false;
		}


		// Check for grabbing handles
		if (!m_SelectionInfo.GrabbedControl && m_SelectionInfo.Control && mouseButtons.at(0) == GUIInput::Pushed) {
			int HandleIndex = HandleUnderMouse(m_SelectionInfo.Control, mousePosX, mousePosY);
			if (HandleIndex != -1) {
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = true;
				m_SelectionInfo.HandleIndex = HandleIndex;

				m_SelectionInfo.GrabX = mousePosX;
				m_SelectionInfo.GrabY = mousePosY;
				m_SelectionInfo.ClickX = mousePosX;
				m_SelectionInfo.ClickY = mousePosY;
			}
		}

		// If we've grabbed a control or handle, and we've moved far enough from the starting spot, trigger the grab
		// This prevents quickly selecting a control and slightly moving a couple pixels before releasing
		if ((m_SelectionInfo.GrabbedControl || m_SelectionInfo.GrabbedHandle) && !m_SelectionInfo.TriggerGrab) {
			int moveDist = 4;
			if (std::fabs(m_SelectionInfo.ClickX - mousePosX) >= moveDist || std::fabs(m_SelectionInfo.ClickY - mousePosY) >= moveDist) { m_SelectionInfo.TriggerGrab = true; }
		}

		// Check if mouse clicked on a control
		if (!m_SelectionInfo.GrabbedControl && !m_SelectionInfo.GrabbedHandle && mouseButtons.at(0) == GUIInput::Pushed) {
			GUIControl *control = ControlUnderMouse(m_RootControl, mousePosX, mousePosY);
			if (control && control != m_RootControl) {
				int xPos;
				int yPos;
				int width;
				int height;
				control->GetControlRect(&xPos, &yPos, &width, &height);

				m_SelectionInfo.GrabbedControl = true;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = control;

				m_SelectionInfo.GrabX = xPos - mousePosX;
				m_SelectionInfo.GrabY = yPos - mousePosY;
				m_SelectionInfo.ClickX = mousePosX;
				m_SelectionInfo.ClickY = mousePosY;

				UpdateControlProperties(m_SelectionInfo.Control, false);
			} else if (control == m_RootControl) {
				// Unselect control
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_SelectionInfo.Control = nullptr;

				m_PropertyPage->ClearValues();
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::ProcessKeyboardInput() {
		// Handle keyboard input directly from Allegro instead of through AllegroInput to make life easier.
		for (int i = 0; i < KEY_MAX; ++i) {
			m_KeyStates.at(i) = key[i];
		}
		int pressed = -1;

		bool modCtrl = m_KeyStates.at(KEY_LCONTROL) == pressed || m_KeyStates.at(KEY_RCONTROL) == pressed;
		bool modShift = m_KeyStates.at(KEY_LSHIFT) == pressed || m_KeyStates.at(KEY_RSHIFT) == pressed;

		if (!m_PropertyPage->HasTextFocus() && m_SelectionInfo.Control) {
			if (m_KeyStates.at(KEY_DEL) == pressed) {
				m_ControlManager->RemoveControl(m_SelectionInfo.Control->GetName(), true);
				m_SelectionInfo.Control = nullptr;
				m_SelectionInfo.GrabbedControl = false;
				m_SelectionInfo.GrabbedHandle = false;
				m_PropertyPage->ClearValues();
			} else {
				const GUIPanel *selectedElement = dynamic_cast<GUIPanel *>(m_SelectionInfo.Control);

				int nudgeSize = modShift ? 1 : m_GridSize;

				if (m_KeyStates.at(KEY_UP) == pressed && m_PrevKeyStates.at(KEY_UP) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos(), selectedElement->GetYPos() - nudgeSize);
					UpdateControlProperties(m_SelectionInfo.Control);
				} else if (m_KeyStates.at(KEY_DOWN) == pressed && m_PrevKeyStates.at(KEY_DOWN) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos(), selectedElement->GetYPos() + nudgeSize);
					UpdateControlProperties(m_SelectionInfo.Control);
				} else if (m_KeyStates.at(KEY_LEFT) == pressed && m_PrevKeyStates.at(KEY_LEFT) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos() - nudgeSize, selectedElement->GetYPos());
					UpdateControlProperties(m_SelectionInfo.Control);
				} else if (m_KeyStates.at(KEY_RIGHT) == pressed && m_PrevKeyStates.at(KEY_RIGHT) != pressed) {
					m_SelectionInfo.Control->Move(selectedElement->GetXPos() + nudgeSize, selectedElement->GetYPos());
					UpdateControlProperties(m_SelectionInfo.Control);
				}
			}
		}

		// Escape key - Undo any grab
		if (m_KeyStates.at(KEY_ESC) == pressed) {
			m_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
		}

		if (modCtrl) {
			if (m_KeyStates.at(KEY_S) == pressed) {
				OnSaveButton(modShift ? true : false);
			} else if (m_KeyStates.at(KEY_O) == pressed) {
				OnLoadButton(modShift ? true : false);
			}
		}

		m_PrevKeyStates = m_KeyStates;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectionBox(GUIControl *control) {
		RTEAssert(control, "Trying to draw selection box for a null control!");

		int mousePosX;
		int mousePosY;
		m_Input->GetMousePosition(&mousePosX, &mousePosY);

		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);

		// If we've grabbed the control, draw the selection lines where the mouse is
		if (m_SelectionInfo.GrabbedControl && m_SelectionInfo.TriggerGrab) {
			controlPosX = mousePosX + m_SelectionInfo.GrabX;
			controlPosY = mousePosY + m_SelectionInfo.GrabY;

			controlPosX = ProcessSnapCoord(controlPosX);
			controlPosY = ProcessSnapCoord(controlPosY);
		}

		// Grabbed handles
		if (m_SelectionInfo.GrabbedHandle && m_SelectionInfo.TriggerGrab) { CalculateHandleResize(mousePosX, mousePosY, &controlPosX, &controlPosY, &controlWidth, &controlHeight); }

		GUIRect rect;
		SetRect(&rect, controlPosX - 6, controlPosY - 6, controlPosX + controlWidth + 6, controlPosY + controlHeight + 6);
		m_Screen->GetBitmap()->SetClipRect(&rect);

		m_Screen->GetBitmap()->DrawRectangle(controlPosX, controlPosY, controlWidth, controlHeight, 0xFFCCCCCC, false);

		// Draw the handles
		for (int i = 0; i < 3; i++) {
			DrawSelectionResizeBox(controlPosX, controlPosY + i * (controlHeight / 2));
			if (i != 1) { DrawSelectionResizeBox(controlPosX + controlWidth / 2, controlPosY + i * (controlHeight / 2)); }
			DrawSelectionResizeBox(controlPosX + controlWidth, controlPosY + i * (controlHeight / 2));
		}
		m_Screen->GetBitmap()->SetClipRect(nullptr);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawSelectionResizeBox(int xPos, int yPos) const {
		int boxSize = (m_Zoom) ? 5 : 7;
		m_Screen->GetBitmap()->DrawRectangle(xPos - boxSize / 2, yPos - boxSize / 2, boxSize, boxSize, 0x000000, true);
		m_Screen->GetBitmap()->DrawRectangle(xPos - boxSize / 2, yPos - boxSize / 2, boxSize, boxSize, 0xFFFFFF, false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::DrawEditor() {
		if (m_WindowResized) {
			acknowledge_resize();
			m_WindowResized = false;
		}
		clear_to_color(m_BackBuffer, 0);

		m_EditorBase->Draw(m_Screen.get());
		m_ControlManager->Draw();
		if (m_SelectionInfo.Control) { DrawSelectionBox(m_SelectionInfo.Control); }
		m_LeftColumn->Draw(m_Screen.get());
		m_RightColumn->Draw(m_Screen.get());
		m_EditorManager->DrawMouse();

		if (m_Zoom) {
			BITMAP *tempBackbuffer = create_bitmap(m_WorkspaceWidth * 2, m_WorkspaceHeight * 2);
			stretch_blit(m_BackBuffer, tempBackbuffer, m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight, 0, 0, m_WorkspaceWidth * 2, m_WorkspaceHeight * 2);
			blit(tempBackbuffer, m_BackBuffer, 0, 0, m_WorkspacePosX, m_WorkspacePosY - 30, m_WorkspaceWidth * 2, m_WorkspaceHeight * 2);
			destroy_bitmap(tempBackbuffer);
		}
		blit(m_BackBuffer, screen, 0, 0, 0, 0, screen->w, screen->h);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnLoadButton(bool addControls) {
		std::string newFilename;
		if (GUIEditorUtil::DisplayLoadGUIFile(&newFilename, win_get_window())) {
			m_ControlManager->Load(newFilename, addControls);
			m_Filename = newFilename;

			GUIControl *newRootControl = m_ControlManager->GetControlList()->front();
			m_RootControl = newRootControl;
			newRootControl->Move(m_WorkspacePosX, m_WorkspacePosY);
			newRootControl->StoreProperties();

			GUIProperties newRootControlProps;
			newRootControlProps.Update(newRootControl->GetProperties(), true);
			newRootControl->GetPanel()->BuildProperties(&newRootControlProps);

			m_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
			m_UnsavedChanges = false;

			PopulateActiveBoxList();
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnSaveButton(bool saveAsNewFile) {
		if (saveAsNewFile || m_Filename.empty()) {
			std::string newFilename;
			if (GUIEditorUtil::DisplaySaveGUIFile(&newFilename, win_get_window())) { m_Filename = newFilename; }
		}
		// Move the root object to top left corner before saving so it is displayed correctly in-game.
		m_RootControl->Move(0, 0);

		m_ControlManager->Save(m_Filename);

		// Move the root object back to the workspace position in the editor
		m_RootControl->Move(m_WorkspacePosX, m_WorkspacePosY);

		m_UnsavedChanges = false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnQuitButton() {
		int quitResult = 1;
		if (m_UnsavedChanges) {
			quitResult = GUIEditorUtil::QuitMessageBox("Save changes made?", win_get_window());
			if (quitResult == 1) { OnSaveButton(); }
		}
		m_Quit = (quitResult != 0) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GUIEditorApp::OnWindowResize(RESIZE_DISPLAY_EVENT *resizeInfo) {
		m_EditorBase->Resize(resizeInfo->new_w, resizeInfo->new_h);
		m_LeftColumn->Resize(m_LeftColumn->GetWidth(), resizeInfo->new_h);
		m_RightColumn->Resize(m_RightColumn->GetWidth(), resizeInfo->new_h);
		m_RightColumn->Move(resizeInfo->new_w - m_RightColumn->GetWidth(), 0);
		m_ControlsInActiveCollectionBoxList->Resize(m_ActiveCollectionBoxList->GetWidth(), resizeInfo->new_h - m_ControlsInActiveCollectionBoxList->GetRelYPos() - 5);
		m_WindowResized = true;
	}
}
