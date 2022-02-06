#ifndef _GUICONTROLFACTORY_
#define _GUICONTROLFACTORY_

namespace RTE {

	/// <summary>
	/// A class used to create the different controls based on name.
	/// </summary>
	class GUIControlFactory {

	public:

		/// <summary>
		/// Method used for creating controls.
		/// </summary>
		/// <param name="owningManager"></param>
		/// <param name="controlType">Control Type Name.</param>
		/// <returns></returns>
		static GUIControl * CreateControl(GUIControlManager *owningManager, const std::string_view &controlType);
	};
};
#endif