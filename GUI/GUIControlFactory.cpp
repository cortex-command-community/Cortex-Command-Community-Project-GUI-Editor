#include "GUI.h"

#include "GUIButton.h"
#include "GUIListBox.h"
#include "GUICheckbox.h"
#include "GUIRadioButton.h"
#include "GUITab.h"
#include "GUICollectionBox.h"
#include "GUIProgressBar.h"
#include "GUISlider.h"
#include "GUITextBox.h"
#include "GUILabel.h"
#include "GUIScrollbar.h"
#include "GUIComboBox.h"
#include "GUIPropertyPage.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControlBase * GUIControlFactory::CreateControl(GUIControlManager *ControlManager, const std::string &ControlName) {
		// Button
		if (ControlName == "BUTTON") {
			return new GUIButton(ControlManager);
		}
		// Listbox
		if (ControlName == "LISTBOX") {
			return new GUIListBox(ControlManager);
		}
		// Checkbox
		if (ControlName == "CHECKBOX") {
			return new GUICheckbox(ControlManager);
		}
		// RadioButton
		if (ControlName == "RADIOBUTTON") {
			return new GUIRadioButton(ControlManager);
		}
		// Tab
		if (ControlName == "TAB") {
			return new GUITab(ControlManager);
		}
		// CollectionBox
		if (ControlName == "COLLECTIONBOX") {
			return new GUICollectionBox(ControlManager);
		}
		// ProgressBar
		if (ControlName == "PROGRESSBAR") {
			return new GUIProgressBar(ControlManager);
		}
		// Slider
		if (ControlName == "SLIDER") {
			return new GUISlider(ControlManager);
		}
		// TextBox
		if (ControlName == "TEXTBOX") {
			return new GUITextBox(ControlManager);
		}
		// Label
		if (ControlName == "LABEL") {
			return new GUILabel(ControlManager);
		}
		// Scrollbar
		if (ControlName == "SCROLLBAR") {
			return new GUIScrollbar(ControlManager);
		}
		// ComboBox
		if (ControlName == "COMBOBOX") {
			return new GUIComboBox(ControlManager);
		}
		// PropertyPage
		if (ControlName == "PROPERTYPAGE") {
			return new GUIPropertyPage(ControlManager);
		}
		// Not Found
		return nullptr;
	}
}