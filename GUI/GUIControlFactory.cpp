#include "GUI.h"
#include "GUIControlFactory.h"
#include "GUIButton.h"
#include "GUICheckbox.h"
#include "GUICollectionBox.h"
#include "GUIComboBox.h"
#include "GUILabel.h"
#include "GUIListBox.h"
#include "GUIProgressBar.h"
#include "GUIPropertyPage.h"
#include "GUIRadioButton.h"
#include "GUIScrollbar.h"
#include "GUISlider.h"
#include "GUITab.h"
#include "GUITextBox.h"

namespace RTE {

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	GUIControl * GUIControlFactory::CreateControl(GUIControlManager *owningManager, const std::string_view &controlType) {
		if (controlType == "BUTTON") {
			return new GUIButton(owningManager);
		} else if (controlType == "CHECKBOX") {
			return new GUICheckbox(owningManager);
		} else if (controlType == "COLLECTIONBOX") {
			return new GUICollectionBox(owningManager);
		} else if (controlType == "COMBOBOX") {
			return new GUIComboBox(owningManager);
		} else if (controlType == "LABEL") {
			return new GUILabel(owningManager);
		} else if (controlType == "LISTBOX") {
			return new GUIListBox(owningManager);
		} else if (controlType == "PROGRESSBAR") {
			return new GUIProgressBar(owningManager);
		} else if (controlType == "PROPERTYPAGE") {
			return new GUIPropertyPage(owningManager);
		} else if (controlType == "RADIOBUTTON") {
			return new GUIRadioButton(owningManager);
		} else if (controlType == "SCROLLBAR") {
			return new GUIScrollbar(owningManager);
		} else if (controlType == "SLIDER") {
			return new GUISlider(owningManager);
		} else if (controlType == "TAB") {
			return new GUITab(owningManager);
		} else if (controlType == "TEXTBOX") {
			return new GUITextBox(owningManager);
		}
		return nullptr;
	}
}