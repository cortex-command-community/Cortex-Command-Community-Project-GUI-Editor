#ifndef _RTEGUIEDITORMANAGER_
#define _RTEGUIEDITORMANAGER_

#include "EditorSelection.h"

#include "GUICollectionBox.h"
#include "GUIPropertyPage.h"
#include "GUIListBox.h"

using namespace RTE;

namespace RTEGUI {

	/// <summary>
	/// GUI Editor Manager class that handles the editor controls and the workspace.
	/// </summary>
	class EditorManager {

	public:

#pragma region Creation
		/// <summary>
		/// Constructor method used to instantiate a EditorManager object in system memory and proceeds to initialize it.
		/// </summary>
		/// <param name="screen">The GUI backbuffer this manager's GUIControlManager will be using.</param>
		/// <param name="input">The input handler this manager's GUIControlManager will be using.</param>
		/// <param name="skinDir">The directory containing the skin file to load.</param>
		/// <param name="skinFilename">The file name of the skin file to load.</param>
		EditorManager(GUIScreen *screen, GUIInput *input, const std::string &skinDir, const std::string &skinFilename) { Initialize(screen, input, skinDir, skinFilename); }

		/// <summary>
		/// Initializes the EditorManager and creates all the GUI elements making it ready for use.
		/// </summary>
		/// <param name="screen">The GUI backbuffer this manager's GUIControlManager will be using.</param>
		/// <param name="input">The input handler this manager's GUIControlManager will be using.</param>
		/// <param name="skinDir">The directory containing the skin file to load.</param>
		/// <param name="skinFilename">The file name of the skin file to load.</param>
		void Initialize(GUIScreen *screen, GUIInput *input, const std::string &skinDir, const std::string &skinFilename);
#pragma endregion

#pragma region Getters and Setters
		/// <summary>
		/// Gets the EditorSelection instance that contains information and handles the currently selected element in the workspace.
		/// </summary>
		/// <returns>Reference to the EditorSelection instance.</returns>
		EditorSelection & GetCurrentSelection() const { return s_SelectionInfo; }

		/// <summary>
		/// Gets the GUIControlManager that handles this EditorManager.
		/// </summary>
		/// <returns>Pointer to the GUIControlManager of this manager.</returns>
		GUIControlManager * GetControlManager() const { return m_EditorControlManager.get(); }

		/// <summary>
		/// Gets the GUIControlManager that handles the workspace.
		/// </summary>
		/// <returns>Pointer to the GUIControlManager of the workspace.</returns>
		GUIControlManager * GetWorkspaceManager() const { return m_WorkspaceManager.get(); }

		/// <summary>
		/// Gets the lowest level collection box containing the workspace.
		/// </summary>
		/// <returns>Pointer to the EditorBase GUICollectionBox.</returns>
		GUICollectionBox * GetEditorBase() const { return m_EditorBase.get(); }

		/// <summary>
		/// Gets the left tool column collection box containing the editor controls and the property page.
		/// </summary>
		/// <returns>Pointer to the LeftColumn GUICollectionBox.</returns>
		GUICollectionBox * GetLeftColumn() const { return m_LeftColumn.get(); }

		/// <summary>
		/// Gets the right tool column collection box containing the CollectionBox and child controls lists.
		/// </summary>
		/// <returns>Pointer to the RightColumn GUICollectionBox.</returns>
		GUICollectionBox * GetRightColumn() const { return m_RightColumn.get(); }

		/// <summary>
		/// Gets the panel that displays and allows editing the properties of the currently selected element in the workspace.
		/// </summary>
		/// <returns>Pointer to the PropertyPage GUIPropertyPage.</returns>
		GUIPropertyPage * GetPropertyPage() const { return m_PropertyPage.get(); }

		/// <summary>
		/// Gets the top level (root) Control of the workspace.
		/// </summary>
		/// <returns>Pointer to the RootControl GUIControl.</returns>
		GUIControl * GetRootControl() const { return m_RootControl; }

		/// <summary>
		/// Sets a GUIControl to be used as top level (root) Control of the workspace.
		/// </summary>
		/// <param name="newRootControl">The GUIControl to set as RootControl.</param>
		void SetRootControl(GUIControl *newRootControl) { m_RootControl = newRootControl; }

		/// <summary>
		/// Gets the horizontal position of the workspace in the editor window.
		/// </summary>
		/// <returns>The horizontal position of the workspace in the editor window.</returns>
		int GetWorkspacePosX() const { return m_WorkspacePosX; }

		/// <summary>
		/// Gets the vertical position of the workspace in the editor window.
		/// </summary>
		/// <returns>The vertical position of the workspace in the editor window.</returns>
		int GetWorkspacePosY() const { return m_WorkspacePosY; }

		/// <summary>
		/// Gets the width of the workspace.
		/// </summary>
		/// <returns>The width of the workspace.</returns>
		int GetWorkspaceWidth() const { return m_WorkspaceWidth; }

		/// <summary>
		/// Gets the height of the workspace.
		/// </summary>
		/// <returns>The height of the workspace.</returns>
		int GetWorkspaceHeight() const { return m_WorkspaceHeight; }
#pragma endregion

#pragma region 
		/// <summary>
		/// Disables and hides the workspace zoom checkbox.
		/// </summary>
		void DisableZoomCheckbox() const;

		/// <summary>
		/// 
		/// </summary>
		/// <param name="frameTime"></param>
		void SetFrameTimeLabelText(int64_t frameTime) const;

		/// <summary>
		/// 
		/// </summary>
		void RemoveFocus() const { m_EditorControlManager->GetManager()->SetFocus(nullptr); }
#pragma endregion

#pragma region Control Lists Handling
		/// <summary>
		/// 
		/// </summary>
		void PopulateCollectionBoxList() const;

		/// <summary>
		/// 
		/// </summary>
		void PopulateCollectionBoxChildrenList(GUICollectionBox *collectionBox) const;

		/// <summary>
		/// 
		/// </summary>
		void SelectActiveControlInList(GUIControl *control) const;
#pragma endregion

#pragma region GUI Element Creation
		/// <summary>
		/// Create a new GUI element in the workspace.
		/// </summary>
		/// <param name="editorEvent">The editor event (button press) to create the element from.</param>
		/// <retruns>True to set unsaved changes state.</returns>
		bool AddNewControl(GUIEvent &editorEvent);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="controlToRemove"></param>
		void RemoveControl(const std::string &controlToRemove) const;

		/// <summary>
		/// Generates a name for a new GUI element based on the element type.
		/// </summary>
		/// <param name="strControlType">Control Type.</param>
		/// <returns>String with the new element name.</returns>
		std::string GenerateControlName(std::string controlType) const;
#pragma endregion

#pragma region Mouse Detection
		/// <summary>
		/// Checks if the mouse point is inside a box.
		/// </summary>
		/// <param name="mousePosX">The X position of the mouse.</param>
		/// <param name="mousePosY">The Y position of the mouse.</param>
		/// <param name="boxPosX">The X position of the box.</param>
		/// <param name="boxPosY">The Y position of the box.</param>
		/// <param name="boxWidth">The width of the box.</param>
		/// <param name="boxHeight">The height of the box.</param>
		/// <returns>Whether the mouse is within the box bounds or not.</returns>
		bool MouseInsideBox(int mousePosX, int mousePosY, int boxPosX, int boxPosY, int boxWidth, int boxHeight) const;

		/// <summary>
		/// Checks if a control is under the mouse pointer.
		/// </summary>
		/// <param name="control">The control to check being under the mouse.</param>
		/// <param name="mousePosX">The X position of the mouse.</param>
		/// <param name="mousePosY">The Y position of the mouse.</param>
		/// <returns>The GUIControl that is under the mouse pointer, or nullptr if none.</returns>
		GUIControl * ControlUnderMouse(GUIControl *control, int mousePosX, int mousePosY);

		/// <summary>
		/// Checks if a control's handle is under the mouse pointer.
		/// </summary>
		/// <param name="control">The control whose handle to check being under the mouse.</param>
		/// <param name="mousePosX">The X position of the mouse.</param>
		/// <param name="mousePosY">The Y position of the mouse.</param>
		/// <returns>The index of the handle that is under the mouse pointer, or -1 if none.</returns>
		int HandleUnderMouse(GUIControl *control, int mousePosX, int mousePosY) const;
#pragma endregion

#pragma region Selection Controls
		/// <summary>
		/// Clears the current selection, the selection in the CollectionBoxList and clears the ControlsInCollectionBoxList and property page.
		/// </summary>
		void ClearCurrentSelection() const;
#pragma endregion

#pragma region Updates
		/// <summary>
		/// Updates the list of active top and sub level CollectionBoxes found in the workspace.
		/// </summary>
		void UpdateCollectionBoxList() const;

		/// <summary>
		/// Updates the snap grid size.
		/// </summary>
		/// <param name="editorEvent">The editor event to get update info from.</param>
		void UpdateSnapGridSize(GUIEvent &editorEvent) const;

		/// <summary>
		/// Updates the properties of an element in the workspace.
		/// </summary>
		/// <param name="control">The element to update properties for.</param>
		bool UpdateControlProperties(GUIControl *control) const;

		/// <summary>
		/// Updates the property page in the left column with all the properties of the currently selected element.
		/// </summary>
		/// <param name="editorEvent">The editor event to get update info from.</param>
		bool UpdatePropertyPage(GUIEvent &editorEvent) const;
#pragma endregion

	private:

		static EditorSelection s_SelectionInfo; //!< EditorSelection instance that contains the information of the currently selected element in the workspace.

		std::unique_ptr<GUIControlManager> m_EditorControlManager = nullptr; //!< The GUIControlManager that handles this EditorManager.
		std::unique_ptr<GUICollectionBox> m_EditorBase = nullptr; //!< The lowest level collection box containing the workspace.
		std::unique_ptr<GUICollectionBox> m_LeftColumn = nullptr; //!< The left tool column collection box containing the editor controls and the property page.
		std::unique_ptr<GUICollectionBox> m_RightColumn = nullptr; //!< The right tool column collection box containing the CollectionBox and child controls lists.
		std::unique_ptr<GUIPropertyPage> m_PropertyPage = nullptr; //!< Panel that displays and allows editing the properties of the currently selected element in the workspace.
		std::unique_ptr<GUIListBox> m_CollectionBoxList = nullptr; //!< List box containing all the active top and sub level CollectionBoxes in the workspace.
		std::unique_ptr<GUIListBox> m_ControlsInCollectionBoxList = nullptr; //!< List box containing all the controls that are children of the currently selected CollectionBox.

		std::unique_ptr<GUIControlManager> m_WorkspaceManager = nullptr; //!< The GUIControlManager that handles the workspace.

		GUIControl *m_RootControl = nullptr; //!< The top level (root) Control of the workspace.

		int m_WorkspacePosX = 320; //!< Horizontal position of the workspace in the editor window.
		int m_WorkspacePosY = 60; //!< Vertical position of the workspace in the editor window.
		int m_WorkspaceWidth = 640; //!< Width of the workspace.
		int m_WorkspaceHeight = 480; //!< Height of the workspace.

		// Disallow the use of some implicit methods.
		EditorManager(const EditorManager &reference) = delete;
		EditorManager &operator=(const EditorManager &rhs) = delete;
	};
}
#endif