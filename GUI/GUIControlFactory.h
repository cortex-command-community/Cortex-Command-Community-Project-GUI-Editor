#ifndef _GUICONTROLFACTORY_
#define _GUICONTROLFACTORY_

namespace RTE {

	class GUIProperties;

/// <summary>
/// A class used to create the different controls based on name.
/// </summary>
class GUIControlFactory {

public:

//////////////////////////////////////////////////////////////////////////////////////////
// Method:          CreateControl
//////////////////////////////////////////////////////////////////////////////////////////
// Description:     Method used for creating controls
// Arguments:       Control Type Name.

    static GUIControlBase * CreateControl(GUIControlManager *owningManager, const std::string_view &controlType, const std::string &name, int posX, int posY, int width, int height);

	static GUIControlBase * CreateControl(GUIControlManager *owningManager, const std::string_view &controlType, GUIProperties *reference);

};
};
#endif