#include "EditorManager.h"
#include "GUIButton.h"
#include "GUICheckbox.h"
#include "GUILabel.h"
#include "GUITextBox.h"
#include "GUIComboBox.h"
#include "GUIProgressBar.h"
#include "GUIRadioButton.h"
#include "GUISlider.h"
#include "GUITab.h"

#include "allegro.h"

namespace RTEGUI {

	EditorSelection EditorManager::s_SelectionInfo;
	EditorManager::EditorSelectionCopyInfo EditorManager::s_SelectionCopyInfo;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::Initialize(GUIScreen *screen, GUIInput *input, const std::string &skinDir, const std::string &skinFilename) {
		m_EditorControlManager = std::make_unique<GUIControlManager>();
		m_EditorControlManager->Create(screen, input, skinDir, skinFilename);
		m_EditorControlManager->EnableMouse();

		m_WorkspaceManager = std::make_unique<GUIControlManager>();
		m_WorkspaceManager->Create(screen, input, skinDir, skinFilename);

		m_EditorBase.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("EditorBase", "COLLECTIONBOX", nullptr, 0, 0, screen->GetBitmap()->GetWidth(), screen->GetBitmap()->GetHeight())));
		m_EditorBase->SetDrawBackground(true);
		m_EditorBase->SetDrawColor(makecol(32, 32, 32));
		m_EditorBase->SetDrawType(GUICollectionBox::DrawType::Color);

		m_LeftColumn.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("LeftColumn", "COLLECTIONBOX", nullptr, 0, 0, 285, screen->GetBitmap()->GetHeight())));
		m_LeftColumn->SetDrawBackground(true);
		m_LeftColumn->SetDrawColor(makecol(23, 23, 23));
		m_LeftColumn->SetDrawType(GUICollectionBox::DrawType::Color);

		GUICollectionBox *editorControls = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("EditorControlsPanel", "COLLECTIONBOX", m_LeftColumn.get(), 0, 30, 270, 155));
		editorControls->SetDrawType(GUICollectionBox::DrawType::Panel);

		GUICollectionBox *elementPanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("NewElementPanel", "COLLECTIONBOX", editorControls, 5, 25, 260, 105));
		elementPanel->SetDrawType(GUICollectionBox::DrawType::Panel);
		GUILabel *newElementLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("NewElementLabel", "LABEL", editorControls, elementPanel->GetRelPosX() + 5, elementPanel->GetRelPosY() - 20, 100, 20));
		newElementLabel->SetText("Add New Element :");

		GUIButton *elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_COLLECTIONBOX", "BUTTON", elementPanel, 5, 5, 80, 20));
		elementButton->SetText("CollectionBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_LISTBOX", "BUTTON", elementPanel, 5, 30, 80, 20));
		elementButton->SetText("ListBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_BUTTON", "BUTTON", elementPanel, 5, 55, 80, 20));
		elementButton->SetText("Button");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_COMBOBOX", "BUTTON", elementPanel, 5, 80, 80, 20));
		elementButton->SetText("ComboBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_LABEL", "BUTTON", elementPanel, 90, 5, 80, 20));
		elementButton->SetText("Label");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_CHECKBOX", "BUTTON", elementPanel, 90, 30, 80, 20));
		elementButton->SetText("CheckBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_TEXTBOX", "BUTTON", elementPanel, 90, 55, 80, 20));
		elementButton->SetText("TextBox");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_RADIOBUTTON", "BUTTON", elementPanel, 90, 80, 80, 20));
		elementButton->SetText("RadioButton");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_SCROLLBAR", "BUTTON", elementPanel, 175, 5, 80, 20));
		elementButton->SetText("ScrollBar");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_SLIDER", "BUTTON", elementPanel, 175, 30, 80, 20));
		elementButton->SetText("Slider");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_PROGRESSBAR", "BUTTON", elementPanel, 175, 55, 80, 20));
		elementButton->SetText("ProgressBar");
		elementButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("C_TAB", "BUTTON", elementPanel, 175, 80, 80, 20));
		elementButton->SetText("Tab");

		GUILabel *gridSizeLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("GridSizeLabel", "LABEL", editorControls, 10, 135, 90, 15));
		gridSizeLabel->SetText("Grid/Nudge Size :");
		GUITextBox *gridSizeTextbox = dynamic_cast<GUITextBox *>(m_EditorControlManager->AddControl("GridSizeTextBox", "TEXTBOX", editorControls, gridSizeLabel->GetRelPosX() + 90, gridSizeLabel->GetRelPosY(), 30, 15));
		gridSizeTextbox->SetText(std::to_string(EditorSelection::s_SnapGridSize));

		GUICheckbox *snapCheckbox = dynamic_cast<GUICheckbox *>(m_EditorControlManager->AddControl("SnapCheckBox", "CHECKBOX", editorControls, gridSizeLabel->GetRelPosX() + 130, gridSizeLabel->GetRelPosY(), 75, 15));
		snapCheckbox->SetText("Snap to Grid");
		snapCheckbox->SetCheck(GUICheckbox::Checked);

		GUICheckbox *zoomCheckBox = dynamic_cast<GUICheckbox *>(m_EditorControlManager->AddControl("ZoomCheckBox", "CHECKBOX", editorControls, snapCheckbox->GetRelPosX() + 85, snapCheckbox->GetRelPosY(), 75, 15));
		zoomCheckBox->SetText("Zoom");
		zoomCheckBox->SetCheck(GUICheckbox::State::Unchecked);

		GUICollectionBox *propertyPagePanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("PropertyPagePanel", "COLLECTIONBOX", m_LeftColumn.get(), 0, editorControls->GetPosY() + editorControls->GetHeight() + 10, 270, 340));
		propertyPagePanel->SetDrawType(GUICollectionBox::DrawType::Panel);

		GUILabel *propertyPageLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("PropertyPageLabel", "LABEL", propertyPagePanel, 10, 5, 110, 20));
		propertyPageLabel->SetText("Element Properties :");
		m_PropertyPage.reset(dynamic_cast<GUIPropertyPage *>(m_EditorControlManager->AddControl("PropertyPage", "PROPERTYPAGE", m_LeftColumn.get(), 5, 220, 260, 310)));

		m_RightColumn.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("RightColumn", "COLLECTIONBOX", nullptr, 955, 0, 285, screen->GetBitmap()->GetHeight())));
		m_RightColumn->SetDrawBackground(true);
		m_RightColumn->SetDrawColor(makecol(23, 23, 23));
		m_RightColumn->SetDrawType(GUICollectionBox::DrawType::Color);

		GUICollectionBox *listsPanel = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("ControlListsPanel", "COLLECTIONBOX", m_RightColumn.get(), 15, 30, m_RightColumn->GetWidth() - 15, 505));
		listsPanel->SetDrawType(GUICollectionBox::DrawType::Panel);

		GUILabel *collectionBoxListLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("CollectionBoxListLabel", "LABEL", listsPanel, 10, 5, 100, 20));
		collectionBoxListLabel->SetText("Active Containers :");
		m_CollectionBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorControlManager->AddControl("CollectionBoxList", "LISTBOX", listsPanel, 5, collectionBoxListLabel->GetHeight() + 5, 260, 230)));
		m_CollectionBoxList->SetMouseScrolling(true);

		// TODO: Add buttons for Z pos changing.
		//GUIButton *collectionBoxChangeZUpButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("CollectionBoxListZChangeUpButton", "BUTTON", listsPanel, 120, 5, 50, 20));
		//GUIButton *collectionBoxChangeZDownButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("CollectionBoxListZChangeDownButton", "BUTTON", listsPanel, 170, 5, 50, 20));

		GUILabel *controlsInCollectionBoxListLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("ControlsInCollectionBoxListLabel", "LABEL", listsPanel, 10, collectionBoxListLabel->GetHeight() + m_CollectionBoxList->GetHeight() + 10, 110, 20));
		controlsInCollectionBoxListLabel->SetText("Container Elements :");
		m_ControlsInCollectionBoxList.reset(dynamic_cast<GUIListBox *>(m_EditorControlManager->AddControl("ControlsInCollectionBoxList", "LISTBOX", listsPanel, 5, controlsInCollectionBoxListLabel->GetHeight() + m_CollectionBoxList->GetRelPosY() + 235, 260, 220)));
		m_ControlsInCollectionBoxList->SetMouseScrolling(true);

		// TODO: Add buttons for Z pos changing.
		//GUIButton *collectionBoxChildChangeZUpButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("CollectionBoxChildrenListZChangeUpButton", "BUTTON", listsPanel, 120, collectionBoxListLabel->GetHeight() + m_CollectionBoxList->GetHeight() + 10, 50, 20));
		//GUIButton *collectionBoxChildChangeZDownButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("CollectionBoxChildrenListZChangeDownButton", "BUTTON", listsPanel, 170, collectionBoxListLabel->GetHeight() + m_CollectionBoxList->GetHeight() + 10, 50, 20));

		// Create the workspace area showing the editing box
		GUICollectionBox *workspace = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("Workspace", "COLLECTIONBOX", m_EditorBase.get(), m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight));
		workspace->SetDrawBackground(true);
		workspace->SetDrawColor(makecol(64, 64, 64));
		workspace->SetDrawType(GUICollectionBox::DrawType::Color);

		m_ToolBar.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("ToolBar", "COLLECTIONBOX", nullptr, 0, 0, screen->GetBitmap()->GetWidth(), 30)));
		m_ToolBar->SetDrawBackground(false);

		GUICollectionBox *toolBarStrip = dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("ToolBarStrip", "COLLECTIONBOX", m_ToolBar.get(), 0, 0, m_ToolBar->GetWidth(), 30));
		toolBarStrip->SetDrawBackground(true);
		toolBarStrip->SetDrawColor(makecol(16, 16, 16));
		toolBarStrip->SetDrawType(GUICollectionBox::DrawType::Color);

		GUILabel *frameTimeLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("FrameTimer", "LABEL", toolBarStrip, 430, 0, 100, 20));
		frameTimeLabel->SetText("Frame Time: 0");

		GUIButton *toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("NewButton", "BUTTON", m_ToolBar.get(), 0, 0, 60, 20));
		toolBarButton->SetText("New");
		toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("LoadButton", "BUTTON", m_ToolBar.get(), 60, 0, 60, 20));
		toolBarButton->SetText("Load");
		toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("AddButton", "BUTTON", m_ToolBar.get(), 120, 0, 60, 20));
		toolBarButton->SetText("Add File");
		toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("SaveButton", "BUTTON", m_ToolBar.get(), 180, 0, 60, 20));
		toolBarButton->SetText("Save");
		toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("SaveAsButton", "BUTTON", m_ToolBar.get(), 240, 0, 60, 20));
		toolBarButton->SetText("Save As");
		toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("TestPanelButton", "BUTTON", m_ToolBar.get(), 300, 0, 60, 20));
		toolBarButton->SetText("Test");
		toolBarButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("QuitButton", "BUTTON", m_ToolBar.get(), 360, 0, 60, 20));
		toolBarButton->SetText("Quit");

		CreateRootControl();
		CreateTestPanel();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::CreateRootControl() {
		GUICollectionBox *rootCollectionBox = dynamic_cast<GUICollectionBox *>(m_WorkspaceManager->AddControl("root", "COLLECTIONBOX", nullptr, m_WorkspacePosX, m_WorkspacePosY, m_WorkspaceWidth, m_WorkspaceHeight));
		rootCollectionBox->SetDrawBackground(false);
		m_RootControl = rootCollectionBox;
		m_CollectionBoxList->AddItem(m_RootControl->GetName());
		m_CollectionBoxList->SetSelectedIndex(0);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::CreateTestPanel() {
		m_TestPanel.reset(dynamic_cast<GUICollectionBox *>(m_EditorControlManager->AddControl("EditorControlTestPanel", "COLLECTIONBOX", m_EditorBase.get(), 0, m_ToolBar->GetPosY() + m_ToolBar->GetHeight(), 190, 510)));
		m_TestPanel->SetDrawType(GUICollectionBox::DrawType::Panel);
		m_TestPanel->SetEnabled(false);
		m_TestPanel->SetVisible(false);

		GUIButton *testButton = dynamic_cast<GUIButton *>(m_EditorControlManager->AddControl("TestButton", "BUTTON", m_TestPanel.get(), 10, 10, 100, 20));
		testButton->SetText("Button");

		GUICheckbox *testCheckbox = dynamic_cast<GUICheckbox *>(m_EditorControlManager->AddControl("TestCheckbox", "CHECKBOX", m_TestPanel.get(), 10, 35, 100, 20));
		testCheckbox->SetText("Checkbox");

		GUIComboBox *testComboBox = dynamic_cast<GUIComboBox *>(m_EditorControlManager->AddControl("TestComboBox", "COMBOBOX", m_TestPanel.get(), 10, 60, 100, 20));
		testComboBox->SetDropHeight(50);
		testComboBox->AddItem("ComboBox");
		testComboBox->AddItem("ComboBox");
		testComboBox->AddItem("ComboBox");
		testComboBox->AddItem("ComboBox");
		testComboBox->SetSelectedIndex(0);

		GUILabel *testLabel = dynamic_cast<GUILabel *>(m_EditorControlManager->AddControl("TestLabel", "LABEL", m_TestPanel.get(), 10, 85, 100, 20));
		testLabel->SetText("Label");

		GUIListBox *testListBox = dynamic_cast<GUIListBox *>(m_EditorControlManager->AddControl("TestListBox", "LISTBOX", m_TestPanel.get(), 10, 110, 100, 60));
		testListBox->AddItem("ListBox");
		testListBox->AddItem("ListBox");
		testListBox->AddItem("ListBox");
		testListBox->AddItem("ListBox");
		testListBox->AddItem("ListBox");

		GUIProgressBar *testProgressBar = dynamic_cast<GUIProgressBar *>(m_EditorControlManager->AddControl("TestProgressBar", "PROGRESSBAR", m_TestPanel.get(), 10, 178, 100, 20));
		testProgressBar->SetValue(50);

		GUIRadioButton *testRadioButton1 = dynamic_cast<GUIRadioButton *>(m_EditorControlManager->AddControl("TestRadioButton1", "RADIOBUTTON", m_TestPanel.get(), 10, 200, 100, 20));
		testRadioButton1->SetText("RadioButton1");
		testRadioButton1->SetCheck(true);

		GUIRadioButton *testRadioButton2 = dynamic_cast<GUIRadioButton *>(m_EditorControlManager->AddControl("TestRadioButton2", "RADIOBUTTON", m_TestPanel.get(), 10, 225, 100, 20));
		testRadioButton2->SetText("RadioButton2");

		GUIScrollbar *testScrollBar1 = dynamic_cast<GUIScrollbar *>(m_EditorControlManager->AddControl("TestScrollBar1", "SCROLLBAR", m_TestPanel.get(), 10, 250, 100, 20));

		GUIScrollbar *testScrollBar2 = dynamic_cast<GUIScrollbar *>(m_EditorControlManager->AddControl("TestScrollBar2", "SCROLLBAR", m_TestPanel.get(), 125, 250, 20, 100));
		testScrollBar2->SetOrientation(GUIScrollbar::Orientation::Vertical);

		GUISlider *testSlider1 = dynamic_cast<GUISlider *>(m_EditorControlManager->AddControl("TestSlider1", "SLIDER", m_TestPanel.get(), 10, 270, 100, 20));

		GUISlider *testSlider2 = dynamic_cast<GUISlider *>(m_EditorControlManager->AddControl("TestSlider2", "SLIDER", m_TestPanel.get(), 150, 250, 20, 100));
		testSlider2->SetOrientation(GUISlider::Orientation::Vertical);

		GUITab *testTab1 = dynamic_cast<GUITab *>(m_EditorControlManager->AddControl("TestTab1", "TAB", m_TestPanel.get(), 10, 295, 100, 20));
		testTab1->SetText("Tab");
		testTab1->SetCheck(true);

		GUITab *testTab2 = dynamic_cast<GUITab *>(m_EditorControlManager->AddControl("TestTab2", "TAB", m_TestPanel.get(), 10, 320, 100, 20));
		testTab2->SetText("Tab");

		GUITextBox *testTextBox1 = dynamic_cast<GUITextBox *>(m_EditorControlManager->AddControl("TestTextBox1", "TEXTBOX", m_TestPanel.get(), 10, 345, 100, 20));

		GUITextBox *testTextBox2 = dynamic_cast<GUITextBox *>(m_EditorControlManager->AddControl("TestTextBox2", "TEXTBOX", m_TestPanel.get(), 10, 370, 100, 20));
		testTextBox2->SetReadOnly(true);
		testTextBox2->SetText("TextBox");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::DisableZoomCheckbox() const {
		m_EditorControlManager->GetControl("ZoomCheckBox")->SetEnabled(false);
		m_EditorControlManager->GetControl("ZoomCheckBox")->SetVisible(false);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SetFrameTimeLabelText(int64_t frameTime) const {
		dynamic_cast<GUILabel *>(m_EditorControlManager->GetControl("FrameTimer"))->SetText("Frame Time: " + std::to_string(frameTime) + "ms");
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorManager::AddNewControl(GUIEvent &editorEvent) {
		if (s_SelectionInfo.GetControl() && s_SelectionInfo.GetControl()->GetControlType() != "COLLECTIONBOX") {
			s_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
		}
		std::string controlClass = editorEvent.GetOrigin()->GetName().substr(2, std::string::npos);
		std::string controlName = GenerateControlName(controlClass);

		GUIControl *parent = m_RootControl;

		// If the focused control is a container set it as parent so controls are added to it
		if (s_SelectionInfo.GetControl() && s_SelectionInfo.GetControl()->IsContainer()) { parent = s_SelectionInfo.GetControl(); }

		if (parent) { m_WorkspaceManager->AddControl(controlName, controlClass, parent, 0, 0, -1, -1); }

		UpdateCollectionBoxList();
		UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(parent));

		return true;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::AddNewControlFromStoredCopyInfo() const {
		if (s_SelectionCopyInfo.Class.empty()) {
			return;
		}
		std::string newControlName;
		for (int i = 1; i < 1000; ++i) {
			newControlName = s_SelectionCopyInfo.Name + " - copy " + std::to_string(i);
			// Check if this name exists
			bool found = false;
			for (GUIControl *control : *m_WorkspaceManager->GetControlList()) {
				if (control->GetName() == newControlName) {
					found = true;
					break;
				}
			}
			if (!found) {
				break;
			}
		}
		int offset = 10;

		GUIControl *createdControl = nullptr;
		createdControl = m_WorkspaceManager->AddControl(newControlName, s_SelectionCopyInfo.Class, s_SelectionCopyInfo.Parent, s_SelectionCopyInfo.PosX + offset, s_SelectionCopyInfo.PosY + offset, s_SelectionCopyInfo.Width, s_SelectionCopyInfo.Height);
		s_SelectionCopyInfo.PosX += offset;
		s_SelectionCopyInfo.PosY += offset;

		GUIProperties properties;
		createdControl->StoreProperties();
		properties.OverwriteProperties(s_SelectionCopyInfo.Properties, true);
		properties.SetPropertyValue("Name", newControlName);
		createdControl->BuildProperties(&properties);
		createdControl->ApplyProperties(&properties);
		m_PropertyPage->SetPropertyValues(&properties);

		s_SelectionInfo.SetControl(createdControl);

		UpdateCollectionBoxList();
		UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(createdControl->GetParent()));

		if (createdControl->GetControlType() == "COLLECTIONBOX") {
			SelectActiveControlInParentList(createdControl);
		} else {
			SelectActiveControlInChildrenList(createdControl);
		}
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::RemoveControl(GUIControl *controlToRemove) const {
		m_WorkspaceManager->RemoveControl(controlToRemove->GetName(), true);
		if (controlToRemove->GetControlType() == "COLLECTIONBOX") {
			ClearCurrentSelection();
			UpdateCollectionBoxList();
		} else {
			s_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
			UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(controlToRemove->GetParent()));
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::string EditorManager::GenerateControlName(std::string controlType) const {
		for (int i = 1; i < 1000; i++) {
			std::string controlName = controlType;
			std::transform(controlName.begin(), controlName.end(), controlName.begin(), tolower);
			controlName.append(std::to_string(i));

			// Check if this name exists
			bool found = false;
			for (GUIControl *control : *m_WorkspaceManager->GetControlList()) {
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

	void EditorManager::SelectActiveControlFromParentList() const {
		if (const GUIListBox::Item *selectedItem = m_CollectionBoxList->GetSelected()) {
			// Try to find the box of that name, and select it
			GUIControl *control = m_WorkspaceManager->GetControl(selectedItem->m_Name.substr(selectedItem->m_Name.find_first_not_of('\t'), std::string::npos));
			if (control) {
				// If the selected item is the root control don't grab it but proceed to populate the children list from it
				if (selectedItem->m_Name == m_RootControl->GetName()) {
					s_SelectionInfo.ClearSelection();
					m_PropertyPage->ClearValues();
				} else {
					s_SelectionInfo.ReleaseAnyGrabs();
					s_SelectionInfo.SetControl(control);
				}
				UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(control));
				UpdateControlProperties(s_SelectionInfo.GetControl());
			}
		} else {
			// Deselection if clicked on no list item
			ClearCurrentSelection();
			// When nothing is selected populate the children list with the root control's children to show any "loose" controls
			UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(m_RootControl));
		}
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlFromChildrenList() const {
		if (const GUIListBox::Item *selectedItem = m_ControlsInCollectionBoxList->GetSelected()) {
			// Try to find the control of that name, and select it
			GUIControl *control = m_WorkspaceManager->GetControl(selectedItem->m_Name);
			if (control) {
				s_SelectionInfo.ReleaseAnyGrabs();
				s_SelectionInfo.SetControl(control);
				UpdateControlProperties(s_SelectionInfo.GetControl());
			}
		} else {
			// Deselection if clicked on no list item
			s_SelectionInfo.ClearSelection();
			m_PropertyPage->ClearValues();
		}
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlInParentList(GUIControl *control) const {
		// Check if this is selected in the editor and select it in the list too
		for (const GUIListBox::Item *listEntry : *m_CollectionBoxList->GetItemList()) {
			if (listEntry->m_Name.substr(listEntry->m_Name.find_first_not_of('\t'), std::string::npos) == control->GetName()) {
				m_CollectionBoxList->SetSelectedIndex(listEntry->m_ID);
				break;
			}
		}
		UpdateCollectionBoxChildrenList(dynamic_cast<GUICollectionBox *>(control));
		m_ControlsInCollectionBoxList->SetSelectedIndex(-1);
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::SelectActiveControlInChildrenList(GUIControl *control) const {
		// Check if this is selected in the editor and select it's parent in the parent list and then select it in the children list
		SelectActiveControlInParentList(control->GetParent());
		for (const GUIListBox::Item *listEntry : *m_ControlsInCollectionBoxList->GetItemList()) {
			if (listEntry->m_Name == control->GetName()) {
				m_ControlsInCollectionBoxList->SetSelectedIndex(listEntry->m_ID);
				break;
			}
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::UpdateCollectionBoxList() const {
		// Store the last collection box that was selected before the update so it can be selected if it still exists, if no selection then select root
		std::string lastSelection = (m_CollectionBoxList->GetSelectedIndex() != -1) ? m_CollectionBoxList->GetSelected()->m_Name : "";

		m_CollectionBoxList->ClearList();
		m_CollectionBoxList->AddItem(m_RootControl->GetName());

		// Lambda expression to recursively add lower-level CollectionBoxes belonging to the higher-level CollectionBoxes
		std::function<void(GUICollectionBox *, const std::string &)> recursiveAddItem = [&recursiveAddItem, this](GUICollectionBox *control, const std::string &indent) {
			m_CollectionBoxList->AddItem(indent + control->GetName());
			for (GUIControl *childControl : *control->GetChildren()) {
				if ((control = dynamic_cast<GUICollectionBox *>(childControl))) { recursiveAddItem(control, indent + "\t"); }
			}
		};

		GUICollectionBox *collectionBox = nullptr;
		for (GUIControl *control : *m_WorkspaceManager->GetContainerList()) {
			if ((collectionBox = dynamic_cast<GUICollectionBox *>(control)) && collectionBox->GetParent() == m_RootControl) { recursiveAddItem(collectionBox, "\t"); }
		}

		for (const GUIListBox::Item *listEntry : *m_CollectionBoxList->GetItemList()) {
			if (listEntry->m_Name == lastSelection) {
				m_CollectionBoxList->SetSelectedIndex(listEntry->m_ID);
				break;
			}
			m_CollectionBoxList->SetSelectedIndex(0);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::UpdateCollectionBoxChildrenList(GUICollectionBox *collectionBox) const {
		if (!collectionBox) {
			return;
		}
		m_ControlsInCollectionBoxList->ClearList();

		// Go through all the top-level (directly under root) controls and add only the CollectionBoxes to the list here
		for (GUIControl *control : *collectionBox->GetChildren()) {
			if (control->GetControlType() != "COLLECTIONBOX") { m_ControlsInCollectionBoxList->AddItem(control->GetName()); }
			// Check if this is selected in the editor, and if so, select it in the list too
			if (collectionBox == s_SelectionInfo.GetControl()) { m_ControlsInCollectionBoxList->SetSelectedIndex(-1); }
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorManager::MouseInsideBox(int mousePosX, int mousePosY, int boxPosX, int boxPosY, int boxWidth, int boxHeight) const {
		return (mousePosX >= boxPosX && mousePosX <= boxPosX + boxWidth && mousePosY >= boxPosY && mousePosY <= boxPosY + boxHeight) ? true : false;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * EditorManager::ControlUnderMouse(GUIControl *control, int mousePosX, int mousePosY) {
		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);
		if (!MouseInsideBox(mousePosX, mousePosY, controlPosX, controlPosY, controlWidth, controlHeight)) {
			return nullptr;
		}

		// Check children. Check in reverse because top most visible control is last in the list.
		for (std::vector<GUIControl *>::reverse_iterator childListEntry = control->GetChildren()->rbegin(); childListEntry != control->GetChildren()->rend(); childListEntry++) {
			GUIControl *childControl = ControlUnderMouse(*childListEntry, mousePosX, mousePosY);
			if (childControl) {
				return childControl;
			}
		}
		return control;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	int EditorManager::HandleUnderMouse(GUIControl *control, int mousePosX, int mousePosY) const {
		int controlPosX;
		int controlPosY;
		int controlWidth;
		int controlHeight;
		control->GetControlRect(&controlPosX, &controlPosY, &controlWidth, &controlHeight);

		int regionSize = 6;
		int handle = 0;

		for (int i = 0; i < 3; i++) {
			if (MouseInsideBox(mousePosX, mousePosY, controlPosX - regionSize, controlPosY + i * (controlHeight / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (i != 1 && MouseInsideBox(mousePosX, mousePosY, controlPosX + controlWidth / 2 - regionSize, controlPosY + i * (controlHeight / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;

			if (MouseInsideBox(mousePosX, mousePosY, controlPosX + controlWidth - regionSize, controlPosY + i * (controlHeight / 2) - regionSize, regionSize * 2, regionSize * 2)) {
				return handle;
			}
			handle++;
		}
		// Not over any handle
		return -1;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::ClearCurrentSelection() const {
		s_SelectionInfo.ClearSelection();
		m_PropertyPage->ClearValues();
		m_CollectionBoxList->SetSelectedIndex(0);
		m_ControlsInCollectionBoxList->ClearList();

		// Clear focused control of the manager itself so it doesn't persist between selection changes (e.g property page line remains selected after clearing or changing selection)
		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::StoreCurrentSelectionCopyInfo() const {
		GUIControl *selectedControl = s_SelectionInfo.GetControl();
		if (selectedControl) {
			s_SelectionCopyInfo = {
				selectedControl->GetName(),
				std::string(selectedControl->GetControlType()),
				selectedControl->GetRelPosX(),
				selectedControl->GetRelPosY(),
				selectedControl->GetWidth(),
				selectedControl->GetHeight(),
				selectedControl->GetParent(),
				selectedControl->GetProperties()
			};
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void EditorManager::UpdateSnapGridSize(GUIEvent &editorEvent) const {
		std::string newValue = dynamic_cast<GUITextBox *>(editorEvent.GetOrigin())->GetText();
		if (newValue.empty()) { newValue = "1"; }

		bool validEntry = true;
		for (const char &stringChar : newValue) {
			if (!std::isdigit(stringChar)) {
				validEntry = false;
				break;
			}
		}
		EditorSelection::s_SnapGridSize = validEntry ? std::clamp(std::stoi(newValue), 0, 255) : EditorSelection::s_SnapGridSize;
		dynamic_cast<GUITextBox *>(editorEvent.GetOrigin())->SetText(std::to_string(EditorSelection::s_SnapGridSize));

		RemoveFocus();
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool EditorManager::UpdateControlProperties(GUIControl *control, bool manualEdit) const {
		bool result = false;
		if (control) {
			if (manualEdit) { control->ApplyProperties(m_PropertyPage->GetPropertyValues()); }
			control->StoreProperties();
			GUIProperties properties;
			properties.OverwriteProperties(control->GetProperties(), true);
			control->BuildProperties(&properties);
			m_PropertyPage->SetPropertyValues(&properties);
			result = true;
		}
		return result;
	}
}